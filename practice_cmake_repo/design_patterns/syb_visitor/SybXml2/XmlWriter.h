/**
\brief The module implements generic function for deserializing any SYB
enabled object from XML

Usage:
  std::ofstream f("someobj.xml");

  SomeObj obj(.....);
  .....

  f << syb::toXml(obj);

\todo redo to XString, Lycia error reporting, and loging
\todo empty elements
\todo bool
\todo formalize
\todo multi constr dispatch for attributes (very minor priority)
*/
#ifndef SYB_XML_WRITER_H
#define SYB_XML_WRITER_H

#if 0
#include "SybXml2/XmlTypes.h"
#include "SybXml2/XmlReader.h"
#include "Syb/Any.h"
#include "Syb/SybAll.h"
#include "Syb/Enum.h"
#include "Syb/Trace.h"
#include "Syb/IToStr.h"
#else
#include "XmlTypes.h"
#include "XmlReader.h"
#include "../Syb/Any.h"
#include "../Syb/SybAll.h"
#include "../Syb/Enum.h"
#include "../Syb/Trace.h"
#include "../Syb/IToStr.h"
#endif

#include <loki/Pimpl.h>
#include <boost/lexical_cast.hpp>
#include <unicode/unistr.h>

namespace qx {
  namespace syb {

    class SybXmlBoolStringPolicy {
    public:
      void yesNo() {
        onSet_ = "yes";
        onUnset_ = "no";
      }
      void trueFalse() {
        onSet_ = "true";
        onUnset_ = "false";
      }
      const std::string& onSet() const {
        return onSet_;
      }
      const std::string& onUnset() const {
        return onUnset_;
      }
      static SybXmlBoolStringPolicy& instance() {
        static SybXmlBoolStringPolicy ret;
        return ret;
      }
    private:
      SybXmlBoolStringPolicy() {
        yesNo();
      }
      std::string onSet_;
      std::string onUnset_;
    };

    struct AnyElementImpl;

    namespace priv {

      /// Template for affecting behaviour of writer depending
      /// on type properties
      template<typename T, typename TagT,
        typename U = typename UnAlias<T>::type, typename EnableT=void>
      struct XmlWriterDispatcher;

      /// Template for affecting behaviour of writer for element
      // case depending on type properties
      template<typename TagT,typename U,typename EnableT = void>
      struct XmlElementWriter;

      /// Writer's state and interface for original XML writer
      struct XmlWriterImpl {
        XmlWriterImpl();
        ~XmlWriterImpl();

        /// Adds a child to the current parent node
        void beginElement(ElementId const&);

        /// Closes the current parent node, its parent becames the current
        void endElement();

        /// set current element's text
        void setBody(std::string const&);

        /// set current element's text with binary encoding (base64 encoded)
        void setBody(std::vector<unsigned char> const&);

        /// set current element's unicode text
        void setBody(UnicodeString const&);

        /// adds attribute to the current element
        void setAttribute(QName const&,std::string const&);

        /// adds unicode attribute to the current element
        void setAttribute(QName const&,UnicodeString const&);

        /// adds global namespace to root element
        void setRootNamespace(const char *rootNs);

        /// outputs all result to the underlying std::stream
        void flush(std::ostream& o, bool isIndent, bool isLowerCase);

        /// checks if an object with provided identity was already serialized
        /// in the current session
        bool isComputed(cfg::key_t);

        /// adds object's identity to a set of computed object's identities
        void setComputed(cfg::key_t);

        /// set's writer context
        void addAnyElement(boost::shared_ptr<AnyElementImpl> const&);


        // next set of function just for compilation optimization

      private:
        Loki::PimplOf<XmlWriterImpl>::Type impl_;
        XmlWriterImpl(const XmlWriterImpl& );
      };
    }

    /// Generic function for customizable XML writer serialization
    template<typename TagT>
    struct XmlWriter:DynFun {
/*
#ifdef NOT_INLINES
      template<typename T>
      inline void operator()(T const& v) const
      {
        notInline(*this,v);
      }

      template<typename T>
      inline void runInline(T const& v) const {
        priv::XmlWriterDispatcher<T,TagT>::action(*this,v);
      }
#else
      template<typename T>
      inline void operator()(T const& v) const {
        priv::XmlWriterDispatcher<T,TagT>::action(*this,v);
      }
#endif
*/
      void action(Any const& v) const {
        v.getDynInfo().getXmlInfo().write(*this,v);
      }


