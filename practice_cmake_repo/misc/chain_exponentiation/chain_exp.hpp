#ifndef __CHAIN_EXPONENTIATION__
#define __CHAIN_EXPONENTIATION__

//https://comeoncodeon.wordpress.com/tag/knuth/

template<class T>
T chainExponet(int powerExp, const T  &value, int &steps) {

  if (powerExp == 1) {
    return value;
  }

  ++steps;
  if (powerExp % 2 == 0)
  {
    T calulated = chainExponet(powerExp / 2, value, steps);
    return calulated * calulated;
  } else {
    return value * chainExponet(powerExp - 1, value, steps);
  }
}

#endif