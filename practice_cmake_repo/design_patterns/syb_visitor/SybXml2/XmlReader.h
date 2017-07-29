/**
\brief The module implements generic function for deserializing any SYB 
enabled object from XML

Usage:
  std::ifstream f("someobj.xml");

  SomeObj obj;
  f >> syb::fromXml(obj);


\todo redo to XString, Lycia error reporting, and logging
\todo exceptions (not found attribute/element)
\todo memory leaks
\todo probably libXML2
*/
#ifndef SYB_XML_READER_H
#define SYB_XML_READER_H

#if 0
#include "Syb/ObjectIdentity.h"
#include "Syb/Any.h"
#include "SybXml2/XmlTypes.h"
#include "Syb/Trace.h"
#include "Syb/SybAll.h"

#include "Syb/UnicodeString.h"

#else
#include "../Syb/ObjectIdentity.h"
#include "../Syb/Any.h"
#include "../SybXml2/XmlTypes.h"
#include "../Syb/Trace.h"
#include "../Syb/SybAll.h"

#include "../SybBoostSerialization/UnicodeString.h"
#endif

//#include "config/lycia_errors.h"

#include <loki/Pimpl.h>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/function.hpp>
#include <boost/ref.hpp>
#include <unicode/unistr.h>
#include <boost/exception/all.hpp>
#include <boost/algorithm/string.hpp>


namespace qx {
  namespace syb {

    struct AnyElementImpl;

    namespace priv {

      /// Customizes XML node output
      template<typename T, typename TagT, 
        typename U = typename UnAlias<T>::type, typename EnableT=void>
      struct XmlReaderDispatcher;

      template<typename TagT>
      struct AbsXmlReaderFormat {
        virtual bool null(XmlReader<TagT> const&, Any const&) const = 0;
        virtual void begin(XmlReader<TagT> const&, ElementId const&) const = 0;
        virtual bool check(XmlReader<TagT> const&, ElementId const&) const = 0;
        virtual void end(XmlReader<TagT> const&) const = 0;
        virtual bool dispatchRead(XmlReader<TagT> const&, Any const&) const = 0;
        virtual std::string getAttribute(XmlReader<TagT> const&, QName const&) const = 0;
      };

      /// Customizes XML element output (it's run by XmlReaderDispatcher)
      template<typename TagT
        , typename U
        , typename MultiConstrT = typename IsMultiConstr<U>::type
        , typename EnT = void
      >
      struct XmlElementReader;

      /// Represents state of XML reader and interface to 
      /// underlying XML parser
      /// \todo constify
      struct XmlReaderImpl : XmlElementTypeReader {
        XmlReaderImpl(std::istream& out, bool isPreprocess);
        explicit XmlReaderImpl(boost::shared_ptr<AnyElementImpl> const& out);
        ~XmlReaderImpl();
        
        //checks is current element has provided name
        //if no just returns false
        //if yes enters the element
        
        /// \brief Checks next sibling has provided identity
        bool checkNextElement(ElementId const&);

        /// \brief Enters next sibling if it has provided identity
        void beginElement(ElementId const&);
        /// \brief Enters next sibling regardless its name
        void beginElement();
        /// \brief Closes traversal of an element
        void endElement();

        /// \brief Returns body's inner text
        std::string getBody();
        /// \brief Returns body's inner text
        void getBody(std::vector<unsigned char>&);
        UnicodeString getUnicodeBody();
        /// \brief Returns value of an attribute for a current element
        std::string getAttribute(QName const& q);
        UnicodeString getUnicodeAttribute(QName const& q);
        boost::optional<UnicodeString> getOptionalUnicodeAttribute(QName const& n);
        /// \brief Returns value of an attribute for a not yet opened element
        std::string getNextElAttribute(QName const& q);
        /// \brief Checks the next sibling is available
        bool hasNext();
        boost::shared_ptr<AnyElementImpl> getAnyElement();
        /// \brief returns all the attributes for current element
        void loadAttributes(std::list<std::pair<std::string,UnicodeString> >&);
        // void setCurrentElement(boost::shared_ptr<AnyElementImpl> const& e);
        QName getElementName();
        void flush();
        void skipElement();
        bool setUnpos(bool save) const;
        void setCaseSensitive(bool isCaseSensitive);
        std::string currentElementLocalName();
      private:
        Loki::PimplOf<XmlReaderImpl>::Type impl_;
      };
    }
   
