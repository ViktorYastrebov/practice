#include <boost/test/unit_test.hpp>
#include "test_suite.h"

using namespace boost::unit_test;

test_suite * init_unit_test_suite(int argc, char * argv[] ) {

  test_suite * test1 = BOOST_TEST_SUITE("Base tests"); {
    test1->add(BOOST_TEST_CASE(&test_def_constructor));
    test1->add(BOOST_TEST_CASE(&test_copy_constructor));
    test1->add(BOOST_TEST_CASE(&test_assign));
  }

  test_suite * test2 = BOOST_TEST_SUITE("Matrix operations"); {
    test2->add(BOOST_TEST_CASE(&test_addition));
    test2->add(BOOST_TEST_CASE(&test_substraction));
    test2->add(BOOST_TEST_CASE(&test_multyplication));
    test2->add(BOOST_TEST_CASE(&test_devide));
  }

  framework::master_test_suite().add(test1);
  framework::master_test_suite().add(test2);

  return 0;
}