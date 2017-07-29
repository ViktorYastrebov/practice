/**
\brief Facilities for erasing static type information, similar to boost::any
(and actually this is an extension for it), void*, java or C# "Object" class.

*/
#ifndef SYB_ANY_H
#define SYB_ANY_H
#define SYB_USE_BOOST_ANY

#include <boost/cast.hpp>
#include <boost/any.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>
#if 0
#include "Syb/Syb.h"
#include "Syb/TypeAlias.h"
#else
#include "Syb.h"
#include "TypeAlias.h"
#endif 

namespace qx {
  namespace syb {

    struct AbsDataType;

    struct Any;

    struct AnyRedir;

    template<typename T,typename EnT=void> struct DataType;

    template<typename T>
    DataType<T> const& dataType();

    template<typename CtxT>
    struct AbsAnyFun {
      typedef CtxT ctx_t;
      virtual ~AbsAnyFun() {}
    };

    /**
    \brief Returns a pointer to a dynamic function.
    The function object is global (stored in static variable)

    This stop instantiation of shared_ptr constructors
    we loose type safety for binary sizes and compilation speed
    but it checked with boost::polymorphic_downcast anyway
    */
    template<typename FunT>
    inline boost::shared_ptr<AbsAnyFun<typename FunT::ctx_t> > const& createAnyFun() {
      typedef boost::shared_ptr<AbsAnyFun<typename FunT::ctx_t> > ret_t;
      static ret_t ret = boost::make_shared<FunT>();
      return ret;
    }

    /**
    \brief Returns a pointer to a dynamic function.
    Returns a new copy of an object constructed with parameter prm
    */
    template<typename FunT, typename PrmT>
    inline boost::shared_ptr<AbsAnyFun<typename FunT::ctx_t> > createAnyFun(PrmT const& p) {
      return boost::make_shared<FunT>(p);
    }

    /**
    \brief Replacement for boost::function<void(CtxT&,T const&)>
    which blows up size of result
    */
    template<typename CtxT, typename T>
    struct AnyFun : AbsAnyFun<CtxT> {
      virtual void action(CtxT&,T const&) const = 0;
    };

    /**
    \brief Lazily initialize value of Any type after its type is known
    */
    struct AbsAnyInitializer {
      virtual void run(Any&) const = 0;
      virtual ~AbsAnyInitializer();
    };

    /**
    \brief Replacement for boost::function<void(Any const& v)>
    */
    struct DynFun {
      virtual void action(Any const&) const = 0;
    };

    typedef boost::shared_ptr<AbsAnyInitializer> AnyInitializerPtr;
    typedef DynFun const* dyn_fun_t;

    struct AbsXmlInfo;
    struct AbsDataType;

    struct AbsDynInfo {
      inline AbsDynInfo(AbsXmlInfo const& x, AbsDataType const& d)
        :xmlInfo_(x),dataType_(d){}
      inline AbsXmlInfo const& getXmlInfo() const {
        return xmlInfo_;
      }
      inline AbsDataType const& getDataType() const {
        return dataType_;
      }
    private:
      AbsDataType const& dataType_;
      AbsXmlInfo const& xmlInfo_;
    };

    /**
    \brief Additional information for generic functions to be defined
    on static config.
    */
    template<typename T>
    AbsDynInfo const& dynInfo();

    /**
    \brief Class for storing a value of any type (extension of boost::any)

    The class adds a type dispatching function to boost::any. This is similar
    to how function dispatching works for virtual function. On caller side
    actual type of object is not known, but inside the function its type is
    known and object of actual type can be accessed by implicit "this" parameter.
    */
    struct Any {


      /// \brief Inits value with any type and it's dispatching function
      /// \todo memory leaks are possible, ensure no exceptions are thrown
      /// on a way to redir (boost::shared_ptr blows up executables size)
      template<typename T, typename CtxT>
      inline Any(T const& v, boost::shared_ptr<AbsAnyFun<CtxT> > const&  f)
        :val_(v)
        ,redir_(boost::make_shared<Redir<CtxT,T> >(f))
        ,dynInfo_(NULL)
      {}

      template<typename T, typename CtxT>
      inline Any(
        T const& v,
        boost::shared_ptr<AbsAnyFun<CtxT> > const&  f,
        AbsDynInfo const& d
        )
        :val_(v)
        ,redir_(boost::make_shared<Redir<CtxT,T> >(f))
        ,dynInfo_(&d)
      {}


      inline Any() {}

      /// \brief Resets value and dispatching function
      template<typename CtxT, typename T>
      inline void set(T const& v, boost::shared_ptr<AbsAnyFun<CtxT> > const& f) {
        val_ = v;
        redir_.reset(static_cast<AbsRedir*>(new Redir<CtxT,T>(f)));
      }

