#include <stdexcept>
#include <string.h>

#include "common.h"

std::string getLastError() {
  char *errMsg = strerror(errno);
  return std::string(static_cast<const char *>(errMsg));
}
