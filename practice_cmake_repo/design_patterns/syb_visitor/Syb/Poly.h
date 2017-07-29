/** 
\brief Generics implementation for polymorphic types.

\todo currently everything that has type boost::shared_ptr is considered 
make a special flag for marking polymorphic objects
\todo Add other instances of smart pointers, plain C++ pointers and references
\todo Only top parent is allowed as a reference now, change this!!!
*/
#ifndef SYB_POLY_H
#define SYB_POLY_H

#include <vector>
#include <map>
#include <list>

#include <iostream>

#include <loki/AssocVector.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/bind.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/transform.hpp>

#if 0
#include "Syb/HierarchyBuilderMisc.h"

#include "Syb/MultiConstr.h"
#include "Syb/Syb.h"
#else
#include "HierarchyBuilderMisc.h"

#include "MultiConstr.h"
#include "Syb.h"
#endif
/** 
\brief Helper macros.

\param name Name of the class
*/
#define ACCEPTABLE_ABSTRACT(name) public:                   \
virtual qx::syb::AbsAcceptor<name>                          \
  const& acceptor() const=0;                                \
  typedef name parent_t;                                    \
  typedef void acceptable_tag_t

/** 
\brief Helper macros.

\param name Name of the class
*/
#define ACCEPTABLE(name) public:                            \
virtual qx::syb::AbsAcceptor<name>                          \
  const& acceptor() const;                                  \
  typedef name parent_t;                                    \
  typedef void acceptable_tag_t

/**
\brief TRAVERSABLE macros modification for polymorphic types.

It should be defined in parent class.

\param name Name of the class
\param fields Field's list separated with "<<"
*/

#define TRAVERSABLE_ABSTRACT_BEGIN(name)   \
  ACCEPTABLE_ABSTRACT(name);               \
  TRAVERSABLE_BEGIN(name)

/**
\brief TRAVERSABLE macros modification for polymorphic types.

It should be defined in parent class.

\param name Name of the class
\param fields Field's list separated with "<<"
*/
#define TRAVERSABLE_PARENT_BEGIN(name)  \
  typedef name parent_t;                \
  ACCEPTABLE(name);                     \
  TRAVERSABLE_BEGIN(name)

/**
\brief The macros should be in class definition for every child of 
polymorphic class


\param name Name of the parent class
*/