      /// \brief Resets just value
      template<typename T>
      inline void set(T const& v) {
        val_ = v;
      }

      template<typename T>
      inline void set(T const& v, AbsDynInfo const& d) {
        val_ = v;
        dynInfo_  = &d;
      }

      template<typename CtxT, typename T>
      inline void set(
        T const& v,
        boost::shared_ptr<AbsAnyFun<CtxT> > const& f,
        AbsDynInfo const& d)
      {
        val_ = v;
        redir_.reset(static_cast<AbsRedir*>(new Redir<CtxT,T>(f)));
        dynInfo_ = &d;
      }

      /** \brief Returns an object debug mode assert if an object with a wrong
                 type is held
      */
      template<typename T>
      inline T const* get() const {
        assert(!val_.empty());
        return boost::any_cast<T>(&val_);
      }

      inline void setInitializer(AnyInitializerPtr const& i) {
        initializer_ = i;
      }

      inline AnyInitializerPtr const& getInitializer() const {
        return initializer_;
      }

      inline AnyInitializerPtr& getInitializer() {
        return initializer_;
      }

      /**
      \brief Runs dispatching function using stored value as its second
      argument and x as its first.

      If we continue using virtual function analogy this function is similar
      to the actual virtual function's call. Stored value is implicit "this"
      parameter, and "x" is the rest of the function's arguments. If there're
      more than one arguments std::pair, boost::tuple, boost::fusion or
      any C++ class can be used.
      */
      template<typename CtxT>
      inline void run(CtxT& x) const {
        if (redir_)
          redir_->action(reinterpret_cast<const boost::any&>(x),val_);
      }

      inline void setDynInfo(AbsDynInfo const& d) {
        dynInfo_ = &d;
      }

      inline AbsDynInfo const& getDynInfo() const {
        assert(dynInfo_);
        return *dynInfo_;
      }

      inline bool empty() const {
        return val_.empty();
      }

      inline bool operator==(Any const& other) const {
        return redir_.get() == other.redir_.get();
      }

      inline bool operator!=(Any const& other) const {
        return !(operator==(other));
      }

    private:

      struct AbsRedir {
        virtual void action(boost::any const& x, boost::any const& v) = 0;
      };

      boost::any val_;
      boost::shared_ptr<AbsRedir> redir_;
      AbsDynInfo const* dynInfo_;
      AnyInitializerPtr initializer_;

     // std::string typeName_;
      TRAVERSABLE_BEGIN(Any)
      TRAVERSABLE_END

      template<typename CtxT, typename T>
      struct Redir : AbsRedir {
        typedef AnyFun<CtxT,T>* fun_t;
        inline explicit Redir(boost::shared_ptr<AbsAnyFun<CtxT> > const& f)
          :fun_(f.get()){}
        void action(boost::any const& x, boost::any const& v) {
          T const* vp = boost::any_cast<T>(&v);
          assert(vp);
          const CtxT *ctx = reinterpret_cast<CtxT*>(&x);
          assert(ctx);
          assert(fun_);
          fun_t f = boost::polymorphic_downcast<fun_t>(fun_);
          f->action(*ctx,*vp);
        }
      private:
        AbsAnyFun<CtxT> *fun_;
      };
    };

    typedef boost::shared_ptr<DynFun> dyn_fun_ptr_t;

    /**
    \brief Erases type of an object and set gmap as its dispatching function.

    Thus we can use the resulting dynamic value for traversing its fields,
    like in static traversal.
    */
    template<typename T>
    inline Any toAny(T& v);

    template<typename T>
    inline Any toAny(T const& v);

    /**
    \brief Dispatching function for gmap
    */
    struct AnyRedir : Visitor<AnyRedir> {
      inline explicit AnyRedir(dyn_fun_t const& s):fun_(s){}
#ifdef NOT_INLINES
      template<typename T>
      inline void operator()(T& v) const  {
        notInline(*this,v);
      }

      template<typename T>
      inline void runInline(T& v) const {
        assert(fun_);
        fun_->action(toAny(v));
      }
#else
      template<typename T>
      inline void operator()(T& v) const {
        assert(fun_);
        fun_->action(toAny(v));
      }
#endif
    private:
      dyn_fun_t const& fun_;
    };
    
        template<typename U>
    struct AnyGmapAdapter : AnyFun<dyn_fun_t const, U*> {
      void action(dyn_fun_t const& s, U* const& v) const {
        assert(v);
        gmap(*v, AnyRedir(s), Unit());
      }
    };


