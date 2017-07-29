#include "stdafx.h"

#include "SybRTType/RTTypeReader.h"
#include "SybRTType/RTTypeWriter.h"

#include "license/Model.h"

#define WINNT
#include "rttype/rttype.h"

#include <iostream>
#include <exception>

#define BOOST_TEST_MODULE SybRTTypeTest

#include <boost/mpl/list.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

BOOST_AUTO_TEST_CASE(Main) {
  qx::License lic1;
  qx::syb::generate(lic1);

  BOOST_CHECK_EQUAL(qx::syb::eq(lic1, lic1), true);

  RTType rttypeObj;
  qx::syb::RTTypeWriter<void> rtWriter(rttypeObj);
  rtWriter(lic1);

  qx::License lic2;
  qx::syb::RTTypeReader<void> rtReader(rttypeObj);
  rtReader(lic2);  

  BOOST_CHECK_EQUAL(qx::syb::eq(lic1, lic2), true);
}


