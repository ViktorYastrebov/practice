/// The file contains hooks for shorter diff/patch XML format
/// if you includes the file you will also need to include 
/// XmlDiffFormat.inl to some .cpp file (can be .lib unlike
/// other *.inl files)
#ifndef SYBXML2_XML_DIFF_FORMAT_H
#define SYBXML2_XML_DIFF_FORMAT_H

#include "SybXml2/XmlDiffPatch.h"

namespace qx {
  namespace syb {
    namespace priv {
      bool doReadDiffItem(XmlReader<void> const& w, boost::shared_ptr<DiffItem>& vv);

      template<typename TagT>
      struct XmlElementReader<TagT,boost::shared_ptr<DiffItem> >
      {
        inline explicit XmlElementReader(
          XmlReaderFormat<TagT,mpl::true_> const&
          ){}
        bool read(XmlReader<TagT> const& w, Any const& v) const {
          boost::shared_ptr<DiffItem>& vv 
            = fromAny<boost::shared_ptr<DiffItem> >(v);
          return doReadDiffItem(w,vv);
        }
      };

      bool doWriteDiffItem(XmlWriter<void> const& w, boost::shared_ptr<DiffItem> const& vv);

      template<typename TagT>
      struct XmlElementWriter<TagT,boost::shared_ptr<DiffItem> > 
      {
        inline explicit XmlElementWriter(XmlWriteFormat<TagT> const& f){}

        bool write(XmlWriter<TagT> const& w, Any const& v) const {
          boost::shared_ptr<DiffItem> const& vv 
            = fromAny<boost::shared_ptr<DiffItem> const>(v);
          return doWriteDiffItem(w,vv);

        }
      };
    }
  }
}

#endif
