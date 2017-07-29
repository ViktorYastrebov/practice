#pragma once

#include <boost/serialization/access.hpp>
#include <string>

struct simple_data {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & a;
    ar & b;
    ar & c;
    ar & d;
    ar & e;
  }
public:
  static const int length = 32;
  int a;
  float b;
  double c;
  char d[length];
  std::string e;
};