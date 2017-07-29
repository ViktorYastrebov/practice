/**
\brief Definitions for MultiConstructors value.

Multiconstructor values includes:
  polymorphic types, boost::variant
  ,boost::optional, ConsListView

\todo Constructors iterator
*/
#ifndef SYB_MULTI_CONSTR_H
#define SYB_MULTI_CONSTR_H

#include <iosfwd>
#include <sstream>

#if 0
#include "Syb/Syb.h"
#include "Syb/TypeAlias.h"
#include "Syb/Any.h"
#include "Syb/BinaryMsgStub.h"
#else
#include "Syb.h"
#include "TypeAlias.h"
#include "Any.h"
#include "BinaryMsgStub.h"
#endif

struct cJSON;

namespace qx {
  namespace syb {

    template<typename T, typename TagT, typename EnableT>
    struct XmlElementName;
    template<typename T, typename TagT, typename EnableT>
    struct XmlName;

    struct AbsConstrInfo {
      virtual void dynBuild(Any const&) const = 0;
      virtual std::string getName() const = 0;
      virtual std::string getXmlName() const = 0;
      virtual ~AbsConstrInfo();
    };

    template<typename T, typename EnT = void>
    struct ConstrInfo;

    template<typename T>
    struct ConstrInfoSpec : AbsConstrInfo {
      inline ConstrInfo<T> const& This() const { 
        return static_cast<ConstrInfo<T> const&>(*this); 
      }

      void dynBuild(Any const& v) const {
        return This().build(fromAny<T>(v));
      }
      std::string getName() const { 
        return This().name(); 
      }
      std::string getXmlName() const {
        return This().xmlName();
      }
    };

    /** 
    \brief Constructor's implementation for a concrete value
    */
    template<typename T, typename EnT>
    struct ConstrInfo : ConstrInfoSpec<T> {
      inline ConstrInfo(){}
      inline static void build(T&) {}
      inline static std::string name() { return TypeName<T>::value(); }
      inline static std::string xmlName() { return XmlElementName<T,void,void>::value().name(); }
      inline bool operator==(ConstrInfo<T> const&) const { return true; }
      template <typename FtrT>
      inline void accept(FtrT const& prod) const {
        prod.visit(Empty<T>());
      }

    /*
    private:
      inline ConstrInfo(ConstrInfo const&) {}
      inline ConstrInfo& operator=(ConstrInfo const&) {}
    */
    };

    /// \brief Specialize default value for an object of specified type
    template<class T>
    struct DefualtValAssigner {
      static void assignDefVal(T& v) {
        v = T();
      }
    };



    /// \brief Just a marker for data types this single constructor
    struct SingleConstr{};

    /** \brief generic data type description
    
    Here all type information is erased thus only dynamic versions of functions
    are available. If at the moment of invocation static type is know it's 
    possible to downcast to static version of metadata with static_cast 
    (boost::polymorphic downcast will be better here).
    */
    struct AbsDataType {
      /// \brief Returns number of constructors (dynamic version).
      virtual size_t dynNumOfConstrs() const = 0;
      /// \brief Access constructor's information by index (dynamic version).
      virtual AbsConstrInfo const& constrByIndex(size_t x) const  = 0;
      /// \brief Returns constructor's information for the provided value 
      /// (dynamic version)
      virtual AbsConstrInfo const& dynFromValue(Any const& x) const = 0;
      /// \brief Checks if fromValue will return constructor's information 
      /// (dynamic version).
      virtual bool dynHasConstructor(Any const& x) const = 0;

      /// \brief Inits an object held within Any with default value
      /// It does not create an object, it just inits it.
      virtual void setDefault(Any const& x) const = 0;

      virtual bool hasStringValue() const = 0;
      virtual bool fromString(Any&, UnicodeString const&) const = 0;
      virtual void toString(Any const&, UnicodeString&) const = 0;

      //virtual void fromBinary(Any &v, const std::string &binary) const = 0;
      //virtual void toBinary(const Any &v, std::string &binary) const = 0;

      virtual cJSON *toJson(const Any &v) const = 0;

      virtual ~AbsDataType();
      /**
      \brief Access constructor's information by name (dynamic version)

      \param name Name of an constructor. Should be equal to \a TypeName
      return value.

      \return If no constructors with such name NULL or 
      constructor's description otherwise

      */
      virtual AbsConstrInfo const* dynByName(std::string const& n) const = 0;
      virtual AbsConstrInfo const* dynByXmlName(std::string const& n) const = 0;
    };

    /// \brief Static to dynamic type info bridge
    template<typename T>
    struct DataTypeSpec:AbsDataType {
      inline DataType<T> const& This() const { 
        return static_cast<DataType<T> const&>(*this); 
      }
      size_t dynNumOfConstrs() const {
        return This().numOfConstrs();
      }
      AbsConstrInfo const& constrByIndex(size_t x) const {
        return This()[x];
      }
      AbsConstrInfo const& dynFromValue(Any const& x) const {
        return This().fromValue(fromAny<T const>(x));
      }
      bool dynHasConstructor(Any const& x) const {
        return This().hasConstructor(fromAny<T const>(x));
      }

