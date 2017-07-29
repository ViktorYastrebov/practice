/**
\brief The file for inclusion in cpp files where polymorphic type support
code is instantied.

Include this header to the any CPP file. And put there list of child/parent
pairs by means of \a ACCEPTABLE_INSTANCE macros call. It doesn't have to be
a single file. The macros can be called from any cpp file for example from
file where other method of a child is declared.

*/
#ifndef SYB_POLY_SYB_H
#define SYB_POLY_SYB_H

#if 0
#include "Syb/Poly.h"
#include "Syb/DynRepr.h"
#include "Syb/cJSON.h"
#else
#include "Poly.h"
#include "DynRepr.h"
#include "cJSON.h"
#endif

/** 
\brief Instanties templates for a given parent and child.

\param parent  Name of a parent class.
\param child  Name of a child class.
*/
//   RegisterConstr<parent,child> sybRegisterConstr ## parent ## child ;

#define ACCEPTABLE_INSTANCE(parent,child)                                 \
  boost::shared_ptr<qx::syb::AbsRegisterConstr> child::registerChild_     \
    = boost::make_shared<qx::syb::RegisterConstr<parent,child> >();       \
  qx::syb::AbsAcceptor<parent> const& child::acceptor() const {           \
    return qx::syb::Acceptor<child,parent>::instance();                   \
  }

//#define ACCEPTABLE_PARENT_INSTANCE(name) ACCEPTABLE_INSTANCE(name,name)
#define ACCEPTABLE_PARENT_INSTANCE(name)                                  \
  AbsAcceptor<name> const& name::acceptor() const {                       \
    return Acceptor<name,name>::instance();                               \
  }

namespace qx {
  namespace syb {

    template<typename ChildT,typename ParentT,typename NextT, typename FtrT>
    struct ConstrImplItem:NextT {
      virtual void accept(FtrT const& ftr) const {
        ftr.visit(Empty<boost::shared_ptr<ChildT> >());
      }
    };

    template<typename ChildT, typename ParentT>
    struct ConstrImplHierarchy: mpl::fold
    <
    typename EmptyVisitableBy<boost::shared_ptr<ParentT> >::type,
      PolyConstr<boost::shared_ptr<ParentT> >,
      mpl::bind4
      <
        mpl::quote4<ConstrImplItem>,
        ChildT,
        ParentT,
        mpl::_1,
        mpl::_2
      >
    >
    {};

    /** 
      \brief Concrete type for constructor's description.

      \tparam ChildT
      \tparam ParentT
      \tparam EnableT

      \todo Create some customizable temple for object creation
      this way we'll support types without argument less constructors

    */
    template<typename ChildT, typename ParentT,typename EnbaleT=void>
    struct ConstrImpl:ConstrImplHierarchy<ChildT, ParentT>::type {

      virtual std::string name() const {
        return TypeName<ChildT>::value();
      }
      
      virtual std::string xmlName() const {
        return TypeName<ChildT>::value();
      }

      boost::shared_ptr<ParentT> create() const {
        // hope ParentT has virtual destructor
        return boost::make_shared<ChildT>();
      }

      static const ConstrImpl<ChildT, ParentT>& instance() {
        //TODO: proper singleton
        static ConstrImpl<ChildT, ParentT> ret = ConstrImpl<ChildT, ParentT>();
        PolyConstr<boost::shared_ptr<ParentT> > *p = &ret;
        p->name();
        return ret;
        // return *(new ConstrImpl<ChildT, ParentT>());
      }
    private:
      inline ConstrImpl(){}
    };

    template<typename T,typename ParentT,typename NextT, typename FtrT>
    struct AcceptorItem:NextT {
      void accept(boost::shared_ptr<ParentT> const& p,FtrT const& ftr) const {
        ftr.polyVisit(*boost::static_pointer_cast<T>(p));
      }
    };

    template<typename T, typename ParentT>
    struct AcceptorHierarchy: mpl::fold
    <
    typename VisitableBy<boost::shared_ptr<ParentT> >::type
      ,AbsAcceptor<ParentT>
      ,mpl::bind4
      <
        mpl::quote4<AcceptorItem>,
        T,
        ParentT,
        mpl::_1,
        mpl::_2
      >
    >
  {
  };

    // if this doesn't work on some compiler redo it using old good method (i.e. manual set fold)
    template<typename T, typename ParentT>
    struct Acceptor: AcceptorHierarchy<T,ParentT>::type {
      static inline Acceptor const& instance() {
        static Acceptor ret = Acceptor();  //TODO: propper singleton
        return ret;
      }
      ConstrInfo<boost::shared_ptr<ParentT> > const& constr() const {
        return constr_;
      }
    private:
      inline Acceptor():constr_(&ConstrImpl<T,ParentT>::instance()){}
      ConstrInfo<boost::shared_ptr<ParentT> > constr_;
    };

    struct AbsRegisterConstr {
      ~AbsRegisterConstr(){};
    };

