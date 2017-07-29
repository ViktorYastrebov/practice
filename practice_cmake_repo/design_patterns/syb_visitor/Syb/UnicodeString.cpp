#include "UnicodeString.h"

#include <unicode/ustring.h>

namespace qx {
  namespace syb {
    int icuStrToWchar(const UnicodeString& src, wchar_t* dest, int32_t destLen) {
      int32_t reqLen = 0 ;
      const UChar* uSrc = src.getBuffer();
      const int32_t uSrcLen = src.length();
      UErrorCode uErr = U_ZERO_ERROR;
      u_strToWCS(dest, destLen, &reqLen, uSrc, uSrcLen, &uErr);
      if(uErr == U_BUFFER_OVERFLOW_ERROR){
          return -1;
      }
      return reqLen;
    }

    UnicodeFStream::UnicodeFStream(
      const icu::UnicodeString& filename, 
      const std::ios_base::openmode& mode) 
    {
#ifdef WINNT
      const int32_t wcPathLen = 10000;
      wchar_t wcPath[wcPathLen];
      icuStrToWchar(filename, wcPath, wcPathLen);
      fs.open(wcPath, mode);
#else
      std::string str;
      filename.toUTF8String(str);
      fs.open(str.c_str(), mode);
#endif
    }

    UnicodeFStream::~UnicodeFStream() {
      fs.close();
    }

    std::fstream& UnicodeFStream::stream() {
      return fs;
    }

  }
}

