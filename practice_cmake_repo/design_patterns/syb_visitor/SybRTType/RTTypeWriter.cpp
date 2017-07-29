#include "RTTypeWriter.h"

#define WINNT
#include "rttype/rttype.h"
#include "rttype/rtint.h"
#include "rttype/rtstring.h"
#include "rttype/rtstring.h"
#include "rttype/rtbool.h"
#include "rttype/rtfloat.h"
#include "rttype/rtdate.h"
#include "rttype/rtvector.h"

#include <stack>
#include <iostream>

using namespace qx;
using namespace qx::syb;

using std::cout;
using std::endl;


template<>
struct Loki::ImplOf<qx::syb::priv::RTTypeWriterImpl> {
  ImplOf() {}
  ~ImplOf() {}
};


namespace qx {
  namespace syb {
    namespace priv {

      RTTypeWriterImpl::RTTypeWriterImpl() {}


      RTTypeWriterImpl::~RTTypeWriterImpl() {}

      
      RTType RTTypeWriterImpl::createRTVector(const std::string& name) const {
        RTVector rtVectorObj = RTVector();
        rtVectorObj->SetDefault(RTString());
        rtVectorObj->SetDynamic(1);
        rtVectorObj->SetName(name.c_str());
        return rtVectorObj;
      }


      void RTTypeWriterImpl::addRTVectorItem(const RTType& rtVectorObj, const RTType& rtObj) const {
        RTVector rtVectorObj_ = rtVectorObj;
        int ind = rtVectorObj_->Add();
        rtVectorObj_->ElementMakeReference(ind, rtObj);
      }


      RTType RTTypeWriterImpl::createRTObject(const std::string& s) const {
        return RTString(s.c_str());
      }


      RTType RTTypeWriterImpl::createRTObject(const UnicodeString& s) const {
        std::string str;
        s.toUTF8String(str);
        return createRTObject(str);
      }


      RTType RTTypeWriterImpl::createRTObject(const boost::gregorian::date& v) const {         
        int val = v.julian_day();
        return RTDate(val);
      }


      RTType RTTypeWriterImpl::createRTObject(int i) const {
        return RTInt(i);
      }

      
      RTType RTTypeWriterImpl::createRTObject(bool v) const {
        return RTBool(v ? RTBOOLTRUE : RTBOOLFALSE);
      }
      
      
      RTType RTTypeWriterImpl::createRTObject(double v) const {
        return RTFloat(v);
      }


      void RTTypeWriterImpl::constructTypeBefore(const std::string& name) {
        RTVector rtVectorObj = createRTVector(name);        
        pushItem(rtVectorObj);
      }

      
      void RTTypeWriterImpl::constructTypeAfter() {

        if (getSize() > 1) {
          //pop himself
          RTType rtObj = topItem();
          popItem();

          //add himself to the parent vector
          addRTVectorItem(topItem(), rtObj);
        }
      }

      void RTTypeWriterImpl::getResult(RTType& rt) const {
        //TODO: check which assertion should be leaved
        //assert(getSize() == 1); 
        assert(getSize() >= 1); 
        rt = topItem();
      }      
    }
  }
}