#define TRAVERSABLE_SUB_CHILD_BEGIN(name,par,toppar)              \
public:                                                           \
  qx::syb::AbsAcceptor<toppar> const& acceptor() const;           \
  typedef void spine_tag_t;                                       \
  static std::string sybTypeName() { return #name; }              \
  static boost::shared_ptr<syb::AbsRegisterConstr>                \
    registerChild_;                                               \
  template<typename T, typename FunT>                             \
  inline static CT_ENABLE((boost::is_same<name                    \
      , typename boost::remove_const<T>::type>))                  \
        gmap(T& p, FunT const& f)                                 \
  {                                                               \
  par::gmap(static_cast<typename boost::mpl::if_<boost::is_const<T>, par const&,par&>::type>(p),f);

#define TRAVERSABLE_CHILD_BEGIN(name,par)                         \
            TRAVERSABLE_SUB_CHILD_BEGIN(name,par,par)

namespace qx {
  namespace syb {
  
    BOOST_MPL_HAS_XXX_TRAIT_DEF(acceptable_tag_t)

    /**
    \brief Type trait that tests whether the type has acceptors support
    */
    template<typename T>
    struct SupportVisitors:
#ifdef BOOST_MPL_CFG_NO_HAS_XXX
      has_acceptable_tag_t<T,mpl::true_>
#else
      has_acceptable_tag_t<T>
#endif
    {};

    /** 
    \brief Returns supported by the class generic functions

    \tparam T    Polymorphic type (parent) for which the template 
      is specialized
    \tparam EnableT Helper for \a boost::enable_if

    \todo Make a list of all known generic functions and return 
    it for every type
    */
    template<typename T, typename EnableT=void>
    struct VisitableBy;//:mpl::vector<>{};

    template<typename ParentT,typename NextT, typename FtrT>
    struct AbsAcceptorItem:NextT {
      virtual void accept(boost::shared_ptr<ParentT> const& p
        ,FtrT const& prod) const=0;
    };

    template<typename ParentT>
    struct AbsAcceptorHierarchy: mpl::fold 
    <
    typename VisitableBy<boost::shared_ptr<ParentT> >::type
      ,HierarchyItemTag
      ,mpl::bind3
      <
        mpl::quote3<AbsAcceptorItem>,
        ParentT,
        mpl::_1,
        mpl::_2
      >
    >
  {};

  template<typename ParentT>
  struct AbsAcceptor: 
    AbsAcceptorHierarchy<ParentT>::type 
  {
    template <typename FtrT>
    void callAccept(boost::shared_ptr<ParentT> const& p, FtrT const& prod) const
    {

      typedef typename GetSubHierarchyByType<
          typename AbsAcceptorHierarchy<ParentT>::type,
          VisitableBy<boost::shared_ptr<ParentT> >,
          FtrT
        >::type const * cast_type;

      static_cast<cast_type>(this)->accept(p, prod);
    }
    virtual ConstrInfo<boost::shared_ptr<ParentT> > const& constr() const = 0;
  };

  template<class ParentT>
  struct HasGeneratedAcceptors 
    :mpl::not_<boost::is_base_of<HierarchyItemTag,AbsAcceptor<ParentT> > > {};


  template<typename ParentT, typename ChildT>
  struct RegisterConstr;

  struct AbsRegisterConstr;

    /**
    \brief Special flag what showes the object satisfies Visitor's pattern
      requirements.
    */
    /**
    \brief Type trait that returns if the type is 
      need to be dispatched polymorphically
    
    \tparam T
    \tparam SpineT
    \tparam EnableT

    \todo Now works only for boost::shared_ptr do for other smart pointers
      and plain pointers
    */
    template<typename T, typename SpineT, typename EnableT=void>
    struct IsAccessibleBy:mpl::false_{};

    /**
    \brief Meta predicate that show is the generic function accessible from
      the polymorphic type
    */
    template<typename SpineT, typename T, typename EnableT=void>
    struct IsSpineSupportedBy
      :mpl::contains<
        typename VisitableBy<boost::shared_ptr<T> >::type
        ,SpineT
      >{};

    template<typename T, typename SpineT, typename EnableT>
    struct IsAccessibleBy<boost::shared_ptr<T>, SpineT, EnableT>
        :mpl::and_
        <
          SupportVisitors<T>,
          IsSpineSupportedBy<SpineT,typename boost::remove_cv<T>::type>
        >
    {};

    template<typename T, typename FunT>
    struct GmapRedir<boost::shared_ptr<T>,FunT,typename boost::enable_if<
          IsAccessibleBy<boost::shared_ptr<T>,FunT>
          >::type> 
    {
      inline static void action(boost::shared_ptr<T>& v, FunT const& s) {
      if (v) 
        v->acceptor().callAccept(v,s);
      }
    };

    template<typename T, typename FunT>
    struct GmapRedir<boost::shared_ptr<T> const,FunT,typename boost::enable_if<
          IsAccessibleBy<boost::shared_ptr<T>,FunT>
          >::type> 
    {
      inline static void action(boost::shared_ptr<T> const& v, FunT const& s) {
      if (v) 
        v->acceptor().callAccept(v,s);
      }
    };

    template<typename T, typename FunT>
    struct GmapRedir<boost::shared_ptr<T>,FunT,typename boost::disable_if<
          IsAccessibleBy<boost::shared_ptr<T>,FunT>
          >::type> 
    {
      inline static void action(boost::shared_ptr<T>& v, FunT const& s) {
      if (v) 
        gmap(*v,s,Unit());
      }
    };

    template<typename T, typename FunT>
    struct GmapRedir<boost::shared_ptr<T> const,FunT,typename boost::disable_if<
          IsAccessibleBy<boost::shared_ptr<T>,FunT>
          >::type> 
    {
      inline static void action(boost::shared_ptr<T> const& v, FunT const& s) {
      if (v) 
        gmap(*v,s,Unit());
      }
    };

    template<typename T, typename EnableT>
    struct TypeName<boost::shared_ptr<T>, EnableT> {
      inline static std::string value() {
        return TypeName<T>::value();
      }
    };

    /**
    \brief Returns list of function which empty value can be traversed 
      for a given type.

    Such kind of traversable is usually needed for object construction.
    But also can be used for metadata retrieving 

    \tparam T
    */
    template<typename T, typename EnableT=void>
    struct EmptyVisitableBy;//:mpl::vector<>{};

    template<typename ParentT,typename NextT, typename FtrT>
    struct PolyConstrItem:NextT {
      virtual void accept(FtrT const& prod) const=0;
    };

    template<typename ParentT>
    struct PolyConstrItemHierarchy: mpl::fold 
    <
    typename EmptyVisitableBy<boost::shared_ptr<ParentT> >::type
      ,HierarchyItemTag
      ,mpl::bind3
      <
        mpl::quote3<PolyConstrItem>,
        ParentT,
        mpl::_1,
        mpl::_2
      >
    >
  {};

  template<typename ParentT>
  struct PolyConstr;
  
  template<typename ParentT>
  struct PolyConstr<boost::shared_ptr<ParentT> >
    :PolyConstrItemHierarchy<ParentT>::type 
  {
      inline PolyConstr(){}
    /**
      \brief Dynamic redirect to \a TypeName meta-function
      */
      virtual std::string name() const=0;
      virtual std::string xmlName() const=0;
      /**
      \brief factory for an objects of given type

      \return Created object 
      */
      virtual boost::shared_ptr<ParentT> create() const=0;
  private:
      template<typename T>
      PolyConstr(const PolyConstr<T> &);
      
      template<typename T>
      PolyConstr& operator=(const PolyConstr<T> &);
  };

  template<typename ParentT>
  struct ConstrInfo<boost::shared_ptr<ParentT> 
    ,typename boost::enable_if<SupportVisitors<ParentT> >::type> 
    : ConstrInfoSpec<boost::shared_ptr<ParentT> >
  {

    inline explicit ConstrInfo(PolyConstr<boost::shared_ptr<ParentT> > const*c)
      :constr_(c){}

    template <typename FtrT>
    void accept(FtrT const& prod) const {
      typedef typename mpl::if_<HasGeneratedAcceptors<ParentT>,
        OverloadedPolyConstrAcceptor,
        BuildedHierarchyPolyConstrAcceptor>::type type;
      type::accept(constr_, prod);
    }

    std::string name() const { return constr_->name(); }
    std::string xmlName() const { return constr_->xmlName(); }    
    template<typename T>
    inline typename boost::enable_if<boost::is_same<T,ParentT> >::type 
      build(boost::shared_ptr<T>& v) const 
    {
      v = constr_->create();
    }
    template<typename T>
    inline typename boost::disable_if<boost::is_same<T,ParentT> >::type 
      build(boost::shared_ptr<T>& v) const 
    {
      v = boost::static_pointer_cast<T>(constr_->create());
    }
    inline bool operator==(
      ConstrInfo<boost::shared_ptr<ParentT> > const& r) const 
    {
      return constr_ == r.constr_;
    }

  private:
    struct BuildedHierarchyPolyConstrAcceptor {
      template <typename FtrT>
      static void accept(
          PolyConstr<boost::shared_ptr<ParentT> > const* constr_
          ,FtrT const& prod) {

        typedef typename GetSubHierarchyByType<
          typename PolyConstrItemHierarchy<ParentT>::type,
          EmptyVisitableBy<boost::shared_ptr<ParentT> >,
          FtrT>::type const * cast_type;

        static_cast<cast_type>(constr_)->accept(prod);
      }
    };

    struct OverloadedPolyConstrAcceptor {
      template <typename FtrT>
      static void accept (
        PolyConstr<boost::shared_ptr<ParentT> > const* constr_
        ,FtrT const& prod) {
        constr_->accept(prod);
      }
    };

    /*
    inline ConstrInfo(ConstrInfo const&) {}
    inline ConstrInfo& operator=(ConstrInfo const&) {}
    */

    PolyConstr<boost::shared_ptr<ParentT> > const* constr_;
  };

  /** 
  \brief Constructor's implementation for a concrete value
  */
  template<typename T>
  struct ConstrInfo<boost::shared_ptr<T>, typename boost::disable_if<SupportVisitors<T> >::type> 
    : ConstrInfoSpec<boost::shared_ptr<T> > 
  {
    inline ConstrInfo(){}
    inline static void build(boost::shared_ptr<T>& v) { v.reset(new T); }
    inline static std::string name() { return TypeName<T>::value(); }
    inline static std::string xmlName() { return XmlElementName<T, void, void>::value().name(); }
    inline bool operator==(ConstrInfo<boost::shared_ptr<T> > const&) const { 
      return true; 
    }
    template <typename FtrT>
    inline void accept(FtrT const& prod) const {
      prod.visit(Empty<boost::shared_ptr<T> >());
    }

  private:
  /*
    inline ConstrInfo(ConstrInfo const&) {}
    inline ConstrInfo& operator=(ConstrInfo const&) {}
  */
  };

    /**
    \brief Type desription.

    The main information in the description is type's constructors. The 
    constructors can be seen as child elements of polymorphic data types.
    
    \tparam T
    \tparam EnableT
    */
    template<typename T>
    struct PolyDataType;
    
    template<typename T>
    struct PolyDataType<boost::shared_ptr<T> >{
      /// \brief Construct's type
      typedef PolyConstr<boost::shared_ptr<T> > constr_item_t;
      typedef constr_item_t const* constr_t;
      
      ///  \brief list of constructors
      typedef std::list<constr_t> cont_t;

      ///  \brief Iterator for traversing constructors.
      typedef typename cont_t::const_iterator iterator_t;

      /// \brief Beginning of data type's constructor list.
      inline iterator_t begin() const {  return constrs_.begin(); }

      /// \brief End of data type's constructor list.
      inline iterator_t end() const {  return constrs_.end(); }

      /// \brief Returns number of constructors.
      inline size_t numOfConstrs() const { return constrs_.size(); }

      /**
      \brief Registers constructor

      \todo make accessible only for 
      */
      inline void reg(constr_item_t const& f) {
        constrs_.push_back(&f);
      }

      /**
      \brief Return constructor's container

      \todo Remove it
      */
      inline cont_t const& cont() const { return constrs_; }
    private:
      cont_t constrs_;
      // std::map<std::string,constr_t> byName_;
    };

    /** 
    \brief Returns the singleton object for a given data type desription

    \tparam T

    \todo Use a proper singleton (from Loki library)
    */
    template<typename T>
    PolyDataType<T>& polyDataType() {
      static PolyDataType<T> ret = PolyDataType<T>();
      return ret;
    }

    /** 
    \brief Evaluates provided C++ functor for each constructor.

    \tparam ParentT
    \tparam FunT

    \param f
    */
    template<typename ParentT, typename FunT>
    inline void forAllConstrs(FunT const& f) {
      std::for_each(polyDataType<ParentT>().begin()
        ,polyDataType<ParentT>().end()
        ,boost::bind(&PolyConstr<ParentT>::accept,_1,f)
        );
    }

    AbsConstrInfo const* findByName(
        Loki::AssocVector<std::string,AbsConstrInfo const*> const &constrMap, 
        std::string const& x);

    template<typename T>
    struct DataType<boost::shared_ptr<T>
        , typename boost::enable_if<SupportVisitors<T> >::type> 
        : DataTypeSpec<boost::shared_ptr<T> >
    {
      DataType();

      typedef ConstrInfo<boost::shared_ptr<T> > constr_t;

      inline size_t numOfConstrs() const { return poly_.numOfConstrs(); }

      inline AbsConstrInfo const& operator[](size_t x) const {
        assert(x < constrs_.size());
        return *constrs_[x];
      }

      inline constr_t const& fromValue(boost::shared_ptr<T> const& x) const {
        return x->acceptor().constr();
      }

      inline AbsConstrInfo const* byName(std::string const& x) const {
        return findByName(constrByName_,x);
      }

      inline AbsConstrInfo const* byXmlName(std::string const& x) const {
        return findByName(constrByXmlName_,x);
      }

      inline bool hasConstructor(boost::shared_ptr<T> const& x) const {
        return x;
      }

    private:
      PolyDataType<boost::shared_ptr<T> >& poly_;
      std::vector<boost::shared_ptr<AbsConstrInfo const> > constrs_;
      Loki::AssocVector<std::string,AbsConstrInfo const*> constrByName_;
      Loki::AssocVector<std::string,AbsConstrInfo const*> constrByXmlName_;
    };

    template<typename T,typename U,typename EnableT>
    struct IsMultiConstr<T,boost::shared_ptr<U>,EnableT>:SupportVisitors<U>{};

    template<typename FunT, typename T>
    void polyField(FunT const& f, boost::shared_ptr<T> const& v) {
      f(boost::static_pointer_cast<typename T::parent_t>(v));
    }

    template<typename FunT, typename T>
    void polyField(FunT const& f, boost::shared_ptr<T>& v) 
    {
      boost::shared_ptr<typename T::parent_t> tmp = v;
      f(tmp);
      v = boost::static_pointer_cast<T>(tmp);
    }

    template<typename T>
    struct DataType<boost::shared_ptr<T>, 
      typename boost::disable_if<SupportVisitors<T> >::type>
      :SingleConstr,DataTypeSpec<boost::shared_ptr<T> > 
    {

      DataType(){}

      typedef ConstrInfo<boost::shared_ptr<T> > constr_t;

      inline size_t numOfConstrs() const { return 1; }

      inline constr_t const& operator[](size_t x) const {
        assert(x==0);
        return constr_;
      }

      inline constr_t const& fromValue(boost::shared_ptr<T> const& x) const {
        assert(x);
        return constr_;
      }

      inline constr_t const* byName(std::string const& name) const {
        assert(name == TypeName<T>::value());
        return &constr_;
      }

      inline constr_t const* byXmlName(std::string const& name) const {
        assert(!strcmp(name.c_str(), XmlElementName<T, void, void>::value().name()));
        return &constr_;
      }

      inline bool hasConstructor(boost::shared_ptr<T> const& x) const {
        return x;
      }
    private:
      constr_t constr_;
    };

    //TODO: HACK!we need separate constructor for empty case
    template<typename T, typename U>
    struct WriteConstr<T, boost::shared_ptr<U>
      , typename boost::disable_if<SupportVisitors<U> >::type> 
    {
      template<typename RedirT>
      inline static void action(RedirT& x, T const& v) {
        if (unAlias(v)) {
          x.write(Empty<T>());
        }
      }
    };

    template<typename U>
    struct ReadConstr<boost::shared_ptr<U>
        , typename boost::disable_if<SupportVisitors<U> >::type
    > 
    {
      template<typename RedirT>
      inline static boost::optional<ConstrInfo<boost::shared_ptr<U> > > 
        action(RedirT& x) 
      {
        if (x.read(Empty<boost::shared_ptr<U> >()))
          return dataType<boost::shared_ptr<U> >()[0];
        return boost::none;
      }
    };

    template<typename T, typename U>
    inline T& downCast(boost::shared_ptr<U> const& v, 
      Empty<boost::shared_ptr<T> > const&)  
    {
      return static_cast<T&>(*v);
    }
  }
}

#endif
