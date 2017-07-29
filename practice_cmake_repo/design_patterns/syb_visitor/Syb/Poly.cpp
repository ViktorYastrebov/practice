#include "Poly.h" 
#include <boost/algorithm/string/case_conv.hpp>

namespace qx {
  namespace syb {

    AbsConstrInfo const* findByName(
        Loki::AssocVector<std::string,AbsConstrInfo const*> const &constrMap,
        std::string const& x) {
      typedef Loki::AssocVector<std::string, AbsConstrInfo const*>::const_iterator it;
      std::string tmp(x);
      boost::to_lower(tmp);
      it r = constrMap.find(tmp);
      return r == constrMap.end() ? NULL : r->second;
    }
  }
}