    template<typename T, typename EnableT=void> 
    struct HasXmlIndependentOrder:mpl::false_{};

    template<typename T>
    struct HasXmlIndependentOrder<std::list<T>,
      typename boost::enable_if<HasXmlIndependentOrder<T> >::type>:mpl::true_{};

    template<typename T>
    struct HasXmlIndependentOrder<boost::optional<T>,
      typename boost::enable_if<HasXmlIndependentOrder<T> >::type>:mpl::true_{};

    template<typename T>
    struct HasXmlIndependentOrder<boost::shared_ptr<T>,
      typename boost::enable_if<HasXmlIndependentOrder<T> >::type>:mpl::true_{};

    struct XmlOptions {
      virtual void action(priv::XmlReaderImpl &r) const;
      virtual ~XmlOptions();
      inline static XmlOptions const& instance() {
        static XmlOptions ret = XmlOptions();
        return ret;
      }
    };

    struct CaseSensitive : XmlOptions {
      void action(priv::XmlReaderImpl &r) const;
      inline static CaseSensitive const& instance() {
        static CaseSensitive ret = CaseSensitive();
        return ret;
      }
    };

    struct CaseInsensitive : XmlOptions {
      void action(priv::XmlReaderImpl &r) const;
      inline static CaseInsensitive const& instance() {
        static CaseInsensitive ret = CaseInsensitive();
        return ret;
      }
    };

    template<typename T, typename EnableT=void>
    struct GetXmlOptions:Ret<XmlOptions>{};

    /**
    \brief Actual generic dynamic function for XML parsing
    */
    template<typename TagT>
    struct XmlReader:DynFun {
      inline explicit XmlReader(std::istream& i, bool isPreprocess)
        : impl_(boost::make_shared<priv::XmlReaderImpl>(boost::ref(i), isPreprocess)) {
      }
      inline explicit XmlReader(boost::shared_ptr<priv::XmlReaderImpl> const& i)
        :impl_(i){}

      inline priv::XmlReaderImpl& impl() const { return *impl_; }
      inline boost::shared_ptr<priv::XmlReaderImpl> const& implPtr() const {
        return impl_; 
      }

      void action(Any const& v) const {
        v.getDynInfo().getXmlInfo().read(*this,v);
      }

      bool setUnpos(bool save) const {
        return impl_->setUnpos(save);
      }
    private:

      boost::shared_ptr<priv::XmlReaderImpl> impl_;
    };

    template<typename TagT, typename T>
    void dynRead(XmlReader<TagT> const& r, T& v) {
        Any av = toAny(v);
        av.run<dyn_fun_t const>(&r);
    }

    template<class T1, class T2>
    struct DefualtValAssigner<std::pair<const T1, T2> > {
      static void assignDefVal(std::pair<const T1, T2>& v) {
        const_cast<T1&>(v.first) = T1();
        v.second = T2();
      }
    };

    template<typename TagT, typename T>
    void dynRead(XmlReader<TagT> const& r, T& v);

    namespace priv {

      template<typename U, typename EnT=void>
      struct XmlStringToPrim {
        inline static void action(U& v, std::string const& vs) {
          if (!vs.empty())
            v = boost::lexical_cast<U>(vs);
        }
      };

      template<typename U>
      struct XmlStringToPrim<U, typename boost::enable_if<IsEnum<U> >::type> {
        inline static void action(U& v, std::string const& vs) {
          if (!vs.empty())
            v.strVal(vs);
        }
      };

      template<>
      struct XmlStringToPrim<bool> {
        inline static void action(bool& v, std::string const& vs) {
          v = (vs == "yes" || vs == "true");
        }
      };


