/**
\brief Helper abstract class for setting and getting properties of 
any object.
*/

#ifndef ABSTRACT_ACCESSORS_H
#define ABSTRACT_ACCESSORS_H

#include <boost/any.hpp>

namespace qx {

  struct Accessor {
    virtual boost::any get(boost::any const& obj) const = 0;
    virtual void set(boost::any const& obj, boost::any const& val) const = 0;
  };

  template<typename ObjT, typename PropT>
  struct ConcrAccessor : Accessor {
  };
}

#endif
