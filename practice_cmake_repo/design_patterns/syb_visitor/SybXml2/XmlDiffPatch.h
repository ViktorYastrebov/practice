/**
\brief Implementation of XML based diff/patch functions.

See Syb/GenericDiff.h and Syb/GenericPatch.h for more details.

The module provides XML diff scripts reading and writing facilities.

Usage:
  
  MyObj obj1;
  MyObj obj2;

  ... init objs 

  std::stringstream f;
  
  f << syb::xmlDiff(obj1,obj2);
  f >> syb::xmlPatch(obj2);

After this obj2 will be equal to obj1.

For embedding a diff script to some other object, the way it appear in
serialization it's possible to use DiffHoder template.
It's mainly needed for eager diff script application. I.e. the script 
will be applied during its message parsing just on parsing an element
this the script. And so running some side effects which are run only 
during application. For example if the script contains some new objects
which are refenced from some other part of the message the object won't
be added to object's dictionary an so they won't be found during reference's
parsing.

*/
#ifndef SYB_XML2_XML_DIFF_PATCH
#define SYB_XML2_XML_DIFF_PATCH

#include "Syb/GenericDiff.h"
#include "Syb/GenericPatch.h"
#include "Syb/Clone.h"
#include "Syb/Any.h"
#include "SybXml2/XmlReader.h"
#include "SybXml2/XmlWriter.h"
#include "SybXml2/XmlInfoSpec.h"

namespace qx {
  namespace syb {

    template<typename T>
    struct XmlDiffManip {
      inline XmlDiffManip(T const& s, T const& d):src_(s),dst_(d){}
      friend inline std::ostream& operator<<(
        std::ostream& f
        ,XmlDiffManip const& This)
      {
        return f << toXml<void>(gDiff(This.src_,This.dst_));
      }
    private:
      T const& src_;
      T const& dst_;
    };

    template<typename T>
    XmlDiffManip<T> xmlDiff(T const& s, T const& d) {
      return XmlDiffManip<T>(s,d);
    }

    template<>
    struct SkipXmlElement<diff::Update::Val>:mpl::true_{};

    template<typename T>
    struct XmlPatchManip {
      inline explicit XmlPatchManip(T& d):dst_(d){}
      friend inline std::istream& operator>>(
        std::istream& f
        ,XmlPatchManip const& This)
      {
        DiffScript scr;
        f >> fromXml<void>(scr);
        gPatch(unAlias(This.dst_),unAlias(scr));
        return f;
      }
    private:
      T& dst_;
    };

    template<typename T>
    XmlPatchManip<T> xmlPatch(T& d) {
      return XmlPatchManip<T>(d);
    }

    struct DiffHolderTag{};

    template<typename Tt>
    struct DiffHolder : DiffHolderTag {
      inline DiffHolder(
        boost::shared_ptr<Tt> const& s, 
        boost::shared_ptr<Tt> const& d
        ):src_(s),dst_(d){}
      inline boost::shared_ptr<Tt> const& src() const { return src_; }
      inline boost::shared_ptr<Tt> const& dst() const { return dst_; }
      inline boost::shared_ptr<Tt>& src() { return src_; }
      inline boost::shared_ptr<Tt>& dst() { return dst_; }

    private:
      boost::shared_ptr<Tt> src_;
      boost::shared_ptr<Tt> dst_;

      TRAVERSABLE_BEGIN(DiffHolder)
      TRAVERSABLE_END
    };

    namespace priv {

      template<typename TagT, typename U>
      struct XmlElementWriter<TagT,U,
        typename boost::enable_if<boost::is_base_of<DiffHolderTag,U> >::type> 
      {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const&){}
        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          U const& vv = fromAny<U const>(v);
          boost::shared_ptr<DiffItem> res = gDiff(vv.src(),vv.dst());
          Any script = toAny(res);
          script.getDynInfo().getXmlInfo().write(w,script);
          // clone<HistoryTag>(unAlias(v).dst(),unAlias(v).src());
          return true;
        }
      };

      template<typename TagT,typename U>
      struct XmlElementReader<TagT,U,mpl::false_,typename boost::enable_if<
        boost::is_base_of<DiffHolderTag,U>
      >::type>
      {
        inline explicit XmlElementReader(
          XmlReaderFormat<TagT,mpl::false_> const&
          ){}
        bool read(XmlReader<TagT> const& w, Any const& v) const {
          DiffScript scr;
          xmlInfo<DiffScript>().read(w,toAny(scr));
          gPatch(fromAny<U>(v).dst(),scr);
          return true;
        }
      };
    }

    template<>
    struct SkipXmlElement<std::list<diff::MergeRecord::Field> >:mpl::true_{};
    
    template<>
    struct SkipXmlElement<std::list<diff::MergeArray::Item> >:mpl::true_{};
  }
}

#endif
