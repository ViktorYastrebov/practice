/**
\brief Main file for SYB library.

Contains all general functionality for whole library.
Implements Spine's traversal.

\todo track recursions
\todo implement friend check for all CRTP idioms
\todo make all static things fully static
\todo strategic programming
*/
#ifndef SYB_SYB_H
#define SYB_SYB_H

#include <boost/type_traits.hpp>
#include <boost/variant.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/call_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/value_init.hpp>
#include <boost/optional.hpp>
#include <boost/ref.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/type_traits.hpp>
#include <boost/functional.hpp>
#include <boost/function.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <unicode/unistr.h>


//#include "Syb/Tags.h"
#include "Tags.h"

#define ACCEPTORS_VIA_HIERARCHY_BUILDER
namespace qx {
  
  namespace cfg {
    typedef size_t key_t;
  }
  
  namespace syb {

    namespace mpl = boost::mpl;

    struct HistoryTag {};

    template<typename T, typename EnableT=void>
    struct CleanType:Ret<T>{};

    template<typename T, typename SpineT, typename EnT=void>
    struct GmapRedir {
      inline static void action(T&, SpineT const&) {
      }
    };

    /**
    \brief Metafunction for returning actual type of an object
    without alias reference or some other manipulators

    \tparam T

    \todo get values from function's signature

    */
    template<typename T, typename EnableT>
    struct CleanType<boost::reference_wrapper<T>,EnableT>:Ret<T>{};

    /** 
    \brief Clean value from alias references and various manipulators
    */
    template<typename T>
    inline T& cleanType(T& v) { return v; }

    template<typename T>
    inline T& cleanType(boost::reference_wrapper<T> const& v) {
      return v.get();
    }
    
    template<typename L, typename R, typename FunT>
    struct GmapRedir<std::pair<L,R>,FunT> {
      inline static void action(std::pair<L,R>& p, FunT const& f) {
        //f(p->first);
        //f(p->second);
        f(const_cast<typename boost::remove_const<L>::type&>(p.first));
        f(p.second);
      }
    };

    template<typename L, typename R, typename FunT>
    struct GmapRedir<std::pair<L,R> const,FunT> {
      inline static void action(std::pair<L,R> const& p, FunT const& f) {
        //f(p->first);
        //f(p->second);
        f(const_cast<typename boost::remove_const<L>::type&>(p.first));
        f(p.second);
      }
    };

    struct VisitorTag{};

    /**
    \brief Meta predicate which returns true if the object supports 
    Visitor's pattern requirements.
    */
    template<typename T> 
    struct IsVisitor:boost::is_base_of<VisitorTag,T>{};

    /**
    \brief Visitor component from Visitor pattern.

    \tparam FunT generic function
    */
    template<typename FunT>
    struct Visitor:VisitorTag {
      template<typename U>
      inline void visit(U& v) const {
        fun()(v);
      }

      template<typename U>
      inline void visit(U const& v) const {
        fun()(v);
      }
      template<typename T>
      inline void polyVisit(T& v) const {
        gmap(v,fun(),Unit());
      }
      template<typename T>
      inline void polyVisit(T const& v) const {
        gmap(v,fun(),Unit());
      }

      inline FunT const& fun() const { return *static_cast<FunT const*>(this); }
    };
#ifdef NOT_INLINES
    template<typename FunT, typename T>
    void notInline(FunT const& f, T& v);
#endif
    /**
    \brief Type trait for a primitive type check
    */
    template<typename T, typename EnableT=void> 
    struct IsPrimitive:boost::is_scalar<T>{};

    template<>
    struct IsPrimitive<std::string,void>:mpl::true_{};

    template<>
    struct IsPrimitive<UnicodeString,void>:mpl::true_{};

    template<>
    struct IsPrimitive<boost::posix_time::ptime>:mpl::true_{};

    template<>
    struct IsPrimitive<boost::gregorian::date>:mpl::true_{};

    template<typename T> struct IsSharedPtr:mpl::false_{};
    template<typename T> struct IsSharedPtr<boost::shared_ptr<T> >:mpl::true_{};
    
    /*
    template<>
    struct IsPrimitive<std::vector<unsigned int> >:mpl::true_{};
    */

    /**
    \brief Type trait for a unicode type check
    */
    template<typename T, typename EnableT=void> 
    struct IsUnicode:mpl::false_{};

    /**
    \brief returns name of a type

    The template is specialized automatically by 
    TRAVERSABLEsomething macroses

    In any reimplementation try to ensure return value is constat reference

    \todo probably std::ostream is better here
    */
    template<typename T, typename EnableT=void>
    struct TypeName {
      /// \brief actually the static function returns the name
      inline static std::string const& value() {
        //TODO: probably with is useless and only prevents inlining
        static std::string ret = typeid(T).name();
        return ret;
      }
    };

    template<typename T, typename EnableT=void>
    struct TypeNamePlural {
      inline static std::string const& value() {
        static std::string const ret = TypeName<T>::value() + "s";
        return ret;
      }
    };

    BOOST_MPL_HAS_XXX_TRAIT_DEF(spine_tag_t)
      BOOST_MPL_HAS_XXX_TRAIT_DEF(external_name_tag_t)

      /**
      \brief Type trait that tests whether traverse configuration is 
      defined within a class declaration or external
      */
      template<typename T>
    struct EmbeddedInfo:
#ifdef BOOST_MPL_CFG_NO_HAS_XXX
      has_spine_tag_t<T,mpl::true_>
#else
      has_spine_tag_t<T>
#endif
    {};