      template<typename TagT>
      struct XmlReadConstr {
        inline static boost::optional<std::pair<ElementId,AbsConstrInfo const*> > 
          read(XmlReader<TagT> const& r, Any const& v, TypeIdPolicy<true> const& tp) 
        {
          if (!r.impl().hasNext())
            return boost::none;
          const std::string &ident = tp.getTypeId(r.impl());
          AbsConstrInfo const* c
            = v.getDynInfo().getDataType().dynByXmlName(ident);
          if (!c) 
            return boost::none;
          const QName &qname = v.getDynInfo().getXmlInfo().getName();
          return std::make_pair(tp.elementId(
            qname,
            //TODO: probably it's worth extending constructor's info 
            // with XML data thus we'll be able to use XmlElementName instead
            // using parent's namespace
            QName(ident, qname.ns(), qname.nsPrefix(), true)), c);
        }

        inline static boost::optional<std::pair<ElementId,AbsConstrInfo const*> > 
          read(XmlReader<TagT> const& r, Any const& v, TypeIdPolicy<false> const& tp) 
        {
          return std::make_pair(
            tp.elementId(v.getDynInfo().getXmlInfo().getName()),
            &v.getDynInfo().getDataType().constrByIndex(0)
            );
        }
      };

      template<typename TagT>
      struct AbsXmlReadContent {
        virtual void read(XmlReader<TagT> const& r, Any const& v) const {
          v.run<dyn_fun_t const>(&r);
        }
      };

      template<typename TagT, typename U, typename En=void>
      struct XmlReadContent : AbsXmlReadContent<TagT>  {};

      template<typename TagT, typename U>
      struct XmlReadContent<TagT,U
        ,typename boost::enable_if<IsEnum<U> >::type
      > : AbsXmlReadContent<TagT>
      {
        void read(XmlReader<TagT> const& w, Any const& v) const {
           XmlStringToPrim<U>::action(fromAny<U>(v), w.impl().getBody());
        }
      };

      template<typename TagT,typename U>
      struct XmlReadContent<TagT,U
        ,typename boost::enable_if<mpl::and_<XmlPrim<U>, mpl::not_<IsUnicode<U> > > >::type
      > : AbsXmlReadContent<TagT>
      {
        void read(XmlReader<TagT> const& w, Any const& v) const {
          try {
           XmlStringToPrim<U>::action(fromAny<U>(v), w.impl().getBody());
          } catch(std::bad_cast &bd) {
            AbsXmlInfo const& i = v.getDynInfo().getXmlInfo();
            throw std::runtime_error(
              boost::str(boost::format("\'%1%\'(xmlns:%2%=\'%3%\') %4%") %
              i.getName().name() % i.getName().nsPrefix() % i.getName().ns() % bd.what()).c_str());
          }
        }
      };

      // TODO: delete
      template<typename TagT,typename U>
      struct XmlReadContent<TagT,boost::optional<U>
        ,typename boost::enable_if<mpl::and_<XmlPrim<U>, mpl::not_<IsUnicode<U> > > >::type
      > : AbsXmlReadContent<TagT>
      {
        void read(XmlReader<TagT> const& w, Any const& v) const {
          boost::optional<U>& ret = fromAny<boost::optional<U> >(v);
          if (!w.impl().getBody().empty()) { //TODO: make more accurate check
            ret.reset(U());
            XmlStringToPrim<U>::action(*ret, w.impl().getBody());
          }
        }
      };

      template<typename TagT, typename IsMultiConstrT>
      struct XmlReaderFormat : AbsXmlReaderFormat<TagT> {
        virtual boost::optional<std::pair<ElementId,AbsConstrInfo const*> > constr(
            XmlReader<TagT> const& w, Any const& v) const = 0;
      };

      template<typename TagT,typename SkipT,typename IsMultiConstrT>
      struct XmlElReadFormat : XmlReaderFormat<TagT,IsMultiConstrT> {
        bool null(XmlReader<TagT> const& w, Any const& v) const {
          ElementId e = v.getDynInfo().getXmlInfo().getNilElementId();
          if (!w.impl().checkNextElement(e))
            return false;
          v.getDynInfo().getDataType().setDefault(v);
          w.impl().beginElement(e);
          w.impl().endElement();
          return true;
        }

