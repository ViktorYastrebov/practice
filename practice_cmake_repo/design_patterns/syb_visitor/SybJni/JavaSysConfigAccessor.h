#ifndef JAVASYSCONFIGACCESSOR_H
#define JAVASYSCONFIGACCESSOR_H

#include <string>
#include "JniCommon.h"

namespace qx {
  class JavaSysConfigAccessor {   
   
  public:
    static bool getProperty(JNIEnv *je, const std::string propName, std::string& res);
  };
}

#endif /*JAVASYSCONFIGACCESSOR_H*/

