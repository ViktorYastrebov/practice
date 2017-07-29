#include <fstream>
#define BOOST_TEST_MODULE SybXml2Test
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include <iostream>


#if 0
#include "SybXml2/SybXml2.h"
#include "SybXml2/XmlSpec.inl"
#else
#include "../../SybXml2/SybXml2.h"
#include "../../SybXml2/XmlSpec.inl"
#endif
#include "IndependentOrderTest/Ordinary/Ordinary.h"
#include "IndependentOrderTest/NoSkipXmlElement/NoSkipXmlElement.h"
#include "IndependentOrderTest/WithSkipXmlElement/WithSkipXmlElement.h"

namespace qx {
  namespace msg {
        template<typename T>
    inline void write(std::ostream &output, const std::list<T> &v) {
      boost::uint32_t size = v.size();
      write(output, size);
      if (size) {
        typename std::list<T>::const_iterator it;
        for (it = v.begin(); it != v.end(); ++it)
          write(output, *it);
      }
    }

    template<typename T>
    inline void read(std::istream &input, std::list<T> &v) {
      boost::uint32_t size;
      read(input, size);
      for(boost::uint32_t i = 0; i < size; i++) {
        T result;
        read(input, result);
        v.push_back(result);
      }
    }

    template<typename T>
    inline void write(std::ostream &output, const boost::optional<T> &v) {
      bool hasValue = v;
      write(output, hasValue);
      if (hasValue)
        write(output, *v);
    }

    template<typename T>
    inline void read(std::istream &input, boost::optional<T> &v) {
      bool hasValue;
      read(input, hasValue);
      if (hasValue) {
        T result;
        read(input, result);
        v = result;
      }
    }

  }
  namespace json {
    template <typename T>
    cJSON *write(const boost::optional<T> &v, bool typeIdByAttribute) {
      cJSON *node = 0;
      if (v) {
        node = qx::json::write(*v, typeIdByAttribute);
      } else {
        node = cJSON_CreateObject();
        cJSON_AddItemToObject(node, "_nil", cJSON_CreateTrue());
      }
      return node;
    }

    template <typename T>
    cJSON *write(const std::list<T> &v, bool typeIdByAttribute) {
      cJSON *array = 0;
      if (!v.empty()) {
        array = cJSON_CreateArray();
        BOOST_FOREACH(const T &t, v) {
          cJSON *item = write(t);
          if (item)
            cJSON_AddItemToArray(array, item);
        }
      }
      return array;
    }
  }
}

#include "../../Syb/PolySpec.h"
//#include "Syb/PolySpec.h"

namespace qx {
  const char *defaultXmlNs() {
    static const char ret[] = "";
    return ret;
  }
  const char *defaultXmlNsPrefix() {
    static const char ret[] = "";
    return ret;
  }
  namespace syb {
    template<bool isMultiConstrT>
    struct XmlInfo;

    template<typename T>
    XmlInfo<IsMultiConstr<T>::value> const& xmlInfo();

    template<typename T>
    inline AbsDynInfo const& dynInfo() {
      static AbsDynInfo ret
        = AbsDynInfo(xmlInfo<T>(),dataType<typename UnAlias<T>::type>());
      return ret;
    }
  }
}

std::vector<std::string> OrdinaryTestCases = boost::assign::list_of
  ("IndependentOrderTest/Ordinary/FSCC.xml")
  ("IndependentOrderTest/Ordinary/CCFS.xml")
  ("IndependentOrderTest/Ordinary/FCSC.xml")
  ("IndependentOrderTest/Ordinary/CFCS.xml")
  ("IndependentOrderTest/Ordinary/FSCC_PLAIN.xml")
  ("IndependentOrderTest/Ordinary/CCFS_PLAIN.xml")
  ("IndependentOrderTest/Ordinary/FCSC_PLAIN.xml")
  ("IndependentOrderTest/Ordinary/CFCS_PLAIN.xml");

std::vector<std::string> noSkipXmlElementTestCases = boost::assign::list_of
  ("IndependentOrderTest/NoSkipXmlElement/FSCC.xml")
  ("IndependentOrderTest/NoSkipXmlElement/CCFS.xml")
  ("IndependentOrderTest/NoSkipXmlElement/FCSC.xml")
  ("IndependentOrderTest/NoSkipXmlElement/CFCS.xml")
  ("IndependentOrderTest/NoSkipXmlElement/FSCC_PLAIN.xml")
  ("IndependentOrderTest/NoSkipXmlElement/CCFS_PLAIN.xml")
  ("IndependentOrderTest/NoSkipXmlElement/FCSC_PLAIN.xml")
  ("IndependentOrderTest/NoSkipXmlElement/CFCS_PLAIN.xml");

std::vector<std::string> withSkipXmlElementTestCases = boost::assign::list_of
  ("IndependentOrderTest/WithSkipXmlElement/FFSS.xml")
  ("IndependentOrderTest/WithSkipXmlElement/SSFF.xml")
  ("IndependentOrderTest/WithSkipXmlElement/FSFS.xml")
  ("IndependentOrderTest/WithSkipXmlElement/SFSF.xml")
  ("IndependentOrderTest/WithSkipXmlElement/FFSS_PLAIN.xml")
  ("IndependentOrderTest/WithSkipXmlElement/SSFF_PLAIN.xml")
  ("IndependentOrderTest/WithSkipXmlElement/FSFS_PLAIN.xml")
  ("IndependentOrderTest/WithSkipXmlElement/SFSF_PLAIN.xml");

template<typename T>
void run(const std::vector<std::string>& testCases) {
  T origin;
  std::ifstream originInput(testCases[0].c_str());
  if (!originInput) {
    std::cout << "can't open the xml file" << testCases[0] << std::endl;
  }
  originInput >> qx::syb::fromXml(origin);
  std::stringstream originOutput;
  originOutput << qx::syb::toXml(origin);

  BOOST_FOREACH(std::string testCase, testCases) {
    T custom;
    std::ifstream customInput(testCase.c_str());
    customInput >> qx::syb::fromXml(custom);
    std::stringstream customOutput;
    customOutput << qx::syb::toXml(custom);
    BOOST_CHECK_EQUAL(originOutput.str(), customOutput.str());
  }
}

BOOST_AUTO_TEST_CASE(IndepdendentOrderTest) {
  run<qx::ordinary::IndependentOrderTest>(OrdinaryTestCases);
  run<qx::no_skip_xml_element::IndependentOrderTest>(noSkipXmlElementTestCases);
  run<qx::with_skip_xml_element::IndependentOrderTest>(withSkipXmlElementTestCases);
}