        boost::optional<std::pair<
            ElementId,
            AbsConstrInfo const*
        > > constr(
            XmlReader<TagT> const& w, 
            Any const& v
          ) const
        {
          return XmlReadConstr<TagT>
            ::read(w,v,
              boost::polymorphic_downcast<
                  XmlInfo<IsMultiConstrT::value> const*
                >(&v.getDynInfo().getXmlInfo())->typeIdPolicy()
              );
        }

        void begin(XmlReader<TagT> const& w,ElementId const& e) const {
          w.impl().beginElement(e);
        }

        bool check(XmlReader<TagT> const& w,ElementId const& e) const {
          return w.impl().checkNextElement(e);
        }

        void end(XmlReader<TagT> const& w) const {
          w.impl().endElement();
        }

        bool dispatchRead(XmlReader<TagT> const& r, Any const& v) const {
          return v.getDynInfo().getXmlInfo().read(r,v);
        }

        std::string getAttribute(XmlReader<TagT> const& r, QName const& n) const {
          return r.impl().getNextElAttribute(n);
        }

        static XmlReaderFormat<TagT,IsMultiConstrT> const& instance() {
          static XmlElReadFormat ret = XmlElReadFormat();
          return ret;
        }
      };

      template<typename TagT, typename IsMultiConstrT>
      struct XmlElReadFormat<TagT,mpl::true_,IsMultiConstrT> 
        : XmlReaderFormat<TagT,IsMultiConstrT> 
      {
        bool null(XmlReader<TagT> const& w, Any const& v) const {
          if (w.impl().getAttribute(getNilAttrName()) != "yes") 
           return false;
          v.getDynInfo().getDataType().setDefault(v);
          return true;
        }

        boost::optional<std::pair<
            ElementId,
            AbsConstrInfo const*
        > > constr(
            XmlReader<TagT> const& w, 
            Any const& v
          ) const
        {
          return XmlReadConstr<TagT>
            ::read(w,v,AnyElementTypeId<IsMultiConstrT::value>::instance());
        }

        void begin(XmlReader<TagT> const& w,ElementId const& e) const {
        }

        bool check(XmlReader<TagT> const& w,ElementId const& e) const {
          return true;
        }

        void end(XmlReader<TagT> const& w) const { }

        bool dispatchRead(XmlReader<TagT> const& r, Any const& v) const {
          return v.getDynInfo().getXmlInfo().readUnfolded(r,v);
        }

        std::string getAttribute(XmlReader<TagT> const& r, QName const& n) const {
          return r.impl().getAttribute(n);
        }

        static XmlReaderFormat<TagT,IsMultiConstrT> const& instance() {
          static XmlElReadFormat ret = XmlElReadFormat();
          return ret;
        }

      };

      template<typename TagT, typename U>
      struct XmlReadContent<TagT,U
        ,typename boost::enable_if<IsCont<U> >::type
      > : AbsXmlReadContent<TagT>
      {
        typedef typename U::value_type val_t;
        typedef typename UnAlias<val_t>::type uval_t;
        inline XmlReadContent()
          :elReader_(XmlElReadFormat<
              TagT,mpl::false_,typename IsMultiConstr<uval_t>::type
          >::instance()){}
        void read(XmlReader<TagT> const& w, Any const& v) const {
          doAction(w,fromAny<U>(v));
        }
      private:
        template<typename UT>
        inline void doAction(XmlReader<TagT> const& w, UT& v) const {
          while(w.impl().hasNext()) {
            val_t r;
            v.push_back(r);
            if(!elReader_.read(w, toAny(v.back()))) {
              v.pop_back();
              break;
            }
          }
        }

        inline void doAction(
          XmlReader<TagT> const& w, std::vector<unsigned char>& v) const
        {
          w.impl().getBody(v);
        }

        XmlElementReader<
                    TagT,uval_t,
                    typename IsMultiConstr<uval_t>::type
                  > elReader_;
      };

      template<typename TagT>
      struct DefaultMultiConstrRead {
        inline DefaultMultiConstrRead(
            XmlReaderFormat<TagT,mpl::true_> const& f,
            AbsXmlReadContent<TagT> const& reader
          ):format_(f),contentReader_(reader){};

