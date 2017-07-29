/*
_QXTGENERATED_
This file was generated from qxt template.
WARNING: do not change it!!!
*/
#ifndef GENERATED_INDEPENDENT_ORDER_MODEL_H
#define GENERATED_INDEPENDENT_ORDER_MODEL_H

#include <list>
#include <string>

#include <boost/scoped_ptr.hpp>

#if 0
#include "Syb/Binary.h"
#include "Syb/Equals.h"
#include "Syb/SybAll.h"
#include "SybXml2/XmlTypes.h"
#include "SybXml2/XmlReader.h"
#include "ForwardDecls.h"
#else
#include "../../Syb/Binary.h"
#include "../../Syb/Equals.h"
#include "../../Syb/SybAll.h"
#include "../../SybXml2/XmlTypes.h"
#include "../../SybXml2/XmlReader.h"
#include "ForwardDecls.h"
#endif

namespace qx {
namespace with_skip_xml_element {
  using namespace syb;

  template<typename T>
  struct IsWithSkipXmlElementModel:mpl::false_{};
    

  NEWTYPE(Code, std::string);

  template<>
  struct IsWithSkipXmlElementModel<Code>:mpl::true_{};
      

  NEWTYPE(Value, int);

  template<>
  struct IsWithSkipXmlElementModel<Value>:mpl::true_{};
      

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
    
    with_skip_xml_element::Value value_;
    
    TRAVERSABLE_BEGIN(FirstEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsWithSkipXmlElementModel<FirstEntity>:mpl::true_{};
      

  struct IndependentOrderTest  {

    
        
    inline const std::list<with_skip_xml_element::FirstEntity>& firstEntity() const {
      return firstEntityList_;
    }
    
    inline std::list<with_skip_xml_element::FirstEntity>& firstEntity() {
      return firstEntityList_;
    }

    inline IndependentOrderTest& firstEntity(const std::list<with_skip_xml_element::FirstEntity>& v) { 
      firstEntityList_ = v;
      return *this;
    }
 

        
    inline const std::list<with_skip_xml_element::SecondEntity>& secondEntity() const {
      return secondEntityList_;
    }
    
    inline std::list<with_skip_xml_element::SecondEntity>& secondEntity() {
      return secondEntityList_;
    }

    inline IndependentOrderTest& secondEntity(const std::list<with_skip_xml_element::SecondEntity>& v) { 
      secondEntityList_ = v;
      return *this;
    }
 
  
  private:
    
    std::list<with_skip_xml_element::FirstEntity> firstEntityList_;
    std::list<with_skip_xml_element::SecondEntity> secondEntityList_;
    
    TRAVERSABLE_BEGIN(IndependentOrderTest) 
      TRAVERSABLE_FIELD(firstEntityList_)
      TRAVERSABLE_FIELD(secondEntityList_)
    TRAVERSABLE_END


  };
  template<>
  struct IsWithSkipXmlElementModel<IndependentOrderTest>:mpl::true_{};
      

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
    
    with_skip_xml_element::Value value_;
    
    TRAVERSABLE_BEGIN(SecondEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsWithSkipXmlElementModel<SecondEntity>:mpl::true_{};
      

  struct SomethingNotDefined  {

      
  private:
    
    
    TRAVERSABLE_BEGIN(SomethingNotDefined) 
    TRAVERSABLE_END


  };
  template<>
  struct IsWithSkipXmlElementModel<SomethingNotDefined>:mpl::true_{};
      

  NEWTYPE(StdString, std::string);

  template<>
  struct IsWithSkipXmlElementModel<StdString>:mpl::true_{};
      

  NEWTYPE(Void, with_skip_xml_element::Unit);

  template<>
  struct IsWithSkipXmlElementModel<Void>:mpl::true_{};
      
    

}
  

  namespace syb {
    template<typename TagT>
    struct XmlKind<with_skip_xml_element::Code,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <with_skip_xml_element::Code,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<with_skip_xml_element::Code>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<with_skip_xml_element::Value,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <with_skip_xml_element::Value,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<with_skip_xml_element::Value>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<with_skip_xml_element::FirstEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <with_skip_xml_element::FirstEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<with_skip_xml_element::FirstEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<> 
    struct HasXmlIndependentOrder<with_skip_xml_element::FirstEntity>:mpl::true_{};
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<with_skip_xml_element::IndependentOrderTest,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <with_skip_xml_element::IndependentOrderTest,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<with_skip_xml_element::IndependentOrderTest>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<with_skip_xml_element::SecondEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <with_skip_xml_element::SecondEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<with_skip_xml_element::SecondEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<> 
    struct HasXmlIndependentOrder<with_skip_xml_element::SecondEntity>:mpl::true_{};
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<with_skip_xml_element::SomethingNotDefined,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <with_skip_xml_element::SomethingNotDefined,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<with_skip_xml_element::SomethingNotDefined>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<with_skip_xml_element::StdString,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <with_skip_xml_element::StdString,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<with_skip_xml_element::StdString>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<with_skip_xml_element::Void,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <with_skip_xml_element::Void,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<with_skip_xml_element::Void>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }


}

#endif  // GENERATED_INDEPENDENT_ORDER_MODEL_H
