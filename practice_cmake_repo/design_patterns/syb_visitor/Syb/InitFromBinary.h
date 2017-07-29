#ifndef MSG_INITFROMBINARY_H
#define MSG_INITFROMBINARY_H

#include "Syb/Any.h"
#include "Syb/ObjectIdentity.h"

namespace qx {
  namespace msg {

    void read(std::istream &input, std::string &value);

    struct InitFromBinary : qx::syb::AbsAnyInitializer {
      inline explicit InitFromBinary(const std::string &val) : val_(val) {
      }
      inline explicit InitFromBinary(std::istream *input) {
        assert(input);
        read(*input, val_);
      }
      void run(qx::syb::Any& v) const {
        v.getDynInfo().getDataType().fromBinary(v, val_);
      }
      std::string buf() {
        return val_;
      }
    private:
      std::string val_;
    };
  }
}

#endif