        bool read(XmlReader<TagT> const& w, Any const& v) const {
          if (format_.null(w,v)) 
            return true;
          bool ret = false;
          boost::optional<std::pair<
            ElementId,
            AbsConstrInfo const*> 
          > e = format_.constr(w,v);
          if (!e || !format_.check(w,e->first))
            return false;
          bool save = w.setUnpos(
            v.getDynInfo().getXmlInfo().hasIndependentOrder());
          format_.begin(w,e->first);
          AbsConstrInfo const* tmp = e->second;
          tmp->dynBuild(v);
          // v.run<dyn_fun_t const>(&w);
          contentReader_.read(w,v);
          // XmlReadContent<TagT,U>::action(w,v,i); -- going inside is the only option for multiconstr
          format_.end(w);
          w.setUnpos(save);
          return true;
        }
      private:
        XmlReaderFormat<TagT,mpl::true_> const& format_;
        AbsXmlReadContent<TagT> const& contentReader_;
      };

      template<typename TagT,typename U,typename IsMultiConstrT>
      struct DefaultXmlElementReader : DefaultMultiConstrRead<TagT>  {
        inline explicit DefaultXmlElementReader(
          XmlReaderFormat<TagT,IsMultiConstrT> const& f
          ) : DefaultMultiConstrRead<TagT>(f,contentReader_){}
      private:
        XmlReadContent<TagT,U> contentReader_; // TODO: make singelton
      };

      template<typename TagT, typename U>
      struct DefaultXmlElementReader<TagT, boost::optional<U>, mpl::true_> {
        inline explicit DefaultXmlElementReader(
          XmlReaderFormat<TagT,mpl::true_> const& f
          ) : format_(f){}
        bool read(XmlReader<TagT> const& w, Any const& v) const {
          ElementId n(v.getDynInfo().getXmlInfo().getName());
          boost::optional<U>& vv = fromAny<boost::optional<U> >(v);
          bool save = w.setUnpos(v.getDynInfo().getXmlInfo().hasIndependentOrder());
          if (!format_.check(w,n)) {
            w.setUnpos(save);
            return false;
          }
          format_.begin(w,n);

          vv.reset(U());
          contentReader_.read(w,toAny(*vv));

          format_.end(w);
          w.setUnpos(save);
          return true;
        }
      private:
        XmlReaderFormat<TagT,mpl::true_> const& format_;
        XmlReadContent<TagT,U> contentReader_;
      };

      template<typename TagT,typename U,typename IsMultiConstrT,typename EnT>
      struct XmlElementReader:DefaultXmlElementReader<TagT,U,IsMultiConstrT>{
        inline XmlElementReader(XmlReaderFormat<TagT,IsMultiConstrT> const& f)
          : DefaultXmlElementReader<TagT,U,IsMultiConstrT>(f){}

      };

      template<typename TagT>
      struct DefaultSingleConstrRead {
        inline DefaultSingleConstrRead(
          AbsXmlReadContent<TagT> const& c,
          XmlReaderFormat<TagT,mpl::false_> const& f
          ):contentReaderRef_(c),format_(f){}

        bool read(XmlReader<TagT> const& w, Any const& v) const {
          AbsXmlInfo const& i = v.getDynInfo().getXmlInfo();
          ElementId e(i.getName());
          bool save = w.setUnpos(i.hasIndependentOrder());
          if(format_.check(w,e)) {
            format_.begin(w,e);
            contentReaderRef_.read(w,v);
            format_.end(w);
            w.setUnpos(save);
            return true;
          }
          w.setUnpos(save);
          return false;
        }
      private:
        AbsXmlReadContent<TagT> const& contentReaderRef_;
        XmlReaderFormat<TagT,mpl::false_> const& format_;
      };

      template<typename TagT,typename U>
      struct DefaultXmlElementReader<TagT,U,mpl::false_> 
        : DefaultSingleConstrRead<TagT> 
      {
        inline explicit DefaultXmlElementReader(
          XmlReaderFormat<TagT,mpl::false_> const& f
          ):DefaultSingleConstrRead<TagT>(contentReader_,f){}
      private:
        XmlReadContent<TagT,U> contentReader_;
      };


