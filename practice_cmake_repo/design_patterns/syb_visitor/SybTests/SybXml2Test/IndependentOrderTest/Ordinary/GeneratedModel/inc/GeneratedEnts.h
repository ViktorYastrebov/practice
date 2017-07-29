/*
_QXTGENERATED_
This file was generated from qxt template.
WARNING: do not change it!!!
*/
#ifndef GENERATED_ORDINARY_MODEL_H
#define GENERATED_ORDINARY_MODEL_H

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
namespace ordinary {
  using namespace syb;

  template<typename T>
  struct IsOrdinaryModel:mpl::false_{};
    

  NEWTYPE(Code, std::string);

  template<>
  struct IsOrdinaryModel<Code>:mpl::true_{};
      

  NEWTYPE(Value, int);

  template<>
  struct IsOrdinaryModel<Value>:mpl::true_{};
      

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
    
    ordinary::Value value_;
    
    TRAVERSABLE_BEGIN(CrowdFirstEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsOrdinaryModel<CrowdFirstEntity>:mpl::true_{};
      

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
    
    ordinary::Value value_;
    
    TRAVERSABLE_BEGIN(CrowdSecondEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsOrdinaryModel<CrowdSecondEntity>:mpl::true_{};
      

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
    
    ordinary::Value value_;
    
    TRAVERSABLE_BEGIN(FirstEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsOrdinaryModel<FirstEntity>:mpl::true_{};
      

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
    
    ordinary::Value value_;
    
    TRAVERSABLE_BEGIN(SecondEntity) 
      TRAVERSABLE_FIELD(value_)
    TRAVERSABLE_END


  };
  template<>
  struct IsOrdinaryModel<SecondEntity>:mpl::true_{};
      

  struct IndependentOrderTest  {

    
        
    inline const ordinary::FirstEntity& firstEntity() const {
      return firstEntity_;
    }
    
    inline ordinary::FirstEntity& firstEntity() {
      return firstEntity_;
    }

    inline IndependentOrderTest& firstEntity(const ordinary::FirstEntity& v) { 
      firstEntity_ = v;
      return *this;
    }
 

        
    inline const ordinary::SecondEntity& secondEntity() const {
      return secondEntity_;
    }
    
    inline ordinary::SecondEntity& secondEntity() {
      return secondEntity_;
    }

    inline IndependentOrderTest& secondEntity(const ordinary::SecondEntity& v) { 
      secondEntity_ = v;
      return *this;
    }
 

        
    inline const ordinary::CrowdFirstEntity& crowdFirstEntity() const {
      return crowdFirstEntity_;
    }
    
    inline ordinary::CrowdFirstEntity& crowdFirstEntity() {
      return crowdFirstEntity_;
    }

    inline IndependentOrderTest& crowdFirstEntity(const ordinary::CrowdFirstEntity& v) { 
      crowdFirstEntity_ = v;
      return *this;
    }
 

        
    inline const ordinary::CrowdSecondEntity& crowdSecondEntity() const {
      return crowdSecondEntity_;
    }
    
    inline ordinary::CrowdSecondEntity& crowdSecondEntity() {
      return crowdSecondEntity_;
    }

    inline IndependentOrderTest& crowdSecondEntity(const ordinary::CrowdSecondEntity& v) { 
      crowdSecondEntity_ = v;
      return *this;
    }
 
  
  private:
    
    ordinary::FirstEntity firstEntity_;
    ordinary::SecondEntity secondEntity_;
    ordinary::CrowdFirstEntity crowdFirstEntity_;
    ordinary::CrowdSecondEntity crowdSecondEntity_;
    
    TRAVERSABLE_BEGIN(IndependentOrderTest) 
      TRAVERSABLE_FIELD(firstEntity_)
      TRAVERSABLE_FIELD(secondEntity_)
      TRAVERSABLE_FIELD(crowdFirstEntity_)
      TRAVERSABLE_FIELD(crowdSecondEntity_)
    TRAVERSABLE_END


  };
  template<>
  struct IsOrdinaryModel<IndependentOrderTest>:mpl::true_{};
      

  struct SomethingNotDefined  {

      
  private:
    
    
    TRAVERSABLE_BEGIN(SomethingNotDefined) 
    TRAVERSABLE_END


  };
  template<>
  struct IsOrdinaryModel<SomethingNotDefined>:mpl::true_{};
      

  NEWTYPE(StdString, std::string);

  template<>
  struct IsOrdinaryModel<StdString>:mpl::true_{};
      

  NEWTYPE(Void, ordinary::Unit);

  template<>
  struct IsOrdinaryModel<Void>:mpl::true_{};
      
    

}
  

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::Code,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::Code,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::Code>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::Value,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::Value,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::Value>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::CrowdFirstEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::CrowdFirstEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::CrowdFirstEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::CrowdSecondEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::CrowdSecondEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::CrowdSecondEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::FirstEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::FirstEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::FirstEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<> 
    struct HasXmlIndependentOrder<ordinary::FirstEntity>:mpl::true_{};
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::SecondEntity,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::SecondEntity,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::SecondEntity>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<> 
    struct HasXmlIndependentOrder<ordinary::SecondEntity>:mpl::true_{};
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::IndependentOrderTest,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::IndependentOrderTest,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::IndependentOrderTest>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::SomethingNotDefined,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::SomethingNotDefined,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::SomethingNotDefined>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::StdString,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::StdString,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::StdString>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }

  namespace syb {
    template<typename TagT>
    struct XmlKind<ordinary::Void,TagT>:XmlElementTag{};
  }


  namespace syb {
    template<typename TagT>
    struct XmlElementName <ordinary::Void,TagT> {
      inline static QName const& value() {
        static QName ret = QName(TypeName<ordinary::Void>::value(),
                                 "",
                                 "");
        return ret;
      }
    };
  }


}

#endif  // GENERATED_ORDINARY_MODEL_H
