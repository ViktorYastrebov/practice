#define ATTRIBUTES_LOADING

#include <list>
#include <fstream>
#include <algorithm>
#include <cctype>

#include "SybXml2/XmlDiffFormat.h"
#include "Syb/SybAll.h"
#include "Syb/StriCmpAZ.h"
#include "Syb/GenericDiff.h"
#include "Syb/GenericPatch.h"

using namespace qx::syb::diff;

namespace qx {
  namespace syb {
    namespace priv {
    #ifdef ATTRIBUTES_LOADING
      struct InitFromString : AbsAnyInitializer {
        inline explicit InitFromString(UnicodeString const& v)
          :val_(v){}
        void run(Any& v) const {
          v.getDynInfo().getDataType().fromString(v,val_);
        }
      private:
        UnicodeString val_;
      };
#endif
      bool doReadDiffItem(XmlReader<void> const& w, boost::shared_ptr<DiffItem>& vv) {
        std::string merge = w.impl().getAttribute(QName("m"));
        if (merge.empty()) {
          boost::shared_ptr<Update> ret = boost::make_shared<Update>();
          vv = ret;
          w.action(toAny(ret->val()));
          return true;
        }
        if (merge == "a") {
          boost::shared_ptr<MergeArray> ret = boost::make_shared<MergeArray>();
          vv = ret;
          std::string sza = w.impl().getAttribute(QName("s"));
          if (!sza.empty())
            ret->size(boost::lexical_cast<unsigned int>(sza));
          while(w.impl().hasNext()) {
            w.impl().beginElement();
            MergeArray::Item i;
            std::string xa = w.impl().getAttribute(QName("x"));
            if (!xa.empty())
              i.index(boost::lexical_cast<unsigned int>(xa));
            doReadDiffItem(w,i.diff());
            ret->items().push_back(i);
            w.impl().endElement();
          }
          return true;
        }
        if (merge == "r") {
          boost::shared_ptr<MergeRecord> ret = boost::make_shared<MergeRecord>();
          vv = ret;
#ifdef ATTRIBUTES_LOADING
          typedef std::list<std::pair<std::string,UnicodeString> > attrs_list_t;
          attrs_list_t attrs;
          typedef attrs_list_t::const_iterator attrs_iter_t;
          w.impl().loadAttributes(attrs);
          for(attrs_iter_t i = attrs.begin(), e = attrs.end(); i != e; ++i) {
            std::string n = i->first;
            if (n == "m" || n == "Oid" || n == "x")
              continue;
            MergeRecord::Field fi;
            boost::shared_ptr<Update> upd = boost::make_shared<Update>();
            upd->val().setInitializer(boost::make_shared<InitFromString>(i->second));
            fi.diff(upd);
            fi.name(n);
            ret->fields().push_back(fi);
          }
#endif
          while(w.impl().hasNext()) {
            w.impl().beginElement();
            MergeRecord::Field fi;
            doReadDiffItem(w,fi.diff());
            fi.name(w.impl().currentElementLocalName());
            ret->fields().push_back(fi);
            w.impl().endElement();
          }
          return true;
        }
        return false;
      }

      bool doWriteDiffItem(XmlWriter<void> const& w, boost::shared_ptr<DiffItem> const& vv) {
        //TODO: make it OOP
        boost::shared_ptr<Update> upd = boost::dynamic_pointer_cast<Update>(vv);
          // we will add attribute merge on merging and set on setting, the rest will be
          // the same, and headless
        if (upd) {
          if (!upd->val().getDynInfo().getXmlInfo().writeUnfolded(w,upd->val())) {
            w.impl().setAttribute(getNilAttrName(),std::string("yes"));
          }
          return true;
        }
        boost::shared_ptr<MergeRecord> mr = boost::dynamic_pointer_cast<MergeRecord>(vv);
        if (mr) {
          w.impl().setAttribute(QName("m"),UNICODE_STRING_SIMPLE("r"));
          std::list<MergeRecord::Field>::const_iterator i = mr->fields().begin(), e = mr->fields().end();
          for (; i!= e; ++i) {
            upd = boost::dynamic_pointer_cast<Update>(i->diff());
            if (upd && upd->val().getDynInfo().getDataType().hasStringValue()) {
              UnicodeString s;
              upd->val().getDynInfo().getDataType().toString(upd->val(),s);
              w.impl().setAttribute(QName(i->name()), s);
              continue;
            }
            w.impl().beginElement(ElementId(i->name().c_str()));
            if (!doWriteDiffItem(w,i->diff()))
              return false;
            w.impl().endElement();
          }
          return true;
        }
        boost::shared_ptr<MergeArray> ma = boost::dynamic_pointer_cast<MergeArray>(vv);
        if (ma) {
          std::list<MergeArray::Item>::const_iterator i = ma->items().begin(), e = ma->items().end();
          w.impl().setAttribute(QName("m"),UNICODE_STRING_SIMPLE("a"));
          if (ma->size())
            w.impl().setAttribute(QName("s"),ma->size() 
            ? boost::lexical_cast<std::string>(*ma->size()) : std::string("")
            );
          for(;i!=e;++i) {
            w.impl().beginElement(ElementId("i"));
            if (i->index())
              w.impl().setAttribute(QName("x"),boost::lexical_cast<std::string>(*i->index()));
            if (!doWriteDiffItem(w,i->diff()))
              return false;
            w.impl().endElement();
          }
          return true;
        }
        return false;
      }
    }
  }
}
