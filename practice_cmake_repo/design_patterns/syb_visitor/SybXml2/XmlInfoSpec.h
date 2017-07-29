#ifndef SYB_XML2_XML_INFO_SPEC_H
#define SYB_XML2_XML_INFO_SPEC_H

/**
\brief Implementation of abstract XML meta-data information classes
*/
#if 0
#include "SybXml2/XmlRef.h"
#include "SybXml2/XmlTypeDateTime.h"
#include "SybXml2/XmlTypeUnicodeString.h"
#else
#include "XmlRef.h"
#include "XmlTypeDateTime.h"
#include "XmlTypeUnicodeString.h"
#endif

namespace qx {
  namespace syb {

    struct TypeIdByElementName : TypeIdPolicy<true> {
      ElementId elementId(QName const& baseName, QName const& identity) const {
        return ElementId(identity);
      }
      std::string getTypeId(XmlElementTypeReader& r) const {
        const QName &qname = r.getElementName();
        return std::string(qname.name(), qname.nameLength());
      }
      inline static TypeIdPolicy<true> const& instance() {
        static TypeIdByElementName ret = TypeIdByElementName();
        return ret;
      }
    };

    struct TypeIdByAttribute : TypeIdPolicy<true> {
      inline explicit TypeIdByAttribute(QName const& attrName)
        :attrName_(attrName){}
      ElementId elementId(
        QName const& baseName, QName const& identity) const
      {
        return ElementId(baseName,attrName_,identity.name());
      }
      boost::optional<ElementId> fixedPattern(QName const& baseName) const {
        return ElementId(baseName);
      }
      std::string getTypeId(XmlElementTypeReader& r) const {
        return r.getNextElAttribute(attrName_);
      }
      /*
      inline static TypeIdPolicy<true> const& instance() {
        static TypeIdByAttribute ret = TypeIdByAttribute(QName("type"));
        return ret;
      }
      */
    private:
      QName attrName_;
    };

    template<typename AttrNameT>
    struct TypedIdByStaticAttr:TypeIdByAttribute {
      inline TypedIdByStaticAttr():TypeIdByAttribute(AttrNameT::value()){};
      inline static TypeIdPolicy<true> const& instance() {
        static TypedIdByStaticAttr ret = TypedIdByStaticAttr();
        return ret;
      }
    };

    template<>
    struct AnyElementTypeId<true>:TypeIdPolicy<true> {
      ElementId elementId(
        QName const& baseName, QName const& identity) const
      {
        return ElementId("", "type", identity.name());
      }
      boost::optional<ElementId> fixedPattern(QName const& baseName) const {
        return boost::none;
      }
      inline static AnyElementTypeId<true> const& instance() {
        static AnyElementTypeId<true> ret = AnyElementTypeId<true>();
        return ret;
      }
      std::string getTypeId(XmlElementTypeReader& r) const {
        return r.getAttribute(QName("type"));
      }

    };

    template<>
    struct AnyElementTypeId<false>:TypeIdPolicy<false> {
      ElementId elementId(QName const& baseName) const {
        return ElementId();
      }
      inline static AnyElementTypeId<false> const& instance() {
        static AnyElementTypeId<false> ret = AnyElementTypeId<false>();
        return ret;
      }
    };

    template<
       typename T,
       typename TagT,
       typename U = typename UnAlias<T>::type,
       bool isMultiConstrT = IsMultiConstr<U>::value,
       bool HasAttrT
        = boost::is_base_of<NotDefined,typename NodeAttribute<T,TagT>::type >::value
    > struct GetTypeIdPolicy;

    template<typename T, typename TagT, typename U>
    struct GetTypeIdPolicy<T,TagT,U,true,true>:Ret<TypeIdByElementName>{};

    template<typename T, typename TagT, typename U>
    struct GetTypeIdPolicy<T,TagT,U,true,false>
      :Ret<TypedIdByStaticAttr<typename NodeAttribute<T,TagT>::type> >{};

    template<typename T, typename TagT, typename U, bool hasAttrT>
    struct GetTypeIdPolicy<T,TagT,U,false,hasAttrT>:Ret<TypeIdPolicy<false> >{};

   template<typename TagT, typename U, bool isMultiConstrT>
   struct DefaultXmlInfo:XmlInfo<isMultiConstrT> {
      inline DefaultXmlInfo(
        QName const& name,
        ElementId const& nullEl,
        bool hasIndOrder,
        TypeIdPolicy<isMultiConstrT> const& tp
        )
        :XmlInfo<isMultiConstrT>(
          name,nullEl,hasIndOrder,tp
          )
        ,unfoldedRead_(priv::XmlElReadFormat<TagT,mpl::true_
          ,typename IsMultiConstr<U>::type>::instance())
        ,unfoldedWrite_(priv::XmlElWriteFormat<TagT,mpl::true_>::instance())
      {}

      bool readUnfolded(XmlReader<void> const& r, Any const& v) const {
        return unfoldedRead_.read(r,v);
      }

      bool writeUnfolded(XmlWriter<void> const& w, Any const& v) const {
        return unfoldedWrite_.write(w,v);
      }
    private:
      priv::XmlElementReader<TagT,U> unfoldedRead_;
      priv::XmlElementWriter<TagT,U> unfoldedWrite_;
    };

   template<
     typename TagT,
     typename U,
     typename SkipT,
     bool isMultiConstrT,
     typename KindT
   >
   struct ConcrXmlInfo;

