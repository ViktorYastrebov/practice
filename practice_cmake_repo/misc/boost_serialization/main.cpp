#include <iostream>
#include <fstream>

#include <sstream>

#include <string>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "portable_binary_iarchive.hpp"
#include "portable_binary_oarchive.hpp"

#include "boost_exports.hpp"
#include "single_class.h"
#include "derived.h"


struct data_test {
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

void single_class() {
  std::stringstream ss;
  {
    simple_data data;
    data.a = 1;
    data.b = 3.14f;
    data.c = 9.000001;
    std::strcpy(data.d, "some_buffer");
    data.e = std::string("string_literal");

    boost::archive::binary_oarchive bin_oarch(ss);
    bin_oarch & data;
  }

  {
    simple_data data;
    boost::archive::binary_iarchive bin_iarch(ss);
    bin_iarch & data;
    std::cout << data.a << " " << data.b << " " << data.c << " " << data.d << " " << data.e << std::endl;
  }
}

BOOST_SERIALIZATION_SHARED_PTR(simple_data);

void single_class_shared_ptr() {
  std::stringstream ss;
  {
    std::shared_ptr<simple_data> sd = std::make_shared<simple_data>();
    sd->a = 1;
    sd->b = 3.14f;
    sd->c = 9.000001;
    std::strcpy(sd->d, "some_buffer");
    sd->e = std::string("string_literal");

    boost::archive::binary_oarchive bin_oarch(ss);
    bin_oarch & sd;
  }

  {
    std::shared_ptr<simple_data> sd = std::make_shared<simple_data>();
    boost::archive::binary_iarchive bin_iarch(ss);
    bin_iarch & sd;
    std::cout << sd->a << " " << sd->b << " " << sd->c << " " << sd->d << " " << sd->e << std::endl;
  }
}

void polymorthic_shared() {
  //http://www.boost.org/doc/libs/1_32_0/libs/serialization/doc/special.html

  std::stringstream ss;
  {
    std::shared_ptr<base_class_id> base_id = std::make_shared<derived1>("string_value");
    boost::archive::binary_oarchive bin_oarch(ss);
    bin_oarch & base_id;
  }

  {
    std::shared_ptr<base_class_id> base_id = std::make_shared<derived1>();
    boost::archive::binary_iarchive bin_iarch(ss);
    bin_iarch & base_id;
    std::cout << "id = " << base_id->get_id() << std::endl;
    std::cout << std::static_pointer_cast<derived1>(base_id)->get() << std::endl;
  }

}

int main(int argc, char *argv[]) {

  single_class();
  single_class_shared_ptr();
  polymorthic_shared();
  return 0;
}