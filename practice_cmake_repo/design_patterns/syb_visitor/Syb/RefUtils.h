/**
\brief Missed from boost reference cleaning function

\a cleanRef returns unwraps reference if it's reference wrapper or just
return input back otherwise

*/
#ifndef SYB_REF_H
#define SYB_REF_H

#include <boost/ref.hpp>

namespace qx {
  template<typename T>
  inline T& cleanRef(boost::reference_wrapper<T> const& r) {
    return r.get();
  }

  template<typename T>
  inline T& cleanRef(T& v) { return v; }
}

#endif