   template<typename TagT, typename U, typename SkipT, bool isMultiConstrT>
   struct ConcrXmlInfo<TagT,U, SkipT,isMultiConstrT,XmlAttributeTag>
     :XmlInfo<isMultiConstrT>
   {
      inline ConcrXmlInfo(
        QName const& name,
        ElementId const& nullEl,
        bool hasIndOrder,
        TypeIdPolicy<isMultiConstrT> const& tp,
        XmlOptions const& opts
        )
        :XmlInfo<isMultiConstrT>(name,nullEl,hasIndOrder,tp),opts_(opts)
      {}

      size_t entityIndex() const {
        return EntId<U>::value;
      }

      bool read(XmlReader<void> const& r, Any const& v) const {
        opts_.action(r.impl());
        readDisp_.read(r,v);
        return true;
      }

      bool write(XmlWriter<void> const& w, Any const& v) const {
        writeDisp_.write(w,v);
        return true;
      }

      bool readUnfolded(XmlReader<void> const& r, Any const& v) const {
        opts_.action(r.impl());
        unfoldedReadDisp_.read(r,v);
        return true;
      }

      bool writeUnfolded(XmlWriter<void> const& r, Any const& v) const {
        unfoldedWriteDisp_.write(r,v);
        return true;
      }

    private:
      priv::XmlAttributeReader<TagT,U> readDisp_;
      priv::XmlAttributeWriter<TagT,U> writeDisp_;
      priv::XmlReadContent<TagT,U> unfoldedReadDisp_;
      priv::XmlWriteContent<TagT,U> unfoldedWriteDisp_;
      XmlOptions const& opts_;
    };

   template<typename TagT, typename U, typename SkipT, bool isMultiConstrT>
   struct ConcrXmlInfo<TagT,U,SkipT,isMultiConstrT,XmlTextTag>
     :XmlInfo<isMultiConstrT>
   {
      inline ConcrXmlInfo(
        QName const& name,
        ElementId const& nullEl,
        bool hasIndOrder,
        TypeIdPolicy<isMultiConstrT> const& tp,
        XmlOptions const& opts
        )
        :XmlInfo<isMultiConstrT>(name,nullEl,hasIndOrder,tp)

      {}

      size_t entityIndex() const {
        return EntId<U>::value;
      }

      bool read(XmlReader<void> const& r, Any const& v) const {
        readDisp_.read(r,v);
        return true;
      }

      bool write(XmlWriter<void> const& w, Any const& v) const {
        writeDisp_.write(w,v);
        return true;
      }

      bool readUnfolded(XmlReader<void> const& r, Any const& v) const {
        return read(r,v);
      }

      bool writeUnfolded(XmlWriter<void> const& r, Any const& v) const {
        return write(r,v);
      }

    private:
      priv::XmlReadContent<TagT,U> readDisp_;
      priv::XmlWriteContent<TagT,U> writeDisp_;
    };

    template<typename TagT, typename U, typename SkipT, bool isMultiConstrT>
    struct ConcrXmlInfo<TagT,U,SkipT,isMultiConstrT,XmlElementTag>:XmlInfo<isMultiConstrT> {
      inline ConcrXmlInfo(
        QName const& name,
        ElementId const& nullEl,
        bool hasIndOrder,
        TypeIdPolicy<isMultiConstrT> const& tp,
        XmlOptions const& opts
        )
        :XmlInfo<isMultiConstrT>(name,nullEl,hasIndOrder,tp)
        ,unfoldedRead_(priv::XmlElReadFormat<TagT,mpl::true_,mpl::bool_<isMultiConstrT> >::instance())
        ,unfoldedWrite_(priv::XmlElWriteFormat<TagT,mpl::true_>::instance())
        ,readDisp_(priv::XmlElReadFormat<TagT,SkipT,mpl::bool_<isMultiConstrT> >::instance())
        ,writeDisp_(priv::XmlElWriteFormat<TagT,SkipT>::instance())
        ,opts_(opts)
      {}

      size_t entityIndex() const {
        return EntId<U>::value;
      }

      bool readUnfolded(XmlReader<void> const& r, Any const& v) const {
        opts_.action(r.impl());
        return unfoldedRead_.read(r,v);
      }

      bool writeUnfolded(XmlWriter<void> const& w, Any const& v) const {
        return unfoldedWrite_.write(w,v);
      }

      bool read(XmlReader<void> const& r, Any const& v) const {
        opts_.action(r.impl());
        return readDisp_.read(r,v);
      }

      bool write(XmlWriter<void> const& w, Any const& v) const {
        return writeDisp_.write(w,v);
      }
    private:
      //TODO: we could go down to alias here if we move to element's reader
      priv::XmlElementReader<TagT,U> readDisp_;
      priv::XmlElementReader<TagT,U> unfoldedRead_;  // TODO: why not use ReadConent
      priv::XmlElementWriter<TagT,U> writeDisp_;
      priv::XmlElementWriter<TagT,U> unfoldedWrite_;
      XmlOptions const& opts_;
    };


    template<typename T>
    XmlInfo<IsMultiConstr<T>::value> const& xmlInfo() {
      typedef typename GetTypeIdPolicy<T,void>::type tp_t;
      typedef typename XmlKind<T,void>::type kind_t;
      typedef typename UnAlias<T>::type u_t;
      typedef typename GetXmlOptions<T>::type opt_t;

      static ConcrXmlInfo<
          void,
          u_t,
          typename SkipXmlElement<T>::type,
          IsMultiConstr<T>::value,
          kind_t
      > ret = ConcrXmlInfo<void,u_t,typename SkipXmlElement<T>::type,
                           IsMultiConstr<T>::value,kind_t>(
          XmlName<T,void>::value(),
          ElementId(XmlName<T,void>::value(),getNilAttrName(),"yes"),
          HasXmlIndependentOrder<T,void>::value,
          tp_t::instance(),
          opt_t::instance()
        );
      return ret;
    }
  }
}

#endif