      template<typename TagT,typename U>
      struct DefaultXmlElementReader<TagT,boost::shared_ptr<U>,mpl::false_> {
        inline explicit DefaultXmlElementReader(
          XmlReaderFormat<TagT,mpl::false_> const& f
          ):format_(f){}
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          if (format_.null(w,v)) 
            return true;
          AbsXmlInfo const& i = v.getDynInfo().getXmlInfo();
          ElementId e(i.getName());
          if(format_.check(w,e)) {
            //TODO: only this needs to be moved to a virtual function 
            // for getting rid of U parameter
            fromAny<boost::shared_ptr<U> >(v).reset(new U); 
            bool save = w.setUnpos(i.hasIndependentOrder());
            format_.begin(w,e);
            // XmlReadContent<TagT,boost::shared_ptr<U> >::action(w,v);
            v.run<dyn_fun_t const>(&w);
            format_.end(w);
            w.setUnpos(save);
            return true;
          } else
            return false;
        }
      private:
        XmlReaderFormat<TagT,mpl::false_> const& format_;
      };

      //TODO: do this on SYB layer
      template<typename TagT>
      struct XmlElementReader<TagT,Unit,mpl::false_> {
        inline explicit XmlElementReader(
          XmlReaderFormat<TagT,mpl::false_> const&
          ){}
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          return true; 
        }
      };