    /**
    \brief Helper clas for registering each constructor in data type description

    \tparam ParentT
    \tparam ChildT

    \todo unregister probably will be needed for some ad-hoc registies
    */
    template<typename ParentT, typename ChildT>
    struct RegisterConstr : AbsRegisterConstr {
      RegisterConstr(){
        polyDataType<boost::shared_ptr<ParentT> >()
          .reg(ConstrImpl<ChildT,ParentT>::instance());
      }
    };

    template<typename T>
    DataType<boost::shared_ptr<T>, typename boost::enable_if<SupportVisitors<T> >::type>
     ::DataType():poly_(polyDataType<boost::shared_ptr<T> >()) {
        typedef typename PolyDataType<boost::shared_ptr<T> >::iterator_t i_t;
        constrs_.reserve(poly_.numOfConstrs());
        for(i_t i = poly_.begin(); i != poly_.end(); ++i) {
          constrs_.push_back(boost::make_shared<constr_t>(*i));
          typename PolyDataType<boost::shared_ptr<T> >::constr_t yo = *i;
          std::string tmp((*i)->name());
          boost::to_lower(tmp);
          constrByName_[tmp] = constrs_.back().get();
          std::string xmlTmp((*i)->xmlName());
          boost::to_lower(xmlTmp);
          constrByXmlName_.insert(
            std::make_pair(xmlTmp,constrs_.back().get()));
        }
      }

    template<typename T, typename EnT=void>
    struct OptRedir {
      /*static void fromBinary(Any &v, const std::string &binary) {
        std::stringstream ss(binary, std::stringstream::binary | std::stringstream::in);
        qx::msg::read(ss, fromAny<T>(v));
      }
      static void toBinary(const Any &v, std::string &binary) {
        std::stringstream ss(std::stringstream::binary | std::stringstream::out);
        qx::msg::write(ss, fromAny<T const>(v));
        binary = ss.str();
      }*/
      static cJSON *toJson(const Any &v) {
        return qx::json::write(fromAny<T const>(v), true);
      }
    };

    template<typename T>
    struct OptRedir<boost::optional<T>, typename boost::enable_if<boost::is_fundamental<T> >::type> {
      /*static void fromBinary(Any &vv, const std::string &binary)  {
        std::stringstream ss(binary, std::stringstream::binary | std::stringstream::in);
        boost::optional<T>& v = fromAny<boost::optional<T> >(vv);
        v = boost::none;
        bool hasValue;
        qx::msg::read(ss, hasValue);
        if (hasValue) {
          T result;
          qx::msg::read(ss, result);
          v = result;
        }
      }
      static void toBinary(const Any &vv, std::string &binary) {
        std::stringstream ss(std::stringstream::binary | std::stringstream::out);
        boost::optional<T> const& v = fromAny<const boost::optional<T> >(vv);
        bool hasValue = v;
        qx::msg::write(ss, hasValue);
        if (hasValue)
          qx::msg::write(ss, *v);
        binary = ss.str();
      }*/
      static cJSON *toJson(const Any &vv) {
        cJSON *node = 0;
        boost::optional<T> const& v = fromAny<const boost::optional<T> >(vv);
        if (v) {
          node = qx::json::write(*v, true);
        } else {
          node = cJSON_CreateObject();
          cJSON_AddItemToObject(node, "_nil", cJSON_CreateTrue());
        }
        return node;
      }
    };

    template<typename T>
    struct OptRedir<boost::optional<T>, typename boost::disable_if<boost::is_fundamental<T> >::type> {
      /*static void fromBinary(Any &vv, const std::string &binary)  {
        std::stringstream ss(binary, std::stringstream::binary | std::stringstream::in);
        boost::optional<T>& v = fromAny<boost::optional<T> >(vv);
        v = boost::none;
        T res;
        qx::msg::read(ss, res);
        v = res;
      }
      static void toBinary(const Any &vv, std::string &binary) {
        std::stringstream ss(std::stringstream::binary | std::stringstream::out);
        boost::optional<T> const& v = fromAny<const boost::optional<T> >(vv);
        if (v) {
          qx::msg::write(ss, *v);
        } else {
          T empty;
          qx::msg::write(ss, empty);
        }
        binary = ss.str();
      }*/
      static cJSON *toJson(const Any &vv) {
        cJSON *node = 0;
        boost::optional<T> const& v = fromAny<const boost::optional<T> >(vv);
        if (v) {
          node = qx::json::write(*v, true);
        } else {
          node = cJSON_CreateObject();
          cJSON_AddItemToObject(node, "_nil", cJSON_CreateTrue());
        }
        return node;
      }
    };

    /*
      template<typename T>
      void DataTypeSpec<T>::fromBinary(Any &v, const std::string &binary) const {
        OptRedir<T>::fromBinary(v,binary);
      }

      template<typename T>
      void DataTypeSpec<T>::toBinary(const Any &v, std::string &binary) const {
        OptRedir<T>::toBinary(v,binary);
      }
      */

      template<typename T>
      cJSON *DataTypeSpec<T>::toJson(const Any &v) const {
        return OptRedir<T>::toJson(v);
      }
  }
}

#endif
