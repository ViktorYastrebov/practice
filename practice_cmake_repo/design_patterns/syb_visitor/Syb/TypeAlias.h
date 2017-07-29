/**
\brief Implement functionality for aliasing types for overloading.

It's needed then we need to redefine behavior of some generic function
for some particular set of fields but not types.
So the user can make type alias and then use it for specializing templates.
*/
#ifndef SYB_TYPE_ALIAS_H
#define SYB_TYPE_ALIAS_H

#if 0
#include "Syb.h"
#include "HistList.h"
#else
#include "Syb.h"
#include "HistList.h"
#endif

#include <boost/call_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/value_init.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

//#include "misc/getenv.h"

#include <list>

namespace qx {
  namespace syb {

    /**
    \brief Tag for marking any class as an alias
    */
    struct NewTypeTag{};

    /**
    \brief Actual type alias implementation.

    The aliases implementation uses Curiously Recurring Template 
    Pattern idiom. So \a AliasT should be child of NewType<AliasT,..>.

    By default the value is initialized using boost::value_initialized.

    \tparam AliasT  new name for a type
    \tparam T    old name for a type
    \tparam EnableT  boost::enable_if helper
    */
    template<typename AliasT,typename T, typename EnableT=void>
    struct NewType : public NewTypeTag {

      inline NewType() : val_(boost::value_initialized<T>().data()) {}
      inline explicit NewType(T const& v) : val_(v) {}

      /// \brief setter
      inline void val(T const& v) { val_ = v; }
      /// \brief getter
      inline T const& val() const { return val_; }
      /// \brief reference getter
      inline T& val() { return val_; }

      // inline operator T() const { return val_; }

      template<typename GmapFunT>
      inline static void gmap(AliasT& p, GmapFunT const& s) {
        s(p.val());
      }
      template<typename GmapFunT>
      inline static void gmap(const AliasT& p, GmapFunT const& s) {
        s(p.val());
      }    

    private:
      T val_;
    };

    template<typename T, typename EnableT=void>
    struct UnAlias:Ret<T> {};

    /** \brief The function gets alias value if it's an alias or simply 
    returns its parameter if it's not.
    */
    template<typename T>
    inline typename boost::disable_if<
        boost::is_base_of<NewTypeTag,T>
       ,T
    >::type const& 
      unAlias(T const& v) { return v; }

    template<typename T>
    inline typename boost::disable_if<
        boost::is_base_of<NewTypeTag,T>
       ,T
    >::type& 
      unAlias(T& v) { return v; }

    template<typename T>
    struct UnAlias<T,typename boost::enable_if<
      boost::is_base_of<NewTypeTag,T>
    >::type>:Ret<typename boost::remove_const<typename T::ValueType>::type>{};

    template<typename T>
    inline typename boost::enable_if<
          boost::is_base_of<NewTypeTag,T>
          ,typename T::ValueType
        >::type const&
      unAlias(T const& v) { return v.val(); }

    template<typename T>
    inline typename boost::enable_if<
          boost::is_base_of<NewTypeTag,T>
          ,typename T::ValueType
        >::type&
      unAlias(T& v) { return v.val(); }

    /** 
    \brief Similar to \a NewType but stores reference of aliased type 
      rather than value itself

    The class is mostly needed for adjusting behaviour of classes that
    is impossible to change. Because it's some 3rd party or it have
    some fragile interface.
    */
    template<typename AliasT, typename T, typename EnableT=void>
    struct NewTypeRef : NewTypeTag {
      inline explicit NewTypeRef(T& v):val_(v){}
      inline T& val() const { return val_; }
    private:
      T& val_;

      template<typename GmapFunT>
      inline static void gmap(AliasT& p, GmapFunT const& s) {
        s(p.val());
      }

      template<typename GmapFunT>
      inline static void gmap(const AliasT& p, GmapFunT const& s) {
        s(p.val());
      }    

    };

    /**
    \brief Helper function for implicit aliased type setting.
    */
    template<typename AliasT,typename T>
    inline NewTypeRef<AliasT,T> alias(T& v) {
      return NewTypeRef<AliasT,T>(v);
    }
    /**
    \brief \a NewTypeRef should be cleared out
    */
    template<typename AliasT,typename T, typename EnableT>
    struct CleanType<NewTypeRef<AliasT,T>, EnableT>:Ret<T>{};

    /**
    \brief \a NewTypeRef should be cleared out
    */
    template<typename AliasT,typename T>
    inline T& cleanType(NewTypeRef<AliasT,T> const& v) { return v.val(); }

    template<typename AliasT,typename T, typename EnableT>
    struct TypeName<NewTypeRef<AliasT,T>, EnableT> {
      inline static std::string value() {
        return TypeName<AliasT>::value();
      }
    };

    template <typename T>
    typename boost::enable_if<
      boost::is_base_of<NewTypeTag,typename boost::remove_const<T>::type>
      , bool
    >::type
    operator<(const T& a, const T& b)
    {
      return a.val() < b.val();
    }
  }
}

/**
\brief The macros defines all a new class this the name as the alias, 
and needed helper functions.

\param n Name of an alias
\param v Type that the new alias will have
*/
#define NEWTYPE(n,v)                                            \
    struct n:syb::NewType<n,v >                                 \
    {                                                           \
        inline n(){}                                            \
        inline n(v const& val) : syb::NewType<n,v >(val){}      \
        typedef v ValueType;                                    \
        typedef void spine_tag_t;                               \
        static std::string sybTypeName() { return #n; }		      \
    };

#define NEWTYPE_WITH_DEFAULT(n,v,d)                             \
    struct n:syb::NewType<n,v >                                 \
    {                                                           \
        inline n() : syb::NewType<n,v >(d){}                    \
        inline n(v const& val) : syb::NewType<n,v >(val){}      \
        typedef v ValueType;                                    \
        typedef void spine_tag_t;                               \
        static std::string sybTypeName() { return #n; }		      \
    };

#define NEWTYPE_LAZY_LOAD(n,v,o)                                     \
    struct n:syb::NewTypeLazyLoad<n,v,o >                            \
    {                                                                \
        inline n(){}                                                 \
        inline n(v const& val) : syb::NewTypeLazyLoad<n,v,o >(val){} \
        typedef v ValueType;                                         \
        typedef void spine_tag_t;                                    \
        static std::string sybTypeName() { return #n; }		           \
    };

#endif
