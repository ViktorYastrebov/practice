/*
_QXTGENERATED_
This file was generated from qxt template.
WARNING: do not change it!!!
*/
#ifndef GENERATED_NO_SKIP_XML_ELEMENT_ORDER_MODEL_H
#define GENERATED_NO_SKIP_XML_ELEMENT_ORDER_MODEL_H

#include <list>
#include <string>

#include <boost/scoped_ptr.hpp>

#if 0
#include "Syb/Binary.h"
#include "Syb/Equals.h"
#include "Syb/SybAll.h"
#include "SybXml2/XmlTypes.h"
#include "SybXml2/XmlReader.h"
#else
#include "../../Syb/Binary.h"
#include "../../Syb/Equals.h"
#include "../../Syb/SybAll.h"
#include "../../SybXml2/XmlTypes.h"
#include "../../SybXml2/XmlReader.h"
#endif
#include "ForwardDecls.h"

namespace qx {
namespace no_skip_xml_element {
  using namespace syb;

  template<typename T>
  struct IsNoSkipXmlElementModel:mpl::false_{};
    

  NEWTYPE(Code, std::string);

  template<>
  struct IsNoSkipXmlElementModel<Code>:mpl::true_{};
      

  NEWTYPE(Value, int);

  template<>
  struct IsNoSkipXmlElementModel<Value>:mpl::true_{};
      

  struct CrowdFirstEntity  {

    
        
    inline const int& value() const {
      return value_.val();
    }
    
    inline int& value() {
      return value_.val();
    }

    inline CrowdFirstEntity& value(const int& v) { 
      value_ = v;
      return *this;
    }
 
  
  private:
    
    no_skip_xml_element::Value value_;
    
    TRAVERSABLE_BEGIN(CrowdFirstEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsNoSkipXmlElementModel<CrowdFirstEntity>:mpl::true_{};
      

  struct CrowdSecondEntity  {

    
        
    inline const int& value() const {
      return value_.val();
    }
    
    inline int& value() {
      return value_.val();
    }

    inline CrowdSecondEntity& value(const int& v) { 
      value_ = v;
      return *this;
    }
 
  
  private:
    
    no_skip_xml_element::Value value_;
    
    TRAVERSABLE_BEGIN(CrowdSecondEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsNoSkipXmlElementModel<CrowdSecondEntity>:mpl::true_{};
      

  struct FirstEntity  {

    
        
    inline const int& value() const {
      return value_.val();
    }
    
    inline int& value() {
      return value_.val();
    }

    inline FirstEntity& value(const int& v) { 
      value_ = v;
      return *this;
    }
 
  
  private:
    
    no_skip_xml_element::Value value_;
    
    TRAVERSABLE_BEGIN(FirstEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsNoSkipXmlElementModel<FirstEntity>:mpl::true_{};
      

  struct IndependentOrderTest  {

    
        
    inline const std::list<no_skip_xml_element::FirstEntity>& firstEntity() const {
      return firstEntityList_;
    }
    
    inline std::list<no_skip_xml_element::FirstEntity>& firstEntity() {
      return firstEntityList_;
    }

    inline IndependentOrderTest& firstEntity(const std::list<no_skip_xml_element::FirstEntity>& v) { 
      firstEntityList_ = v;
      return *this;
    }
 

        
    inline const std::list<no_skip_xml_element::SecondEntity>& secondEntity() const {
      return secondEntityList_;
    }
    
    inline std::list<no_skip_xml_element::SecondEntity>& secondEntity() {
      return secondEntityList_;
    }

    inline IndependentOrderTest& secondEntity(const std::list<no_skip_xml_element::SecondEntity>& v) { 
      secondEntityList_ = v;
      return *this;
    }
 

        
    inline const no_skip_xml_element::CrowdFirstEntity& crowdFirstEntity() const {
      return crowdFirstEntity_;
    }
    
    inline no_skip_xml_element::CrowdFirstEntity& crowdFirstEntity() {
      return crowdFirstEntity_;
    }

    inline IndependentOrderTest& crowdFirstEntity(const no_skip_xml_element::CrowdFirstEntity& v) { 
      crowdFirstEntity_ = v;
      return *this;
    }
 

        
    inline const no_skip_xml_element::CrowdSecondEntity& crowdSecondEntity() const {
      return crowdSecondEntity_;
    }
    
    inline no_skip_xml_element::CrowdSecondEntity& crowdSecondEntity() {
      return crowdSecondEntity_;
    }

    inline IndependentOrderTest& crowdSecondEntity(const no_skip_xml_element::CrowdSecondEntity& v) { 
      crowdSecondEntity_ = v;
      return *this;
    }
 
  
  private:
    
    std::list<no_skip_xml_element::FirstEntity> firstEntityList_;
    std::list<no_skip_xml_element::SecondEntity> secondEntityList_;
    no_skip_xml_element::CrowdFirstEntity crowdFirstEntity_;
    no_skip_xml_element::CrowdSecondEntity crowdSecondEntity_;
    
    TRAVERSABLE_BEGIN(IndependentOrderTest) 
      TRAVERSABLE_FIELD(firstEntityList_)
      TRAVERSABLE_FIELD(secondEntityList_)
      TRAVERSABLE_FIELD(crowdFirstEntity_)
      TRAVERSABLE_FIELD(crowdSecondEntity_)
    TRAVERSABLE_END


  };
  template<>
  struct IsNoSkipXmlElementModel<IndependentOrderTest>:mpl::true_{};
      

  struct SecondEntity  {

    
        
    inline const int& value() const {
      return value_.val();
    }
    
    inline int& value() {
      return value_.val();
    }

    inline SecondEntity& value(const int& v) { 
      value_ = v;
      return *this;
    }
 
  
  private:
    
    no_skip_xml_element::Value value_;
    
    TRAVERSABLE_BEGIN(SecondEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsNoSkipXmlElementModel<SecondEntity>:mpl::true_{};
      

  struct SomethingNotDefined  {

      
  private:
    
    
    TRAVERSABLE_BEGIN(SomethingNotDefined) 
    TRAVERSABLE_END


  };
  template<>
  struct IsNoSkipXmlElementModel<SomethingNotDefined>:mpl::true_{};
      

  NEWTYPE(StdString, std::string);

  template<>
  struct IsNoSkipXmlElementModel<StdString>:mpl::true_{};
      

  NEWTYPE(Void, no_skip_xml_element::Unit);

  template<>
  struct IsNoSkipXmlElementModel<Void>:mpl::true_{};
      
    

}
  

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::Code,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::Code,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::Code>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::Value,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::Value,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::Value>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::CrowdFirstEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::CrowdFirstEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::CrowdFirstEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::CrowdSecondEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::CrowdSecondEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::CrowdSecondEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::FirstEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::FirstEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::FirstEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<> 
    struct HasXmlIndependentOrder<no_skip_xml_element::FirstEntity>:mpl::true_{};
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::IndependentOrderTest,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::IndependentOrderTest,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::IndependentOrderTest>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::SecondEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::SecondEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::SecondEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<> 
    struct HasXmlIndependentOrder<no_skip_xml_element::SecondEntity>:mpl::true_{};
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::SomethingNotDefined,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::SomethingNotDefined,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::SomethingNotDefined>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::StdString,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::StdString,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::StdString>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<no_skip_xml_element::Void,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <no_skip_xml_element::Void,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<no_skip_xml_element::Void>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }


}

#endif  // GENERATED_NO_SKIP_XML_ELEMENT_ORDER_MODEL_H