      template<typename TagT>
      struct XmlElementReader<TagT,bool,mpl::false_> {
        inline explicit XmlElementReader(
          XmlReaderFormat<TagT,mpl::false_> const& f
          ):format_(f){}
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          ElementId e(v.getDynInfo().getXmlInfo().getName());
          if (w.impl().checkNextElement(e)) {
            w.impl().beginElement(e);
            w.impl().endElement();
            fromAny<bool>(v) = true;
          } else fromAny<bool>(v) = false;
          return true;
        }
      private:
        XmlReaderFormat<TagT,mpl::false_> const& format_;
      };

      /**
      \brief During parsing of Any type we don't know it's exact type, so its
      parsing is postponed

      This also holds whole document in memory so this may lead to a memory leak
      if the value won't be parsed for a long time
      */
      template<typename TagT>
      struct XmlElementReader<TagT,Any,mpl::false_> {
        inline explicit XmlElementReader(
          XmlReaderFormat<TagT,mpl::false_> const& f
          ):format_(f){}

        struct Initializer : AbsAnyInitializer {
          inline Initializer(
            boost::shared_ptr<AnyElementImpl> const& s,
            boost::shared_ptr<XmlReaderImpl> const& i,
            XmlReaderFormat<TagT,mpl::false_> const& f
            )
            :state_(s),impl_(i),format_(f){}
          void run(Any& v) const {
            XmlReader<TagT> r(boost::make_shared<priv::XmlReaderImpl>(state_));
            if (!format_.dispatchRead(r,v)) {
              v.getDynInfo().getDataType().setDefault(v);
            }
          }
          boost::shared_ptr<AnyElementImpl> const& state() { return state_; }

        private:
          boost::shared_ptr<AnyElementImpl> state_;
          // this is not used but keeps document open, probably it would be 
          // better to move this to state_
          boost::shared_ptr<XmlReaderImpl> impl_; 
          XmlReaderFormat<TagT,mpl::false_> const& format_;
          
        };

        bool read(XmlReader<TagT> const& w, Any const& v) const {
          boost::shared_ptr<AnyElementImpl> el = w.impl().getAnyElement();
          if (!el)
            return false;
          // TODO: probably it's possible somehow remove this double boxing
          fromAny<Any>(v).setInitializer(
            boost::make_shared<Initializer>(el,w.implPtr(),format_));
          w.impl().skipElement();
          return true;
        }
      private:
        XmlReaderFormat<TagT,mpl::false_> const& format_;
      };

      template<
        typename TagT, 
        typename U, 
        typename NillableT, 
        typename IsMultiConstrT
      > struct XmlCheckElementExists
        :XmlElementReader<TagT,U,IsMultiConstrT>
      {
        inline explicit XmlCheckElementExists(
          XmlReaderFormat<TagT,IsMultiConstrT> const& f
          ):XmlElementReader<TagT,U,IsMultiConstrT>(f){}
      };

      template<typename TagT, typename U, typename IsMultiConstrT>
      struct XmlCheckElementExists<TagT,U,mpl::false_,IsMultiConstrT> {
        inline explicit XmlCheckElementExists(
          XmlReaderFormat<TagT,IsMultiConstrT> const& f
          ):reader_(f){}

        bool read(XmlReader<TagT> const& r, Any const& v) const {
          AbsXmlInfo const& i = v.getDynInfo().getXmlInfo();
          if (!reader_.read(r,v)) {
            throw std::runtime_error(std::string("no such element ")  + i.getName().name());
          }
        }
      private:
        XmlElementReader<TagT,U,IsMultiConstrT> reader_;
      };

      template<typename T,typename TagT,typename U>
      struct XmlReaderDispatcher<T,TagT,U,typename boost::enable_if<
          boost::is_base_of<XmlElementTag,XmlKind<T,TagT> >
      >::type>:XmlCheckElementExists<TagT,U,
        typename Nullable<T>::type,
        typename IsMultiConstr<T>::type
      > {
        inline XmlReaderDispatcher()
          :XmlCheckElementExists<TagT,U,
            typename Nullable<T>::type,
            typename IsMultiConstr<T>::type
          >(XmlElReadFormat<
          TagT,
          typename SkipXmlElement<T,TagT>::type,
          typename IsMultiConstr<T>::type
        >::instance()){}
      };

      template<typename TagT,typename U, typename EnT=void>
      struct XmlAttributeReader {
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          QName n = v.getDynInfo().getXmlInfo().getName();
          std::string ret = w.impl().getAttribute(n);
          if (ret.empty())
            return false;
          XmlStringToPrim<U>
            ::action(fromAny<U>(v),w.impl().getAttribute(n));
          return true;
        }
      };

      template<typename TagT, typename U>
      struct XmlAttributeReader<TagT,boost::optional<U> 
        ,typename boost::enable_if<mpl::not_<IsUnicode<U> > >::type> {
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          boost::optional<U>& vv = fromAny<boost::optional<U> >(v);
          std::string ret 
            = w.impl().getAttribute(v.getDynInfo().getXmlInfo().getName());
          if (ret.empty()) 
            return false;
          vv.reset(U());
          try {
            XmlStringToPrim<U>::action(*vv,ret);
          } catch(const boost::bad_lexical_cast&) {
            std::string strType = typeid(U).name();
            qx::syb::QName attr = v.getDynInfo().getXmlInfo().getName();
            throw std::runtime_error( boost::str(boost::format("The value %1% of attribute %2% can not be converted to %3% type.")  % ret.c_str() % attr.name() % strType));
          }
          return true;
        }
      };

      template<typename TagT>
      struct XmlAttributeReader<TagT,bool,void> {
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          const std::string &name = w.impl().getAttribute(v.getDynInfo().getXmlInfo().getName());
          XmlStringToPrim<bool>::action(fromAny<bool>(v), name);
          return true;
        }
      };

      template<typename T,typename TagT,typename U>
      struct XmlReaderDispatcher<T,TagT,U,typename boost::enable_if<
        boost::is_base_of<XmlAttributeTag,XmlKind<T,TagT> >
      >::type>:XmlAttributeReader<TagT,U>{};

      template<typename T, typename TagT,typename U>
      struct XmlReaderDispatcher<T,TagT,U,typename boost::enable_if<
        boost::is_base_of<XmlTextTag,XmlKind<T,TagT> >
      >::type> {
        bool read(XmlReader<TagT> const& r, Any const& v) const {
          readContent_.read(r,v);
          return true;
        }
      private:
        XmlReadContent<TagT,U> readContent_;
      };

    }

    template<typename T, typename TagT>
    struct XmlReaderManip {

      inline explicit XmlReaderManip(T& v, bool isPreprocess)
        : val_(v), isPreprocess_(isPreprocess) {
      }

      void doRead(std::istream&, bool isPreprocess) const;

      friend inline std::istream& 
        operator>>(std::istream& f, syb::XmlReaderManip<T,TagT> const& t)
      {
        t.doRead(f, t.isPreprocess_);
        return f;
      }


    private:
      T& val_;
      bool isPreprocess_;
    };

    template<typename TagT, typename T>
    inline XmlReaderManip<T,TagT> fromXml(T& val, bool isPreprocess = false) {
      return XmlReaderManip<T,TagT>(val, isPreprocess);
    }

    template<typename T>
    inline XmlReaderManip<T,void> fromXml(T& val, bool isPreprocess = false) {
      return XmlReaderManip<T,void>(val, isPreprocess);
    }

    template<typename TagT, typename T>
    inline void fromXml(std::string const& xml, T& val, bool isPreprocess = false) {
      std::stringstream str(xml);
      str >> fromXml(val, isPreprocess);
    }

    template<typename T>
    inline void fromXml(std::string const& xml, T& val, bool isPreprocess = false) {
      fromXml<void>(xml, val, isPreprocess);
    }

    /**
    \brief Type trait for a primitive type check
    */
    /*template<typename T, typename EnableT=void> 
    struct HasXmlIndependentOrder:mpl::false_{};

    
    template<typename T, typename EnableT=void>
    struct AdditionalAttrs:NotDefined {
      static void action(priv::XmlReaderImpl& r) {
      }
    };*/
    
  }
}

