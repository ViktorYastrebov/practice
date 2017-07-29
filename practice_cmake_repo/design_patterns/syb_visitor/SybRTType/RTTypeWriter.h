#ifndef SYB_RTTYPE_WRITTER_H
#define SYB_RTTYPE_WRITTER_H

#include "RTTypeCommon.h"
#include "RTTypeStack.h"

#include "Syb/SybAll.h"

#include <loki/Pimpl.h>

#include <stack>

namespace qx {
  namespace syb {
    
    template<typename TagT>
    struct RTTypeWriter;


    namespace priv {
      template<typename T, typename TagT, typename EnableT=void>
      struct RTTypeWriterDispatcher;


      struct RTTypeWriterImpl : RTTypeStack {

        explicit RTTypeWriterImpl();

        ~RTTypeWriterImpl();        

        RTType createRTVector(const std::string& s) const;
        void addRTVectorItem(const RTType& rtVectorObj, const RTType& rtObj) const;
        
        RTType createRTObject(const std::string& s) const;
        RTType createRTObject(const UnicodeString& s) const;
        RTType createRTObject(const boost::gregorian::date& v) const;
        RTType createRTObject(int i) const;
        RTType createRTObject(bool v) const;
        RTType createRTObject(double v) const;        

        /*----------------------------Type construction-----------------------*/
        void constructTypeBefore(const std::string& name);
        void constructTypeAfter();

        template<typename T>
        void constructPrimitive(const T& v) {
          RTType rtObj = createRTObject(unAlias(v));
          rtObj->SetName(TypeName<T>::value().c_str());
          addRTVectorItem(topItem(), rtObj);
        }
        /*--------------------------------------------------------------------*/

        void getResult(RTType& rt) const;
        
      private:        
        Loki::PimplOf<RTTypeWriterImpl>::Type impl_;
      };
    }        


    /*---------------------Dispatchers------------------------------*/    
    namespace priv {

      template<typename T,typename TagT, typename EnableT>
      struct RTTypeWriterDispatcher
      {

        inline static typename void
         action(const RTTypeWriter<TagT>& w, const T& v) {

           w.impl_.constructTypeBefore(TypeName<T>::value());
           gmap(v,w,Unit());
           w.impl_.constructTypeAfter();
        }
      };


      template<typename T,typename TagT>
      struct RTTypeWriterDispatcher<T, TagT, 
        typename boost::enable_if <
          boost::is_same< 
            typename UnAlias<T>::type, 
            boost::optional<typename UnAlias<T>::type::value_type> 
          >
        >::type
      > 
      {
        inline static typename void
         action(const RTTypeWriter<TagT>& w, const T& v) {           
           if (!!unAlias(v)) {
             w(unAlias(v).get());
           }           
        }
      };


      template<typename T,typename TagT>
      struct RTTypeWriterDispatcher<T, TagT, 
        typename boost::enable_if <
          IsPrimitive<typename UnAlias<T>::type> 
        >::type 
      > 
      {
        inline static typename void
         action(const RTTypeWriter<TagT>& w, const T& v) {

           w.impl_.constructPrimitive(v);
        }
      };


      template<typename T,typename TagT>
      struct RTTypeWriterDispatcher<T, TagT,
        typename boost::enable_if <
          IsCont<typename UnAlias<T>::type> 
        >::type 
      > 
      {
        inline static typename void
         action(const RTTypeWriter<TagT>& w, const T& v) {

          typedef UnAlias<T>::type::value_type cont_elem_type_alias;
          typedef UnAlias<cont_elem_type_alias>::type cont_elem_type;
                    
          RTType rtVectorObj = w.impl_.createRTVector(TypeName<T>::value());

          w.impl_.addRTVectorItem(w.impl_.topItem(), rtVectorObj);

          w.impl_.pushItem(rtVectorObj);

          for (UnAlias<T>::type::const_iterator it = unAlias(v).begin(); it != unAlias(v).end(); ++it) {
            w(*it);
          }

          w.impl_.popItem();
        }
      };
    }


    template<typename TagT>
    struct RTTypeWriter:Visitor<RTTypeWriter<TagT> > {
      inline explicit RTTypeWriter(RTType& rt) : rt_(rt) {}

      ~RTTypeWriter() {}

      template<typename T>
      inline void operator()(const T& v) const {
        priv::RTTypeWriterDispatcher<T,TagT>::action(*this,v);

        //TODO: fix this extra stuff
        getResult(rt_);
      }

      void getResult(RTType& rt) const {
        impl_.getResult(rt);
      }

    private:
      mutable priv::RTTypeWriterImpl impl_;
      RTType& rt_;

      template<typename T, typename TagT, typename EnableT>
      friend struct priv::RTTypeWriterDispatcher;
    };
  }
}

#endif /*SYB_RTTYPE_WRITTER_H*/
