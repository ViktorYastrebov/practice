#include "RTTypeReader.h"
#include "RTTypeCommon.h"

#include "rttype/rtint.h"
#include "rttype/rtstring.h"
#include "rttype/rtvector.h"
#include "rttype/rtbool.h"
#include "rttype/rtdate.h"
#include "rttype/rtfloat.h"

#include "misc/exception.h"

#include <stack>
#include <iostream>

using namespace qx;
using namespace qx::syb;

using std::cout;
using std::endl;


template<>
struct Loki::ImplOf<qx::syb::priv::RTTypeReaderImpl> {
  ImplOf() : currentRTVectorItemIndex(0) {}
  ~ImplOf() {}
  
  RTType rtRootObj;
  int currentRTVectorItemIndex;
};


namespace qx {
  namespace syb {
    namespace priv {

      RTTypeReaderImpl::RTTypeReaderImpl() {}


      RTTypeReaderImpl::~RTTypeReaderImpl() {}


      void RTTypeReaderImpl::setRootObj(const RTType& rtRootObj) {
        impl_->rtRootObj = rtRootObj;
      }


      void RTTypeReaderImpl::checkRootObj() const {
        checkBool(impl_->rtRootObj.HaveData(), "Root object must be already initialized");        
      }


      RTType RTTypeReaderImpl::getRTVectorItem(const RTType& rtVector, const std::string& typeName) const {
        assert (rtVector.HaveData());

        RTType result = NULL;        
        result = RTVector(rtVector)->_ElementReference(typeName.c_str());

        return result;
      }


      RTType RTTypeReaderImpl::getRTVectorItem(const RTType& rtVector, int index) {        
        assert (rtVector.HaveData());

        RTType result = NULL;        
        result = RTVector(rtVector)->_ElementReference(index);

        setCurrentRTVectorItemIndex(0);

        return result;
      }


      void RTTypeReaderImpl::getRTTypeObjectVal(const RTType& rttypeObj, std::string& v) const {
        v = RTString(rttypeObj)->GetValue();
      }


      void RTTypeReaderImpl::getRTTypeObjectVal(const RTType& rttypeObj, UnicodeString& v) const {
        std::string str;
        getRTTypeObjectVal(rttypeObj, str);
        v = UnicodeString(str.c_str());
      }


      void RTTypeReaderImpl::getRTTypeObjectVal(const RTType& rttypeObj, boost::gregorian::date& v) const {        
        int val = RTDate(rttypeObj)->GetValue();        
        v = boost::gregorian::date(val);
      }


      void RTTypeReaderImpl::getRTTypeObjectVal(const RTType& rttypeObj, int& v) const {
        const_cast<int&>(v) = RTInt(rttypeObj)->GetValue();
      }


      void RTTypeReaderImpl::getRTTypeObjectVal(const RTType& rttypeObj, bool& v) const {        
        const_cast<bool&>(v) = (RTBool(rttypeObj)->GetValue() == RTBOOLTRUE);
      }


      void RTTypeReaderImpl::getRTTypeObjectVal(const RTType& rttypeObj, double& v) const {
        const_cast<double&>(v) = RTFloat(rttypeObj)->GetValue();
      }


      void RTTypeReaderImpl::constructTypeBefore(const std::string& childName) {        
        if (!isEmpty()) {
          RTType rttypeObj = getRTVectorItem(topItem(), childName);
          pushItem(rttypeObj);
        } else {          
          pushItem(impl_->rtRootObj);
        }
      }


      void RTTypeReaderImpl::constructTypeBefore(int index) {        
        assert(!isEmpty());

        try {          
          RTType rttypeObj = getRTVectorItem(topItem(), index);          
          pushItem(rttypeObj);
        } catch (Exception* e) {
          std::cerr << "Can't find necessary element, type in not complete!" << std::endl;
          std::cerr << "error code = " << e->GetCode() << std::endl;
        }
      }


      void RTTypeReaderImpl::constructTypeAfter() {
        if (getSize() > 1) {          
          popItem();
        }
      }      


      int RTTypeReaderImpl::getRTVectorLen(const RTType& rtVector) const {
        return RTVector(rtVector)->GetLen();
      }


      int RTTypeReaderImpl::getCurrentRTVectorItemIndex() const {
        return impl_->currentRTVectorItemIndex;
      }


      void RTTypeReaderImpl::setCurrentRTVectorItemIndex(int index) {
        impl_->currentRTVectorItemIndex = index;
      }
    }
  }
}
