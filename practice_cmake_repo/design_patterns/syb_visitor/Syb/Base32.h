#ifndef SYB_BASE32_H
#define SYB_BASE32_H

/*!
  Generic Base32 encoding/decoding implementation for which:
  I/Can be confused with 1/ replaced with W
  L/Can be confused with 1/ replaced with L
  O/Can be confused with 0/ replaced with Y
  U/Accidental obscenity/   replaced with Z
*/

namespace qx {
namespace syb {

  static const unsigned char base32EncodeTable[] =
  "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

  static const unsigned char base32DecodeTable[] =
  {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
    -1,10,11,12,13,14,15,16,17,-1,18,19,-1,20,21,-1,
    22,23,24,25,26,-1,27,28,29,30,31,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  };

  template<typename Base32, typename Source>
  Base32 toBase32(const Source& source) {

    Base32 base32;
    base32.resize(((source.size() * 8 + 4) / 5));
    typename Source::value_type word;
    typename Source::size_type i = 0, index = 0;

    typename Source::const_iterator sourceIterator = source.begin();
    typename Base32::iterator base32Iterator = base32.begin();

    while(sourceIterator != source.end()) {
      if(index > 3) {
        word = (*sourceIterator & (0xFF >> index));
        index = (index + 5) % 8;
        word <<= index;
        if (++sourceIterator != source.end())
          word |= *sourceIterator >> (8 - index);
      } else {
        word = (*sourceIterator >> (8 - (index + 5))) & 0x1F;
        index = (index + 5) % 8;
        if(!index)
          ++sourceIterator;
      }
      *base32Iterator = base32EncodeTable[word];
      ++base32Iterator;
    }
    return base32;
  }

  template<typename Sink, typename Base32>
  Sink fromBase32(const Base32& base32) {

    Sink sink;
    sink.resize((base32.size() * 5) / 8);
    typename Base32::value_type word;
    typename Base32::size_type index = 0;

    typename Base32::const_iterator base32Iterator = base32.begin();
    typename Sink::iterator sinkIterator = sink.begin();

    while(base32Iterator != base32.end()) {
      if((word = base32DecodeTable[*base32Iterator++]) != -1) {
        if (index <= 3) {
          index = (index + 5) % 8;
          if (index == 0) {
             *sinkIterator |= word;
             ++sinkIterator;
          } else if(sinkIterator != sink.end())
             *sinkIterator |= word << (8 - index);
        } else {
          index = (index + 5) % 8;
          *sinkIterator |= (word >> index);
          if(++sinkIterator != sink.end())
            *sinkIterator |= word << (8 - index);
        }
      }
    }
    return sink;
  }

}  // syb
}  // qx

#endif  // SYB_BASE32_H
