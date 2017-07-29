#ifndef SYB_RTTYPE_READER_H
#define SYB_RTTYPE_READER_H

#include "Syb/SybAll.h"

#include <loki/Pimpl.h>

#include "RTTypeCommon.h"
#include "RTTypeStack.h"

namespace qx {
  namespace syb {
    
    template <class TagT>
    struct RTTypeReader;

    namespace priv {
      template<typename T, typename TagT, typename EnableT=void>
      struct RTTypeReaderDispatcher;

      struct RTTypeReaderImpl : RTTypeStack {
        
        explicit RTTypeReaderImpl();

        ~RTTypeReaderImpl();       

        RTType getRTVectorItem(const RTType& rtVector, const std::string& typeName) const;
        RTType getRTVectorItem(const RTType& rtVector, int index);

        void getRTTypeObjectVal(const RTType& rttypeObj, std::string& v) const;
        void getRTTypeObjectVal(const RTType& rttypeObj, UnicodeString& v) const;
        void getRTTypeObjectVal(const RTType& rttypeObj, boost::gregorian::date& v) const;
        void getRTTypeObjectVal(const RTType& rttypeObj, int& v) const;
        void getRTTypeObjectVal(const RTType& rttypeObj, bool& v) const;
        void getRTTypeObjectVal(const RTType& rttypeObj, double& vj) const;
        
        /*----------------------------Type construction-----------------------*/
        void constructTypeBefore(const std::string& childName);
        void constructTypeBefore(int index);
        void constructTypeAfter();

        template<typename T>
        void constructPrimitive(T& v) {          

          RTType rtObj;
          if (getCurrentRTVectorItemIndex() != 0) {
            rtObj = getRTVectorItem(topItem(), getCurrentRTVectorItemIndex());
          } else {
            rtObj = getRTVectorItem(topItem(), TypeName<T>::value());
          }

          assert(rtObj.HaveData());

          getRTTypeObjectVal(rtObj, unAlias(v));
        }
        /*--------------------------------------------------------------------*/
                      
        int getRTVectorLen(const RTType& rtVector) const;

        //This keeps vector item index while vector element is being processed
        void setCurrentRTVectorItemIndex(int index);
        int getCurrentRTVectorItemIndex() const;      

        void setRootObj(const RTType& root);
        void checkRootObj() const;
        
      private:
        Loki::PimplOf<RTTypeReaderImpl>::Type impl_;
      };
    }        


    /*---------------------Dispatchers------------------------------*/    
    namespace priv {

      template<typename T,typename TagT, typename EnableT>
      struct RTTypeReaderDispatcher
      {
        inline static typename void
         action(const RTTypeReader<TagT>& w, T& v) {
          
           if (w.impl_.getCurrentRTVectorItemIndex() != 0) {
             w.impl_.constructTypeBefore(w.impl_.getCurrentRTVectorItemIndex());
           } else {
             w.impl_.constructTypeBefore(TypeName<T>::value());
           }
           
           gmap(v,w,Unit());

           w.impl_.constructTypeAfter();
        }
      };


      template<typename T,typename TagT>
      struct RTTypeReaderDispatcher<T, TagT, 
        typename boost::enable_if <
          boost::is_same< 
            typename UnAlias<T>::type, 
            boost::optional<typename UnAlias<T>::type::value_type> 
          >
        >::type 
      >
      {
        inline static typename void
         action(const RTTypeReader<TagT>& w, T& v) {           
           typename UnAlias<T>::type::value_type val;
           w(val);
           unAlias(v) = val;           
        }
      };


      template<typename T,typename TagT>
      struct RTTypeReaderDispatcher<T, TagT, 
        typename boost::enable_if <
          IsPrimitive<typename UnAlias<T>::type> 
        >::type 
      > 
      {
        inline static typename void
         action(const RTTypeReader<TagT>& w, T& v) {
           w.impl_.constructPrimitive(v);
        }
      };

      
      template<typename T,typename TagT>
      struct RTTypeReaderDispatcher<T, TagT,
        typename boost::enable_if <
          IsCont<typename UnAlias<T>::type> 
        >::type 
      > 
      {
        typedef typename UnAlias<T>::type::value_type val_t;

        inline static typename void
        action(const RTTypeReader<TagT>& w, T& v) {
                    
          w.impl_.pushItem(w.impl_.getRTVectorItem(w.impl_.topItem(), TypeName<T>::value()));

          int len = w.impl_.getRTVectorLen(w.impl_.topItem());
          for (int i = 1; i <= len ; ++i) {
            w.impl_.setCurrentRTVectorItemIndex(i);
            
            unAlias(v).push_back(val_t());

            w(unAlias(v).back());
          }

          w.impl_.popItem();
        }
      };
    }


    /*------------------------------------------------------------------*/      
    template<typename TagT>
    struct RTTypeReader: Visitor<RTTypeReader<TagT> > {
      inline explicit RTTypeReader(const RTType& rtRootObj) {
        impl_.setRootObj(rtRootObj);
      }

      ~RTTypeReader() {}      

      template<typename T>
      inline void operator()(T& v) const {        
        priv::RTTypeReaderDispatcher<T,TagT>::action(*this,v);
      }

    private:     
      mutable priv::RTTypeReaderImpl impl_;

      template<typename T, typename TagT, typename EnableT>
      friend struct priv::RTTypeReaderDispatcher;          
    };
  }
}

#endif /*SYB_RTTYPE_READER_H*/
