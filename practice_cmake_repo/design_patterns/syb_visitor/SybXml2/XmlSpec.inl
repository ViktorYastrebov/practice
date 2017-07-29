/**
  \brief A file to include once in one and only .cpp for each shared library 
  or executable (not to static library!)

  Each top level type for which read/write is performed should be registered
  with INST_XML_READER and/or INST_XML_WRITER
*/
#include "XmlReader.h"
#include "XmlWriter.h"

/// \brief Registers a top level type for each XML deserialization is used
#define INST_XML_READER(n) namespace qx {         \
  namespace syb {                                 \
    template struct XmlReaderManip<n,void>;       \
  }                                               \
}

/// \brief Registers a top level type for each XML serialization is used
#define INST_XML_WRITER(n) namespace qx {         \
  namespace syb {                                 \
    template struct XmlWriterManip<n,void>;       \
  }                                               \
}

namespace qx {

  namespace syb {

    template<typename T,typename TagT>
    void XmlWriterManip<T,TagT>::doWrite(std::ostream& f, bool isIndent, bool isLowerCase) const {
      XmlWriter<TagT> writer;
      Any v = toAny(val_);
      const AbsXmlInfo &xmlInfo = v.getDynInfo().getXmlInfo();
      xmlInfo.write(writer,v);
      writer.impl().setRootNamespace(xmlInfo.getName().ns());
      writer.impl().flush(f, isIndent, isLowerCase);
    }

    template<typename T, typename TagT>
    void XmlReaderManip<T,TagT>::doRead(std::istream& f, bool isPreprocess) const {
      XmlReader<TagT> reader(f, isPreprocess);
      Any v = toAny(val_);
      v.getDynInfo().getXmlInfo().read(reader,v);
    }
  }
}