namespace qx {
  namespace syb {
    template<typename AliasT,typename T, typename InstT, typename EnableT=void>
    struct NewTypeLazyLoad : public NewType<AliasT, T, EnableT> {

      inline NewTypeLazyLoad() : fileExtension_(".fm2") {}
      inline explicit NewTypeLazyLoad(T const& v) : NewType<AliasT, T, EnableT>(v), fileExtension_(".fm2", -1, US_INV) {}

      inline boost::shared_ptr<InstT> getInstance() {
        if(instance_)
          return instance_;

        instance_ = boost::make_shared<InstT>();
/*
        XString fileName(this->val().get());
        fileName.Clip();
        if(!fileName.EndsWith(fileExtension_))
          fileName += fileExtension_;
        UnicodeFStream fi(fileName.getIcuString(), std::ios_base::in);
        if(!fi.stream().is_open()) {
          XString dbPath = GetEnv("DBPATH") + "/" + fileName;
          UnicodeFStream dbfi(dbPath.getIcuString(), std::ios_base::in);
          if(!dbfi.stream().is_open())
            throw new Exception(QXE_FORM_FILE_NOT_FOUND, fileName);
          dbfi.stream() >> syb::fromXml(*instance_.get());
        } else {
          fi.stream() >> syb::fromXml(*instance_.get());
        }
*/
        std::string fileName(this->val().get());
        boost::trim(fileName);
        icu::UnicodeString ufileName(fileName.c_str(), fileName.length(), US_INV);
        //if(boost::ends_with(fileExtension_.))
        if (fName.endsWith(ufileName))
          ufileName += fileExtension_;
        UnicodeFStream fi(ufileName, std::ios_base::in);
        if (!fi.stream().is_open()) {
          icu::UnicodeString dbPath = icu::UnicodeString(getenv(), -1, US_INV) + uFileName;
          UnicodeFStream dbfi(dbPath, std::ios_base::in);
          if (!dbfi.stream().is_open()) {
            std::string error = std::string("file could not be found") + fileName;
            throw std::runtime_error(error);
          }
          dbfi.stream() >> syb::fromXml(*instance_.get());
        } else {
          fi.stream() >> syb::fromXml(*instance_.get());
        }
        return instance_;
      }

      inline void setInstance(const boost::shared_ptr<InstT>& val) {
        instance_ = val;
      }

      inline void setFileExtension(const icu::UnicodeString& fileExtension) {
        fileExtension_ = fileExtension;
      }

    private:
      boost::shared_ptr<InstT> instance_;
      //XString fileExtension_;
      icu::UnicodeString fileExtension_;
    };
  }
}

template<>
struct TraceName<qx::syb::priv::XmlReaderImpl> {
  inline static std::string const& value() {
    static std::string ret = "read xml:";
    return ret;
  }
};

#endif
