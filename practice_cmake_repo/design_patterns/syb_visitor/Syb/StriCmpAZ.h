#ifndef STRCMP_AZ_H
#define STRCMP_AZ_H

inline int stricmpAZ(const char * src, const char * dst) {
  int ret = 0 ;
  while(!(ret = (*(unsigned char *)src & 0xDF) - (*(unsigned char *)dst & 0xDF)) && *dst)
    ++src, ++dst;
  return ret;
}

#endif  // STRCMP_AZ_H
