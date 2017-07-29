#include "base_class.h"
#include "derived.h"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/serialization.hpp>

BOOST_SERIALIZATION_SHARED_PTR(base_class)
BOOST_SERIALIZATION_SHARED_PTR(base_class_id)
BOOST_SERIALIZATION_SHARED_PTR(derived1)

BOOST_SERIALIZATION_ASSUME_ABSTRACT(base_class)
BOOST_SERIALIZATION_ASSUME_ABSTRACT(base_class_id)
BOOST_CLASS_EXPORT(base_class_id)
BOOST_CLASS_EXPORT(derived1)