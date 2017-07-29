/**
\brief Implements generic Zip function.

Zip function simply applies provided binary function for each child
of two specified objects sequentially.
I.e. first it applies the function to the first child of left object 
and the first child of the second, then to the second children and so on.
An example of zip function is equality. Where we function to apply is 
usual (==) operator.
*/
#ifndef SYB_ZIP_H
#define SYB_ZIP_H

#if 0
#include "Syb/StlSyb.h"
#include "Syb/Syb.h"
#include "Syb/Poly.h"
#include "Syb/DynRepr.h"
#include "Syb/Trace.h"
#else
#include "StlSyb.h"
#include "Syb.h"
#include "Poly.h"
#include "DynRepr.h"
#include "Trace.h"
#endif
namespace qx{

  namespace syb {

#if 0
    namespace Priv {
      template<typename FtrT, typename T, typename EnableT = void>
      struct ZipHelper;
    }

    template<typename FtrT, typename EnableT=void>
    struct ZipHook {
      template<typename T, typename U>
      inline static void action(FtrT const& f, T& l, U& r) {
        f(l,r);
      }
    };

    template<typename FtrT, typename ParT, typename EnableT=void>
    struct PolyZipHook:ZipHook<FtrT>{};

    template<typename ParT>
    struct PolyZipMember {
      template<typename FtrT, typename T, typename U>
      inline static void action(FtrT const& f, T& l, U& r) {
        f.polyZip(l,r);
      }
    };

    template<
      typename TQ, 
      typename UQ, 
      typename T = typename boost::remove_const<TQ>::type,
      typename U = typename boost::remove_const<TQ>::type ,
      typename EnableT=void
    >
    struct JoinType:mpl::false_{};

    template<typename TQ, typename UQ, typename T, typename U>
    struct JoinType<TQ,UQ,T,U,typename boost::enable_if<
      boost::is_same<T,U>
    >::type>:mpl::true_
    {
      template<typename F>
      inline static F& convert(F& v) { return v; }
    };

    template<typename TQ, typename UQ, typename T, typename U>
    struct JoinType<TQ,UQ,boost::shared_ptr<T>,boost::shared_ptr<U>,
      typename boost::enable_if<mpl::and_<
        boost::is_base_of<T,U>
        ,mpl::not_<boost::is_same<T,U> >
      > >::type>:mpl::true_
    {
      inline static TQ& convert(TQ& v) { return v; }
      inline static boost::shared_ptr<T> convert(UQ& v) { 
        return boost::static_pointer_cast<T>(v); 
      }
    };

    template<typename TQ, typename UQ, typename T, typename U>
    struct JoinType<TQ,UQ,boost::shared_ptr<T>,boost::shared_ptr<U>,
      typename boost::enable_if<mpl::and_<
        boost::is_base_of<U,T>
        ,mpl::not_<boost::is_same<T,U> >
      > >::type>:mpl::true_
    {
      inline static UQ& convert(UQ& v) { return v; }
      inline static boost::shared_ptr<U> convert(TQ& v) { 
        return boost::static_pointer_cast<U>(v); 
      }
    };

    /**
    \brief Generic zip function
    */
    template<typename FtrT>
    struct Zip:Visitor<Zip<FtrT> >
    {
      typedef std::list<boost::any>::iterator fld_iter_t;
      inline explicit Zip(FtrT const& f, fld_iter_t& i, fld_iter_t const& e)
        :ftr_(f),current_(i),end_(e){}
#ifdef NOT_INLINES
      template<typename T>
      inline void operator()(T& l) const {
        notInline(*this,l);
      }

      template<typename T>
      inline void runInline(T& l) const {
        run<ZipHook<FtrT> >(l);
      }
#else
      template<typename T>
      inline void operator()(T& l) const {
        run<ZipHook<FtrT> >(l);
      }
#endif
/*
      template<typename ParT,typename T>
      inline void polyVisit(T& l) const {
        run<PolyZipHook<FtrT,ParT> >(l);
      }
*/
    private:

      template<typename RedirT, typename T>
      inline void run(T& l) const {
        if (current_ == end_) return;
        typedef typename boost::remove_const<T>::type t;
        typedef boost::reference_wrapper<t const> cref_t;
        typedef boost::reference_wrapper<t> ref_t;
        ref_t* r = boost::any_cast<ref_t>(&(*current_));
        if (r)
          RedirT::action(ftr_,l,r->get());
        else
          RedirT::action(ftr_,l,boost::any_cast<cref_t>(*current_).get());
        ++current_;
      }

      FtrT const& ftr_;
      fld_iter_t& current_;
      fld_iter_t end_;
    };
 
    namespace Priv {
      template<
        typename FtrT
        , typename T
        , typename EnableT
      > struct ZipHelper {
        template<typename L, typename R>
        inline static void action(FtrT const& f, L& l, R& r) {
          f(l,r);
        }
      };
    }

