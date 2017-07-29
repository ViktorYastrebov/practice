#ifndef SYB_BINARY_H
#define SYB_BINARY_H

#include "Syb.h"
#include <vector>

typedef std::vector<unsigned char> Binary;

inline Binary operator+(const Binary& lhs, const Binary& rhs) {
  Binary result(lhs);
  result.insert(result.end(), rhs.begin(), rhs.end()); 
  return result;
}

inline Binary& operator+=(Binary& lhs, const Binary& rhs) {
  lhs.insert(lhs.end(), rhs.begin(), rhs.end());
  return lhs;
}

class BinaryData {
public:  
  inline const Binary& binary() const {
    return binary_;
  }
    
  inline Binary& binary() {
    return binary_;
  }

  inline BinaryData& binary(const Binary& v) { 
    binary_ = v;
    return *this;
  }
private:
  Binary binary_;
  TRAVERSABLE_BEGIN(BinaryData) 
      TRAVERSABLE_FIELD(binary_)
  TRAVERSABLE_END
};

#endif  // SYB_BINARY_H