    template<typename T>
    inline Any toAny(T& v) {
      return Any(&unAlias(v),
        createAnyFun<AnyGmapAdapter<typename UnAlias<T>::type> >(),
        (AbsDynInfo const&)dynInfo<T>());
    }

    template<typename T>
    inline typename boost::disable_if<boost::is_const<T>,T&>::type
      fromAny(Any const& v)
    {
      T* const* ret = v.get<T*>();
      assert(ret);
      return **ret;
    }

    template<typename T>
    inline typename boost::enable_if<boost::is_const<T>,T&>::type
      fromAny(Any const& v)
    {
      typedef typename boost::remove_const<T>::type t_t;
      t_t const* const* cret = v.get<t_t const*>();
      if (cret)
        return **cret;
      return fromAny<t_t>(v);
    }

    template<typename T>
    inline Any toAny(T const& v) {
      return Any(&unAlias(v),
        createAnyFun<AnyGmapAdapter<typename UnAlias<T>::type const> >(),
        (AbsDynInfo const&)dynInfo<T>()
        );
    }

    template<typename TagT, typename T, typename U>
    typename boost::enable_if<boost::is_same<
      T,typename boost::remove_const<U>::type
    > >::type clone(T const& in, U& out);

    template<typename T>
    void initAny(Any& a, T& v, AbsDynInfo const& d = dynInfo<T>()) {
      typedef typename UnAlias<T>::type u_t;
      if(!a.getInitializer() && !a.empty()) {
        T const& vv = fromAny<T const>(a);
        clone<HistoryTag>(vv, v);
        //v = fromAny<T const>(a);
        a.set(&unAlias(v),createAnyFun<AnyGmapAdapter<u_t> >(),d);
        return;
      }
      assert(a.getInitializer());
      a.set(&unAlias(v),createAnyFun<AnyGmapAdapter<u_t> >(),d);
      a.getInitializer()->run(a);
      a.getInitializer().reset();
    }

    /**
    \brief A helper for converting a set of statically typed functions
    to dynamic function.

    It uses runtime check for dispatching an argument. So if the list is
    large it's bad for runtime performance
    */
    template<typename T, typename EnableT=void>
    struct MkFnRedir : DynFun {
      typedef boost::function1<void, T&> fun_t;

      inline explicit MkFnRedir(
        fun_t const& f,
        dyn_fun_t const& t = dyn_fun_t()
        ):fun_(f),tail_(t){}

      void action(Any const& a) const {
        T* const* v = a.get<T*>();
        if (v)
          fun_(**v);
        else if (tail_)
          tail_->action(a);
      }
    private:
      fun_t fun_;
      dyn_fun_t tail_;
    };

    template<typename T>
    struct MkFnRedir<T,typename boost::enable_if<boost::is_const<T> >::type>{

      typedef boost::function<void(T& v)> fun_t;
      typedef typename boost::remove_const<T>::type clean_t;

      inline explicit MkFnRedir(
        fun_t const& f,
        dyn_fun_ptr_t const& t = dyn_fun_ptr_t()
        ):fun_(f),tail_(t.get()){}

      void action(Any const& a) const {
        T* const* v = a.get<T*>();
        if (v)
          fun_(boost::unwrap_ref(*v));
        else {
          clean_t const * const* v
            = &a.get<clean_t const *>();
         if (v)
          fun_(**v);
         else if (tail_)
          tail_(a);
        }
      }
    private:
      fun_t fun_;
      dyn_fun_t tail_;
    };

    /**
    \brief This will make a dynamic function from a C++ functor,
    the C++ functor will be applied only if resulting's function argument
    is convertible to C++ functor's argument

    \todo The type should be strictly equal to the provided function's type,
    this is not a problem for now since anyway we have flat hierarchies in syb
    but in future if we need a function to be run on all children of specified
    type we need to extend current implementation somehow.
    boost::any_cast doesn't support implicit C++ cast, so we need to use some
    hack. For example we can store a pointer to parent, and if any its child
    is requested do dynamic_cast after any_cast, etc.
    */
    template<typename T>
    dyn_fun_ptr_t dynfn(boost::function1<void, T&> const& f) {
      return boost::make_shared<MkFnRedir<T> >(f);
    }
    /**
    \brief Adds a type case for a dynamic function

    For example:
      dyn_fun_t f = dynfn(fn1) << fn2 << fn3;
    TODO: more description
    */
    template<typename T>
    dyn_fun_ptr_t operator<<(dyn_fun_ptr_t const& d,
      boost::function<void(T const& v)> const& f)
    {
      return MkFnRedir<T>(f,d);
    }
  }
}

#endif