      inline priv::XmlWriterImpl& impl() const { return impl_; }

    private:
      mutable priv::XmlWriterImpl impl_;

      template<typename T, typename TagUT, typename U, typename EnableT>
      friend struct priv::XmlWriterDispatcher;

      template<typename TagUT,typename U,typename EnT>
      friend struct priv::XmlElementWriter;
    };

    namespace priv {

      /// By means of this template it's possible to customize format
      /// of primitive values
      template<typename T, typename EnT=void>
      struct XmlPrimToString {
        inline static std::string action(T const& v) {
          return boost::lexical_cast<std::string>(v);
        }
      };

      template<>
      struct XmlPrimToString<bool> {
        inline static std::string action(bool v) {
          return v ? SybXmlBoolStringPolicy::instance().onSet() :
                     SybXmlBoolStringPolicy::instance().onUnset();
        }
      };

      template<>
      struct XmlPrimToString<int> {
        inline static std::string action(int v) {
          return itostr(v);
        }
      };

      template<>
      struct XmlPrimToString<unsigned int> {
        inline static std::string action(unsigned int v) {
          return itostr(v);
        }
      };

      template<typename TagT>
      struct AbsWriteContent {
        virtual void write(XmlWriter<TagT> const& w, Any const& v) const {
          v.run<dyn_fun_t const>(&w);
        }
      };

      template<typename TagT, typename U, typename EnT=void>
      struct XmlWriteContent:AbsWriteContent<TagT>{};

      template<typename TagT, typename U>
      struct XmlWriteContent<TagT, U
        ,typename boost::enable_if<XmlPrim<U> >::type> : AbsWriteContent<TagT>
      {
        void write(XmlWriter<TagT> const& w, Any const& v) const {
          w.impl().setBody(XmlPrimToString<U>::action(fromAny<U const>(v)));
        }
      };

      template<typename TagT, typename U>
      struct XmlWriteContent<TagT, boost::optional<U>
        ,typename boost::enable_if<XmlPrim<U> >::type>:AbsWriteContent<TagT>
      {
        void write(XmlWriter<TagT> const& w, Any const& v) const {
          boost::optional<U> const& vv = fromAny<boost::optional<U> const>(v);
          if (vv)
            w.impl().setBody(XmlPrimToString<U>::action(*vv));
        }
      };

      template<typename TagT, typename U>
      struct XmlWriteContent<TagT, boost::optional<U>
        ,typename boost::enable_if<boost::is_same<std::vector<unsigned char>
        ,typename boost::remove_const<U>::type> >::type
      >
      {
        inline static void action(
          XmlWriter<TagT> const& w, boost::optional<U> const& v)
        {
          if (v)
            w.impl().setBody(*v);
        }
      };

      template<typename TagT, typename U>
      struct XmlWriteContent<TagT, U
        ,typename boost::enable_if<IsEnum<U> >::type> : AbsWriteContent<TagT>
      {
        void write(XmlWriter<TagT> const& w, Any const& v) const {
          w.impl().setBody(fromAny<U const>(v).strVal());
        }
      };

      template<typename TagT>
      struct XmlWriteFormat {
        virtual void begin(XmlWriter<TagT> const&, ElementId const&) const = 0;
        virtual void end(XmlWriter<TagT> const&) const = 0;
        virtual boost::optional<ElementId> id(
          XmlWriter<TagT> const&, Any const&, mpl::true_ const&) const = 0;
        virtual boost::optional<ElementId> id(
          XmlWriter<TagT> const&, Any const&, mpl::false_ const&) const = 0;
        virtual bool dispatchWrite(XmlWriter<TagT> const&, Any const&) const = 0;
      };

      template<typename TagT, typename SkipT>
      struct XmlElWriteFormat : XmlWriteFormat<TagT> {