      AbsConstrInfo const* dynByName(std::string const& name) const {
        // TODO: probably optional already has some kind of map?
        return This().byName(name);
      }
      AbsConstrInfo const* dynByXmlName(std::string const& name) const {
        return This().byXmlName(name);
      }
      void setDefault(Any const& x) const {
        DefualtValAssigner<T>::assignDefVal(fromAny<T>(x));
      }
      bool hasStringValue() const { 
        return StringRepr<T>::value;
      }
      bool fromString(Any& v, UnicodeString const& s) const {
        return StringRepr<T>::fromString(fromAny<T>(v),s);
      }
      void toString(Any const& v, UnicodeString& s) const {
        StringRepr<T>::toString(fromAny<T>(v),s);
      }

      //void fromBinary(Any &v, const std::string &binary) const;
      //void toBinary(const Any &v, std::string &binary) const;
      cJSON *toJson(const Any &v) const;
    };

    template<typename T,typename EnT>
    struct DataType:SingleConstr, DataTypeSpec<T> {

      /// \brief Returns number of constructors.
      inline size_t numOfConstrs() const { return 1; }

      /// \brief Access constructor's information by index.
      inline ConstrInfo<T> const& operator[](size_t x) const {
        assert(x == 0);
        return constr_;
      }
      
      /// \brief Returns constructor's inforation for the provided value
      inline ConstrInfo<T> const& fromValue(T const& x) const {
        return constr_;
      }

      /// \brief Checks if fromValue will return constructor's information
      inline bool hasConstructor(T const& x) const {
        return true;
      }

      /**
      \brief Access constructor's information by name.

      \param name Name of an constructor. Should be equal to \a TypeName
      return value.

      \return If no constructors with such name NULL or 
      constructor's description otherwise

      */
      inline ConstrInfo<T> const* byName(std::string const& name) const {
        return /*r == */ name/*()*/==constr_.name()
            ?&constr_
            :NULL;
      }

      inline ConstrInfo<T> const* byXmlName(std::string const& name) const {
        return name == constr_.xmlName()? &constr_ : NULL;
      }
    private:
      ConstrInfo<T> constr_;

      inline DataType& operator=(DataType const&) {}
      inline DataType(DataType const&) {}
      inline DataType() {}
      template<typename U>
      friend DataType<U> const& dataType(); 
   };

    /** 
    \brief Returns the singleton object for a given data type description

    \tparam T

    \todo Use a proper singleton (from Loki library)
    */
    template<typename T>
    DataType<T> const& dataType() {
      static DataType<T> ret = DataType<T>();
      return ret;
    }

    /**
    \brief Meta predicate for multi-constructor functions.
    */
    template<
      typename T,
      typename U=typename UnAlias<T>::type,
      typename EnT=void
    >
    struct IsMultiConstr:mpl::false_{}; 
    //mpl::not_<boost::is_base_of<SingleConstr,DataType<T> > >{};

    /** \defgroup multiconstropts "optionals implementation" */
    // @{

    template<typename T, typename EnableT>
    struct ConstrInfo<boost::optional<T>, EnableT> 
      : ConstrInfoSpec<boost::optional<T> > 
    {
      inline void build(boost::optional<T>& v) const {
        v.reset(T());
      }
      inline std::string name() const { return TypeName<T>::value(); }
      inline std::string xmlName() const { return XmlElementName<T, void, void>::value().name(); }

      template<typename FtrT>
      inline void accept(FtrT const& f) const {
        f.visit(Empty<T>());
      }
      inline bool operator==(ConstrInfo const& v) const {
        return true;
      }
    private:
    /*
      inline ConstrInfo(ConstrInfo const&) {}
      inline ConstrInfo& operator=(ConstrInfo const&) {}
   */
    };

    template<typename T>
    struct TypeName<boost::optional<T> >:TypeName<T>{};

    template<typename T, typename EnableT>
    struct DataType<boost::optional<T>, EnableT> 
      : DataTypeSpec<boost::optional<T> >
    {
      typedef ConstrInfo<boost::optional<T> > constr_t;

      inline size_t numOfConstrs() const { return 1; }
      inline constr_t const& operator[](size_t x) const {
        assert(x == 0);
        return val_;
      }
      inline constr_t const* byName(std::string const& name) const {
        if (name == TypeName<T>::value())
          return &val_;
        return NULL;
      }
      inline constr_t const* byXmlName(std::string const& name) const {
        if (name == XmlElementName<T, void, void>::value().name())
          return &val_;
        return NULL;
      }
      inline bool hasConstructor(boost::optional<T> const& x) const { 
        return x;
      }
      inline constr_t const& fromValue(boost::optional<T> const& x) const {
        assert(x);
        return val_;
      }
    private:
      constr_t val_;
    };

