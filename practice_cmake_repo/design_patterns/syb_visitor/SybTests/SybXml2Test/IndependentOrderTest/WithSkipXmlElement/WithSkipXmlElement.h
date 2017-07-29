#ifndef WITH_SKIP_XML_ELEMENT_MODEL_H
#define WITH_SKIP_XML_ELEMENT_MODEL_H

#include "GeneratedModel/inc/GeneratedEnts.h"

namespace qx {
namespace syb {
  template<>
  struct SkipXmlElement<std::list<qx::with_skip_xml_element::FirstEntity> >:mpl::true_{};
  template<>
  struct SkipXmlElement<std::list<qx::with_skip_xml_element::SecondEntity> >:mpl::true_{};
}  // syb
}  // qx
#endif  // WITH_SKIP_XML_ELEMENT_MODEL_H
