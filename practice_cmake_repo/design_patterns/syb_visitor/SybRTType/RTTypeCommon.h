#ifndef RTTYPE_COMMON_H
#define RTTYPE_COMMON_H

class RTType;

#include <string>

inline void checkBool(bool b, const std::string& mes) {
  if (!b) throw std::runtime_error(mes);
}

#endif /*RTTYPE_COMMON_H*/
