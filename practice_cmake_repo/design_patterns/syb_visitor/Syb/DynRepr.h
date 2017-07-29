/**
\brief The module implements a function for getting list of fields in runtime.

It's highly inefficient and it will be replaced soon.
\depricated
*/
#ifndef SYB_DYN_REPR_H
#define SYB_DYN_REPR_H

//#include "Syb/Syb.h"
#include "Syb.h"

#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/ref.hpp>
#include <list>
#include <vector>

namespace qx {
  namespace syb {

    //TODO: singleton?
    struct GetFields:Visitor<GetFields> {

      inline explicit GetFields(std::list<boost::any>& res):res_(res){}
#ifdef NOT_INLINES
      template<typename T>
      inline void operator()(T& l) const {
        notInline(*this,l);
      }

      template<typename T>
      inline void runInline(T& l) const {
        res_.push_back(boost::ref(l));
      }
#else
      template<typename T>
      inline void operator()(T& l) const {
        res_.push_back(boost::ref(l));
      }
#endif
    private:
      std::list<boost::any>& res_;
    };

    template<bool isConstT>
    struct EntPtr:mpl::if_c<isConstT,unsigned char const*,unsigned char*> {};

    typedef EntPtr<true>::type c_ent_ptr_t;
    typedef EntPtr<false>::type ent_ptr_t;

    template<typename FldT>
    struct FieldRedir {
      typedef typename boost::remove_const<FldT>::type fld_t;
      inline explicit FieldRedir(size_t s):shift_(s){}

      inline fld_t& operator()(ent_ptr_t v) const {
        return *reinterpret_cast<fld_t*>(v + shift_);
      }
      inline fld_t const& operator()(c_ent_ptr_t v) const {
        return *reinterpret_cast<fld_t const*>(v + shift_);
      }
    private:
      size_t shift_;
    };

    template<typename FunT>
    struct GmapAccessors {
      inline GmapAccessors(FunT const& f, c_ent_ptr_t const& s):fun_(f),sample_(s){}

      template<typename FldT>
      inline void operator()(FldT const& f) const {
        fun_(FieldRedir<FldT>(
          reinterpret_cast<size_t>(&f) 
          - reinterpret_cast<size_t>(sample_)));
      }
    private:
      FunT const& fun_;
      c_ent_ptr_t sample_;
    };

    template<typename T, typename FunT>
    void gmapAccessors(FunT const& f) {
      static T* sample = 0;
      T::gmap(*sample,GmapAccessors<FunT>(f,reinterpret_cast<c_ent_ptr_t>(sample)));
    }

    template<typename T>
    inline void getFields(T& v, std::list<boost::any>& res) {
      GetFields fun(res);
      gmap(v,fun,Unit());
    }
  }
}
#endif