    //TODO: unfortunately here we need to use dynamic field representation
    // we can use static actually but this will significantly increase 
    // C++ compiler load, the other option is to used modified Syb library
    // where we traverse phantom objects rather than actual objects.
    // This is one of future SYB ideas, it will resolve the current issue.
    /**
    \brief Helper for running generic Zip function
    */
    template<typename FtrT, typename T, typename U>
    inline typename boost::enable_if<JoinType<T,U> >::type 
      zip(FtrT const& fun, T& l, U& r)
    {
      std::list<boost::any> ri;
      getFields(JoinType<T,U>::convert(r),ri);
      std::list<boost::any>::iterator i = ri.begin();
      Zip<FtrT> zf(fun,i,ri.end());
      gmap(JoinType<T,U>::convert(l),zf,Unit());
    }

#else

    template<typename FtrT, typename U, typename EnT=void>
    struct ZipHelper;

    template<typename FtrT, typename T, typename U>
    void zip(FtrT const& f, T& l, U& r) {
      typedef typename UnAlias<T>::type a_t;
      ZipHelper<FtrT,typename mpl::if_<boost::is_const<T>,a_t const,a_t>::type>
        ::action(f,unAlias(l),unAlias(r));
    }


    template<typename FtrT>
    struct Zip {
      typedef typename EntPtr<FtrT::is_left_const>::type l_t;
      typedef typename EntPtr<FtrT::is_right_const>::type r_t;

      inline Zip(FtrT const& f, l_t l, r_t r):ftr_(f),left_(l),right_(r){}

      template<typename FldT>
      inline void operator()(FieldRedir<FldT> const& f) const {
        ftr_(f(left_),f(right_));
      }

    private:
      FtrT const& ftr_;
      l_t left_;
      r_t right_;
    };

    template<typename FtrT, typename U, typename EnT>
    struct ZipHelper {
      template<typename U1, typename U2>
      inline static void action(FtrT const& f, U1& l, U2& r) {
      }
      template<typename U1, typename U2>
      inline static void action(FtrT const& f, 
        boost::shared_ptr<U1> const& l, boost::shared_ptr<U2> const& r) 
      {
        if (!l || !r)
          return;
        zip(f,*l,*r);
      }
    };
    
    template<typename FtrT, typename U>
    struct ZipHelper<FtrT,U,typename boost::enable_if<
      EmbeddedInfo<U>
      >::type>
    {
      template<typename U2>
      inline static void action(FtrT const& f, U& l, U2& r) {
        typedef typename EntPtr<FtrT::is_left_const>::type lr_t;
        typedef typename EntPtr<FtrT::is_right_const>::type rr_t;
        gmapAccessors<U>(Zip<FtrT>
          (f,reinterpret_cast<lr_t>(&l),reinterpret_cast<rr_t>(&r)));
      }
    };

    template<typename FtrT, typename UL, typename UR>
    inline void contZip(FtrT const& f, UL& l, UR& r) {
      typedef typename mpl::if_<
          boost::is_const<UL>,
          typename UL::const_iterator,
          typename UL::iterator
        >::type l_iter_t;
      typedef typename mpl::if_<
          boost::is_const<UR>,
          typename UR::const_iterator,
          typename UR::iterator
        >::type r_iter_t;
      l_iter_t li = l.begin();
      l_iter_t ri = r.begin();
      for(;li!=l.end() && ri!=r.end();++li,++ri) {
        f(*li,*ri);
      }
    }

    template<typename FtrT, typename UL>
    struct ZipHelper<FtrT,UL,typename boost::enable_if<
      IsCont<typename boost::remove_const<UL>::type> 
    >::type> {
      template<typename UR>
      inline static void action(FtrT const& f,UL& l, UR& r) {
        contZip(f,l,r);
      }
    };

    template<typename FtrT, typename U>
    struct PolyZip {

      typedef typename mpl::if_c<FtrT::is_left_const,U const,U>::type ul_t;
      typedef typename mpl::if_c<FtrT::is_right_const,U const,U>::type ur_t;

      inline PolyZip(FtrT const& f, ul_t& l, ur_t& r)
        :ftr_(f),left_(l),right_(r) {}

      template<typename T>
      inline void visit(Empty<T> const& e) const {
        // ftr_(downCast(left_,e),downCast(right_,e));
        zip(ftr_,downCast(left_,e),downCast(right_,e));
      }
    private:
      FtrT const& ftr_;
      ul_t& left_;
      ur_t& right_;
    };

    template<typename FtrT, typename UL, typename UR>
    void polyZip(FtrT const& f, UL& l, UR& r, 
      ConstrInfo<typename boost::remove_const<UL>::type> const& c) 
    {
      c.accept(PolyZip<FtrT,typename boost::remove_const<UL>::type>(f,l,r));
    }

    template<typename FtrT, typename U>
    struct ZipHelper<FtrT,U,typename boost::enable_if<
      IsMultiConstr<typename boost::remove_const<U>::type> 
    >::type> {
      template<typename U2>
      inline static void action(FtrT const& f,U& l, U2& r) {
        typedef typename boost::remove_const<U>::type u_t;
        DataType<u_t> const& dt = dataType<u_t>();
        if (!dt.hasConstructor(l) || !dt.hasConstructor(r)) 
          return;
        ConstrInfo<u_t> const& c = dt.fromValue(l);
        if (c == dt.fromValue(r)) {
          polyZip(f,l,r,c);
          // c.accept(PolyZip<FtrT,U,U2>(f,l,r));
        }
      }
    };

#endif
  }
}

#endif