        boost::optional<ElementId>
          id(XmlWriter<TagT> const& w, Any const& v, mpl::true_ const&) const
        {
          if (!v.getDynInfo().getDataType().dynHasConstructor(v))
            return boost::none;
          AbsConstrInfo const& c
            = v.getDynInfo().getDataType().dynFromValue(v);
          XmlInfo<true> const& ci
            = *boost::polymorphic_downcast<XmlInfo<true> const*>(
                &v.getDynInfo().getXmlInfo()
              );
          return ci.typeIdPolicy().elementId(
            ci.getName(),QName(c.getXmlName(),ci.getName().ns(), true)
            );
        }
        boost::optional<ElementId>
          id(XmlWriter<TagT> const& w, Any const& v, mpl::false_ const&) const
        {
          if (!v.getDynInfo().getDataType().dynHasConstructor(v))
            return boost::none;
          XmlInfo<false> const& ci
            = *boost::polymorphic_downcast<XmlInfo<false> const*>(
                &v.getDynInfo().getXmlInfo()
              );
          return ElementId(ci.typeIdPolicy().elementId(ci.getName()));
        }
        void begin(XmlWriter<TagT> const& w, ElementId const& e) const {
          w.impl().beginElement(e);
        }
        void end(XmlWriter<TagT> const& w) const {
            w.impl().endElement();
        }
        bool dispatchWrite(XmlWriter<TagT> const& w, Any const& vv) const {
          return vv.getDynInfo().getXmlInfo().write(w,vv);
        }
        static XmlWriteFormat<TagT> const& instance() {
          static XmlElWriteFormat ret = XmlElWriteFormat();
          return ret;
        }
      };

      template<typename TagT>
      struct XmlElWriteFormat<TagT,mpl::true_>
        : XmlWriteFormat<TagT>
      {
        boost::optional<ElementId>
          id(XmlWriter<TagT> const& w, Any const& v, mpl::false_ const&) const
        {
          if (!v.getDynInfo().getDataType().dynHasConstructor(v))
            return boost::none;
          return ElementId();
        }
        boost::optional<ElementId>
          id(XmlWriter<TagT> const& w, Any const& v, mpl::true_ const&) const
        {
          if (!v.getDynInfo().getDataType().dynHasConstructor(v))
            return boost::none;
          AbsConstrInfo const& c
            = v.getDynInfo().getDataType().dynFromValue(v);
          return ElementId("", "type", c.getName());
        }
        void begin(XmlWriter<TagT> const& w, ElementId const& e) const {
          if (e.attr())
            w.impl().setAttribute(e.attr()->first,e.attr()->second);
        }
        void end(XmlWriter<TagT> const& w) const {}
        bool dispatchWrite(XmlWriter<TagT> const& w, Any const& vv) const {
          return vv.getDynInfo().getXmlInfo().writeUnfolded(w,vv);
        }

        static XmlWriteFormat<TagT> const& instance() {
          static XmlElWriteFormat ret = XmlElWriteFormat();
          return ret;
        }
      };

      template<typename TagT, typename IsMultiConstrT>
      struct DefaultXmlElWriter {
        inline explicit DefaultXmlElWriter(
          AbsWriteContent<TagT> const& cw,
          XmlWriteFormat<TagT> const& f
          ):contWriter_(cw), format_(f){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          boost::optional<ElementId> e = format_.id(w,v,IsMultiConstrT());
          if (e) {
            format_.begin(w,*e);
            contWriter_.write(w,v);
            format_.end(w);
            return true;
          }
          return false;
        }
      private:
        AbsWriteContent<TagT> const& contWriter_;
        XmlWriteFormat<TagT> const& format_;
      };

      //TODO: move parts to cpp
      template<typename TagT,typename U,typename EnT>
      struct XmlElementWriter
        :DefaultXmlElWriter<TagT,typename IsMultiConstr<U>::type>
      {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f)
          :DefaultXmlElWriter<TagT,typename IsMultiConstr<U>::type>(
              contentWriter_,f
            ){}
      private:
        XmlWriteContent<TagT,U> contentWriter_;
      };

      template<typename TagT, typename U>
      struct XmlElementWriter<TagT,U,
        typename boost::enable_if<IsEnum<U> >::type>
      {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f)
          :format_(f){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          U const& vv = fromAny<U const>(v);
          if (vv.val()) {
            format_.begin(w,ElementId(v.getDynInfo().getXmlInfo().getName()));
            w.impl().setBody(vv.strVal());
            format_.end(w);
            return true;
          }
          return false;
        }
      private:
        XmlWriteFormat<TagT> const& format_;
      };