    template<typename T,typename U,typename EnableT>
    struct IsMultiConstr<T,boost::optional<U>,EnableT>:mpl::true_{};

    template<typename RedirT, typename T, typename EnableT = void>
    struct DispatchMultiConstr {
      template<typename CtxT>
      inline static void action(CtxT const& w, T const& d) {
        RedirT::singleConstr(w,d);
      }
    };

    template<typename RedirT,typename T>
    struct DispatchMultiConstr<RedirT,T
      ,typename boost::enable_if<IsMultiConstr<T> >::type> 
    {
      template<typename CtxT>
      inline static void action(CtxT const& w, T const& d) {
        RedirT::multiConstr(w,d);
      }
    };

    //TODO: generic version
    template<typename T,typename U=typename UnAlias<T>::type,typename EnT=void>
    struct WriteConstr {
      template<typename CtxT>
      inline static void action(CtxT& x, T const&) {
        x.write(Empty<T>());
      }
    };

    template<typename RedirT>
    struct WriteConstrChild {
      inline explicit WriteConstrChild(RedirT& x):ctx_(x){}
      template<typename ChT>
      inline void visit(Empty<ChT> const& v) const {
        ctx_.write(v);
      }
    private:
      RedirT& ctx_;
    };

    template<typename T, typename U>
    struct WriteConstr<T,U
      , typename boost::enable_if<IsMultiConstr<U> >::type
    > {

      template<typename RedirT>
      inline static void action(RedirT& x, T const& v) {
        // TODO: we need separate constructor for empty case?
        DataType<U> dt = dataType<U>();
        if (dt.hasConstructor(unAlias(v))) {
          dt.fromValue(unAlias(v)).accept(WriteConstrChild<RedirT>(x));
        }
      }
    };

    // not using boost::bind for compile time optimisation (todo: check it works)
    namespace priv {

      template<typename T>
      struct BindToBuild { 
        inline explicit BindToBuild(ConstrInfo<T> const& c):constr_(c){}
        inline void operator()(T& v) {
          constr_.build(v);
        }
      private:
        ConstrInfo<T> constr_;
      };
    }

    // TODO: generic version
    template<typename U, typename EnT = void>
    struct ReadConstr {
      template<typename RedirT>
      inline static boost::optional<ConstrInfo<U> > action(RedirT& x) 
      {
        x.read(Empty<U>());
        return dataType<U>()[0];
      }
    };


    template<typename RedirT>
    struct ReadConstrChild {
      inline ReadConstrChild(RedirT& x, bool& r)
        :ctx_(x),res_(r){}
      template<typename ChT>
      inline void visit(Empty<ChT> const& v) const {
        res_ = ctx_.read(v);
      }
    private:
      RedirT& ctx_;
      bool& res_;
    };

    template<typename U>
    struct ReadConstr<U, typename boost::enable_if<
          IsMultiConstr<U>
        >::type
    > 
    {
      template<typename RedirT>
      inline static boost::optional<ConstrInfo<U> > action(RedirT& x) 
      {
        bool ret = false;
        /*
        if (advice) {
          advice->accept(ReadConstrChild<RedirT>(x,ret));
          if (ret) return advice;
        } 
        */
        DataType<U> dt = dataType<U>();
        for (size_t i = 0;i < dt.numOfConstrs();i++) {
          dt[i].accept(ReadConstrChild<RedirT>(x,ret));
          if (ret) return dt[i];
        }
        return boost::none;
      }
    };

    template<typename T>
    inline T& downCast(boost::optional<T>& v, Empty<T> const&) {
      return *v;
    }

    template<typename T>
    inline T const& downCast(boost::optional<T> const& v, Empty<T> const&) {
      return *v;
    }

    template<typename T, typename EnableT=void>
    struct CastToTopParent:Ret<T> {
      inline static T& cast(T& v) { return v; }
      inline static T const& cast(T const& v) { return v; }
    };

    /**
    \brief Signals the type has null value 
    (defined by constructor without arguments)
    
    This is needed for validation for example in SYB XML.
    */

    template<typename T, typename EnableT=void>
    struct Nullable:mpl::true_{};

    template<typename T, typename FunT>
    struct GmapRedir<boost::optional<T>,FunT> {
      inline static void action(boost::optional<T>& p, FunT const& f) {
        if (p) f.polyVisit(*p);
      }
    };

    template<typename T, typename FunT>
    struct GmapRedir<boost::optional<T> const,FunT> {
      inline static void action(boost::optional<T> const& p, FunT const& f) {
        if (p) f.polyVisit(*p);
      }
    };

    // @}

    
    /**
    \brief Optional as a multiconstr is a mistake, and this is 
    a temporary solution.

    Use it to filter out optional from multiconstr specialization.
    */
    template<typename T>
    struct NotOpt:mpl::true_{};

    template<typename T>
    struct NotOpt<boost::optional<T> >:mpl::false_{};

  }
}

#endif
