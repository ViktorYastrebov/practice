#include "XmlTypes.h"

namespace qx {
  namespace syb {
    QName const& getIdAttrName() {
      static QName ret("Id");
      return ret;
    }

    QName const& getIndexAttrName() {
      static QName ret("index");
      return ret;
    }

    QName const& getSizeAttrName() {
      static QName ret("size");
      return ret;
    }

    QName const& getRefAttrName() {
      static QName ret("ref");
      return ret;
    }

    QName const& getNilAttrName() {
      static QName ret("nil");
      return ret;
    }

    QName const& getArrayResizeElement() {
      static QName ret("resize", defaultXmlNs());
      return ret;
    }

    QName const& getArrayItemElement() {
      static QName ret("item", defaultXmlNs());
      return ret;
    }

    QName const& TypeAttributeName::value() {
      static QName ret("type"/*,
        "http://www.w3.org/2001/XMLSchema-instance",
        "xsi"*/);
      return ret;
    }

  }
}