      template<typename TagT, typename U>
      struct XmlElementWriter<TagT,boost::optional<U> >
      {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f)
          :format_(f){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          boost::optional<U> const& vv = fromAny<boost::optional<U> const>(v);
          if (vv) {
            format_.begin(w,ElementId(v.getDynInfo().getXmlInfo().getName()));
            contentWriter_.write(w,toAny(*vv));
            format_.end(w);
            return true;
          }
          return false;
        }
      private:
        XmlWriteContent<TagT,U> contentWriter_;
        XmlWriteFormat<TagT> const& format_;
      };

      template<typename TagT>
      struct XmlElementWriter<TagT,Any> {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f)
          :format_(f){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          //TODO!: format
          Any const& vv = fromAny<Any const>(v);
          // * check vv is inited
          // * if not inited by has initilizer from writer get its string
          if (vv.empty()) {
            if (vv.getInitializer()) {
              typedef typename XmlElementReader<TagT,Any>::Initializer init_t;
              boost::shared_ptr<init_t> init = boost::dynamic_pointer_cast<init_t>(vv.getInitializer());
              if(!init)
                return false;
              w.impl().addAnyElement(init->state());
              return true;
            } else {
              return false;
            }
          }
          if (!format_.dispatchWrite(w,vv)) {
            format_.begin(w,ElementId(vv.getDynInfo().getXmlInfo().getName()));
            w.impl().setAttribute(getNilAttrName(), SybXmlBoolStringPolicy::instance().onSet());
            format_.end(w);
          }
          return true;
        }
      private:
        XmlWriteFormat<TagT> const& format_;
      };

      template<typename TagT>
      struct XmlElementWriter<TagT,std::vector<unsigned char> >
      {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f)
          :format_(f){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          format_.begin(w,ElementId(v.getDynInfo().getXmlInfo().getName()));
          w.impl().setBody(fromAny<std::vector<unsigned char> const>(v));
          format_.end(w);
          return true;
        }
      private:
        XmlWriteFormat<TagT> const& format_;
      };

      //TODO: do this on SYB layer
      template<typename TagT>
      struct XmlElementWriter<TagT,Unit> {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const&){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          return true;
        }
      };

      //TODO: this won't work in array of bools!!!!
      template<typename TagT>
      struct XmlElementWriter<TagT,bool> {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const&){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          if (fromAny<bool const>(v)) {
            w.impl_.beginElement(ElementId(v.getDynInfo().getXmlInfo().getName()));
            w.impl_.endElement();
          }
          return true;
        }
      };

      template<typename T,typename TagT,typename U>
      struct XmlWriterDispatcher<T,TagT,U,typename boost::enable_if<
          boost::is_base_of<XmlElementTag,XmlKind<T,TagT> >
        >::type>:XmlElementWriter<TagT,U>
      {
        inline XmlWriterDispatcher():XmlElementWriter<TagT,U>(
          XmlElWriteFormat<TagT,typename SkipXmlElement<T>::type>::instance()
          ){}
      };

      // this instance is for optimization, it should work without it
      template<typename TagT, typename U>
      struct XmlElementWriter<TagT,U
        ,typename boost::enable_if<IsCont<U> >::type
        >
      {
        typedef typename U::value_type val_t;
        typedef typename U::const_iterator iter_t;
        typedef typename UnAlias<val_t>::type uval_t;

        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f)
          :elWriter_(XmlElWriteFormat<TagT,mpl::false_>::instance()),format_(f){}

        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          U const& vv = fromAny<U const>(v);
          format_.begin(w,ElementId(v.getDynInfo().getXmlInfo().getName()));
          for (iter_t i = vv.begin(); i != vv.end(); ++i) {
            if (!elWriter_.write(w, toAny(*i))) {
              w.impl().beginElement(getNilElementId<val_t,TagT>());
              w.impl().endElement();
            }
          }
          format_.end(w);
          return true;
        }
      private:
        XmlElementWriter<TagT,uval_t> elWriter_;
        XmlWriteFormat<TagT> const& format_;
      };

      /// Customizes format of attributes' value
      template<typename TagT, typename U, typename EnableT = void>
      struct XmlAttributeWriter;


      template<typename TagT, typename U>
      struct XmlAttributeWriter<TagT,U,
        typename boost::enable_if<XmlPrim<U> >::type>
      {
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          w.impl().setAttribute(v.getDynInfo().getXmlInfo().getName()
            ,XmlPrimToString<U>::action(fromAny<U const>(v)));
          return true;
        }
      };

      template<typename TagT, typename U>
      struct XmlAttributeWriter<TagT,boost::optional<U>,
        typename boost::enable_if<XmlPrim<U> >::type>
      {
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          boost::optional<U> const& vv = fromAny<boost::optional<U> const>(v);
          if (vv)
            w.impl().setAttribute(v.getDynInfo().getXmlInfo().getName()
              ,XmlPrimToString<U>::action(*vv));
          return true;
        }
      };

      template<typename TagT, typename U>
      struct XmlAttributeWriter<TagT,U,
        typename boost::enable_if<IsEnum<U> >::type>
      {
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          U const& vv = fromAny<U const>(v);
          if (vv.val())
            w.impl().setAttribute(
              v.getDynInfo().getXmlInfo().getName(),vv.strVal()
              );
          return true;
        }
      };

      template<typename TagT, typename U>
      struct XmlAttributeWriter<TagT,boost::optional<U>,
        typename boost::enable_if<IsEnum<U> >::type>
      {
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          boost::optional<U> const& vv = fromAny<boost::optional<U> const>(v);
          if (vv)
            w.impl().setAttribute(
                v.getDynInfo().getXmlInfo().getName(),vv->strVal()
              );
          return true;
        }
      };

      template<typename TagT>
      struct XmlAttributeWriter<TagT,bool,void> {
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          if (fromAny<bool const>(v))
            w.impl().setAttribute(
              v.getDynInfo().getXmlInfo().getName(),
              SybXmlBoolStringPolicy::instance().onSet()
              );
          return true;
        }
      };

      template<typename T,typename TagT,typename U>
      struct XmlWriterDispatcher<T,TagT,U,typename boost::enable_if<
        boost::is_base_of<XmlAttributeTag,XmlKind<T,TagT> >
      >::type>:XmlAttributeWriter<TagT,U> {};

      template<typename T, typename TagT,typename U>
      struct XmlWriterDispatcher<T,TagT,U,typename boost::enable_if<
        boost::is_base_of<XmlTextTag,XmlKind<T,TagT> >
      >::type> {
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          content_.write(w,v);
          return true;
        }
      private:
        XmlWriteContent<TagT,U> content_;
      };
    }

    template<typename T, typename TagT>
    struct XmlWriterManip {

      inline XmlWriterManip(T const& v, bool isIndent, bool isLowerCase)
        : val_(v), isIndent_(isIndent), isLowerCase_(isLowerCase) {
      }

      void doWrite(std::ostream&, bool isIndent, bool isLowerCase) const;

      friend inline std::ostream&
        operator<<(std::ostream& f, XmlWriterManip const& t)
      {
        t.doWrite(f, t.isIndent_, t.isLowerCase_);
        return f;
      }

    private:
      T const& val_;
      bool isIndent_;
      bool isLowerCase_;
    };

    /**
    Returns output stream manipulator for, which writes the argument
    to the stream, and allows to define additional Tag which can be used
    for providing custom formats.

    For example:

      struct MyField;

      template<>
      struct XmlKind<MyField,MakeMyFieldToBeAnAttribute>:XmlAttributeKind{};

      std::cout << toXml<MakeMyFieldToBeAnAttribute>(val);

    */
    template<typename TagT, typename T>
    XmlWriterManip<T,TagT> toXml(T const& val, bool isIndent = false, bool isLowerCase = false) {
      return XmlWriterManip<T,TagT>(val, isIndent, isLowerCase);
    }

    /**
    Returns output stream manipulater for, which writes the argument
    to the stream


    For example:

      std::cout << toXml(val);

    */
    template<typename T>
    XmlWriterManip<T,void> toXml(T const& val,  bool isIndent = false, bool isLowerCase = false) {
      return XmlWriterManip<T,void>(val, isIndent, isLowerCase);
    }
  }
}

template<>
struct TraceName<qx::syb::priv::XmlWriterImpl> {
  inline static std::string const& value() {
    static std::string ret = "write xml:";
    return ret;
  }
};

#endif
