/**
\brief Special treatment for objects which may be referenced
by platform independent identity.

Only implementation's details are here.

Limitation:

\todo Only boost::shared_ptr and boost::weak_ptr are supported at the moment

\todo During actual object's writing and diff script creation different sets
are used to detect object's duplicate, this can lead to the same object is
serialized twice (MergeRecord and as actual object), this affects only size
of resulting XML. But it's possible to use just one set for both to avoid
this, or use weak references (boost::weak_ptr on C++ and WeakReference
annotation on C#).

\todo If weak reference appears earlier in the document it won't be parsed
since the object is not in the dictionary, some lazy lookup can be implemented
to avoid the problem
*/
#ifndef SYB_XML2_REF
#define SYB_XML2_REF

#if 0
#include "SybXml2/XmlReader.h"
#include "SybXml2/XmlWriter.h"
#include "Syb/ObjectIdentity.h"
#include "Syb/IToStr.h"
#else
#include "XmlReader.h"
#include "XmlWriter.h"
#include "../Syb/ObjectIdentity.h"
#include "../Syb/IToStr.h"
#endif

namespace qx {
  namespace syb {

    struct XmlRefProcessing {
      XmlRefProcessing() : enabled_(false) {
      }
      bool isEnabled() const {
        return enabled_;
      }
      void enable() {
        enabled_ = true;
      }
      void disable() {
        enabled_ = false;
      }
      static XmlRefProcessing& instance() {
        static XmlRefProcessing ret;
        return ret;
      }
    private:
      bool enabled_;
    };

    namespace priv {

      template<typename TagT,typename U, typename IsMultiConstrT>
      struct XmlElementReader<TagT,boost::shared_ptr<U>,IsMultiConstrT
        ,typename boost::enable_if<HasId<U> >::type
      >
      {
        inline explicit XmlElementReader(
          XmlReaderFormat<TagT,IsMultiConstrT> const& f
          ):format_(f){}
        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          AbsXmlInfo const& i = v.getDynInfo().getXmlInfo();
          if (format_.null(w,v))
            return true;
          boost::optional<std::pair<
            ElementId,
            AbsConstrInfo const*
          > > e = format_.constr(w, v);
          boost::shared_ptr<U>& vv = fromAny<boost::shared_ptr<U> >(v);
          ElementId b(i.getName());
          if (!e || e->first == b) {
            if (!e && !format_.check(w,b) /* && refs.empty() */)
              return false;
            std::string refs = format_.getAttribute(w,getRefAttrName());
            format_.begin(w,b);
            if (!refs.empty()) {
              cfg::key_t k = boost::lexical_cast<cfg::key_t>(refs);
              boost::shared_ptr<WithId> obj = ObjDir::instance().lookup(k);
              if (!obj)
                throw std::runtime_error("no such object: " + refs);
              vv = boost::dynamic_pointer_cast<U>(obj);
              if (!vv)
                throw std::runtime_error("wrong type for object: " + refs);
              format_.end(w);
              return true;
            }
            if (!e)
              throw std::runtime_error("wrong object's encoding");
          } else {
              format_.begin(w,e->first);
          }
          std::string ids = w.impl().getAttribute(getIdAttrName());
          if (ids.empty())
            throw std::runtime_error("object without identity");
          e->second->dynBuild(v);
          vv->id(boost::lexical_cast<cfg::key_t>(ids));
          v.run<dyn_fun_t const>(&w);
          ObjDir::instance().add(vv);
          format_.end(w);
          return true;
        }
      private:
        XmlReaderFormat<TagT,IsMultiConstrT> const& format_;
      };

      template<typename TagT,typename U,typename IsMultiConstrT>
      struct XmlElementReader<TagT,boost::weak_ptr<U>,IsMultiConstrT
        ,typename boost::enable_if<HasId<U> >::type
      >
      {
        inline explicit XmlElementReader(
          XmlReaderFormat<TagT,IsMultiConstrT> const& f
          ):format_(f){}

        inline bool read(XmlReader<TagT> const& w, Any const& v) const {
          AbsXmlInfo const& i = v.getDynInfo().getXmlInfo();
          if (format_.null(w,v))
            return true;
          ElementId e(i.getName());
          if (!format_.check(w,e))
            return false;
          boost::weak_ptr<U>& vv = fromAny<boost::weak_ptr<U> >(v);
          format_.begin(w,e);
          std::string refs = w.impl().getAttribute(getRefAttrName());
          if (refs.empty())
            throw std::runtime_error("weak pointer passed by value");
          cfg::key_t k = boost::lexical_cast<cfg::key_t>(refs);
          boost::shared_ptr<WithId> obj = ObjDir::instance().lookup(k);
          if (!obj)
            throw std::runtime_error("no such object: " + refs);
          boost::shared_ptr<U> tobj = boost::dynamic_pointer_cast<U>(obj);
          if (!tobj)
            throw std::runtime_error("wrong type for object: " + refs);
          vv = tobj;
          format_.end(w);
          return true;
        }
      private:
        XmlReaderFormat<TagT,IsMultiConstrT> const& format_;
      };

      template<typename TagT,typename U>
      struct XmlElementWriter<TagT,boost::shared_ptr<U>,
        typename boost::enable_if<HasId<U> >::type
      >
      {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f)
          :format_(f){}
        inline bool write(XmlWriter<TagT> const& w, Any const& v) const {
          typedef typename IsMultiConstr<boost::shared_ptr<U> >::type mc_t;
          if(!format_.id(w,v,mc_t()))
            return false;
          boost::shared_ptr<U> const& vv
            = fromAny<boost::shared_ptr<U> const>(v);
          unsigned int id = vv->id();
          ElementId e(v.getDynInfo().getXmlInfo().getName(),
                      getRefAttrName(),
                      itostr(id));
          format_.begin(w,e);
          if (XmlRefProcessing::instance().isEnabled() && !w.impl().isComputed(vv->id())) {
            w.impl().setComputed(vv->id());
            w.impl().setAttribute(getIdAttrName(), itostr(static_cast<unsigned int>(vv->id())));
            v.run<dyn_fun_t const>(&w);
          }
          format_.end(w);
          return true;
        }
      private:
        XmlWriteFormat<TagT> const& format_;
      };

      template<typename TagT,typename U>
      struct XmlElementWriter<TagT,boost::weak_ptr<U>
        ,typename boost::enable_if<HasId<U> >::type
      >
      {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f)
          :format_(f){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          boost::shared_ptr<U> lv
            = fromAny<boost::weak_ptr<U> const>(v).lock();
          if (!lv)
            return false;
          unsigned int id = lv->id();
          format_.begin(w,ElementId(v.getDynInfo().getXmlInfo().getName()));
          w.impl().setAttribute(getRefAttrName(), itostr(id));
          format_.end(w);
          return true;
        }
      private:
        XmlWriteFormat<TagT> const& format_;
      };
    }
  }
}

#endif
