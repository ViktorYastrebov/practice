#ifndef SYB_UNICODE_STRING_H
#define SYB_UNICODE_STRING_H

#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include <functional>
#include <iostream>
#include <fstream>

namespace qx {
  namespace syb {
    struct UnicodeFStream {
      UnicodeFStream(const icu::UnicodeString& filename,
                     const std::ios_base::openmode& mode);
      ~UnicodeFStream();
      std::fstream& stream();
    private:
      std::fstream fs;
    };

    // icuStrToWchar - convert UnicodeString to wchar_t string
    // return: amount of converted symbols and negative if error
    int icuStrToWchar(const UnicodeString& src, wchar_t* dest, int32_t destLen);
  }
}

#endif
