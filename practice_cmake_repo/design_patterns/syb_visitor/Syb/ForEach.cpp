#include "ForEach.h"

namespace qx {
  namespace syb {
    
    void ForEach::action(Any const& v) const {
      fun_->action(v);
      v.run<dyn_fun_t const>(this);
    }
  }
}