    /*
    \brief Checks name is overloaded via TypeName
    */
    template<typename T>
    struct HasExternalName:
#ifdef BOOST_MPL_CFG_NO_HAS_XXX
      has_external_name_tag_t<T,mpl::true_>
#else
      has_external_name_tag_t<T>
#endif
    {};

    struct ExternalTypeName {
      typedef void external_name_tag_t;
    };

    template<typename T>
    struct TypeName<T,typename boost::enable_if<mpl::and_<
      EmbeddedInfo<T>
      ,mpl::not_<HasExternalName<T> >
    > >::type> 
    {
      inline static std::string const& value() { 
        static std::string ret = T::sybTypeName();      
        return ret;
      }
    };

    template<typename T, typename SpineT>
    struct GmapRedir<T, SpineT, typename boost::enable_if<EmbeddedInfo<
      typename boost::remove_const<T>::type>
      >::type> {
      inline static void action(T& p, SpineT const& s) {
        T::gmap(p,s);
      }
    };

    template<typename T,typename SpineT, typename EnT>
    inline void gmap(T& p, SpineT const& s, EnT const&) 
    {
      GmapRedir<T,SpineT>::action(p,s);
    }

    inline void nullDeleter(void*) {}
    template<typename T>
    inline boost::shared_ptr<T> noDelShared(T* t = NULL) {
      return boost::shared_ptr<T>(t,&nullDeleter);
    }
    
    
    template<typename T, typename EnT=void>
    struct StringRepr : mpl::false_ {
      static bool fromString(T& v, UnicodeString const& s) { return false; }
      static void toString(T const& v, UnicodeString& s) {}
    };

    template<>
    struct StringRepr<UnicodeString> : mpl::true_ {
      static bool fromString(UnicodeString& v, UnicodeString const& s) {
        v = s;
        return true;
      }
      static void toString(UnicodeString const& v, UnicodeString& s) {
        s = v;
      }
    };

    template<>
    struct StringRepr<boost::optional<UnicodeString> > : mpl::true_ {
      static bool fromString(boost::optional<UnicodeString>& v, UnicodeString const& s) {
        v = s;
        return true;
      }
      static void toString(boost::optional<UnicodeString> const& v, UnicodeString& s) {
        if (v)
          s = *v;
      }
    };

    template<typename U>
    struct StringRepr<U, typename boost::enable_if<
      mpl::and_<IsPrimitive<U>,mpl::not_<boost::is_same<U,UnicodeString> > >
      >::type
    > : mpl::true_
    {
      static bool fromString(U& v, UnicodeString const& s) {
        std::string us;
        us = s.toUTF8String(us);
        try {
          v = boost::lexical_cast<U>(us);
        } catch(boost::bad_lexical_cast const&) {
          return false;
        }
        return true;
      }
      static void toString(U const& v, UnicodeString& s) {
        s = UnicodeString::fromUTF8(boost::lexical_cast<std::string>(v));
      }
    };

    template<typename U>
    struct StringRepr<boost::optional<U>, typename boost::enable_if<
      mpl::and_<IsPrimitive<U>,mpl::not_<boost::is_same<U,UnicodeString> > >
      >::type
    > : mpl::true_
    {
      static bool fromString(boost::optional<U>& v, UnicodeString const& s) {
        if (s.isEmpty()) 
          return true;
        std::string us;
        us = s.toUTF8String(us);
        try {
          v = boost::lexical_cast<U>(us);
        } catch(boost::bad_lexical_cast const&) {
          return false;
        }
        return true;
      }
      static void toString(boost::optional<U> const& v, UnicodeString& s) {
        if (v)
          s = UnicodeString::fromUTF8(boost::lexical_cast<std::string>(*v));
      }
    };

    template<>
    struct StringRepr<boost::optional<bool> > : mpl::true_ {
      static bool fromString(boost::optional<bool>& v, UnicodeString const& s) {
        if (s.isEmpty()) {
          v.reset();
          return true;
        }
        std::string us;
        us = s.toUTF8String(us);
        try {
          v = us[0] == '1' || us[0] == 'y' || us[0] == 'Y' || us[0] == 't' || us[0] == 'T';
        } catch(boost::bad_lexical_cast const&) {
          return false;
        }
        return true;
      }
      static void toString(boost::optional<bool> const& v, UnicodeString& s) {
        if (v)
          s = UnicodeString::fromUTF8(*v?"yes":"no");
      }
    };

    template<>
    struct StringRepr<bool> : mpl::true_ {
      static bool fromString(bool& v, UnicodeString const& s) {
        std::string us;
        us = s.toUTF8String(us);
        v = !us.empty() && (us[0] == '1' || us[0] == 'y' || us[0] == 'Y' || us[0] == 't' || us[0] == 'T');
        return true;
      }
      static void toString(boost::optional<bool> const& v, UnicodeString& s) {
        s = UnicodeString::fromUTF8(*v?"yes":"no");
      }
    };

  }
}

/**
\brief By means of this macro we can enable or disable additional checks.
Disabling is needed for improving compilation speed

*/
//#define CT_ENABLE(v) typename boost::enable_if<v>::type
#define CT_ENABLE(v) void

#define TRAVERSABLE_BEGIN(name)                                     \
  public:                                                           \
  typedef void spine_tag_t;                                         \
  static std::string sybTypeName() { return #name; }                \
  template<typename T, typename FunT>                               \
  inline static CT_ENABLE((boost::is_same<name                      \
  ,typename boost::remove_const<T>::type>))                         \
  gmap(T& p, FunT const& f)                                         \
{

#define TRAVERSABLE_FIELD(name)	f(p.name);

#define TRAVERSABLE_END }

#endif
