#include "Generate.h"

#include <stdio.h>
#include <stdlib.h>

#include <boost/nondet_random.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

//TODO: redo via boost::random

namespace qx {
  namespace syb {
    unsigned int genUInt(unsigned int min, unsigned int max) {
      return min + static_cast<unsigned int>(rand()) % (max - min + 1);
    }
    bool genBool() { 
      //better than assigning int due to absence of cast warning errors.; 
      return (rand() % 100) > 50;
    }

    std::string guid() {
      boost::uuids::basic_random_generator<boost::random_device> gen;
      return boost::uuids::to_string(gen());
    }

    //TODO: move it to some other cpp
    HierarchyItemTag::~HierarchyItemTag() {}
  }
}
