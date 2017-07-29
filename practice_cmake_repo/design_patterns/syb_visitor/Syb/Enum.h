/**
\brief Simple implementation for enum types

By default it acts like a primitive int type. But type trait 
IsPrimitive is false for it because of several template restrictions.
It uses IsEnum type treaits instead.

It's defined like enum embedded into empty class. This is needed
for avoiding accidental conversions between integer, and so loosing
enum information.

First value of enum should be its default value. And it always should
have explicitely defined value equal to 0.

Also the class should have enum description specified with special macroses.

After this is created we'll have a class Color with operator* to access
integer value of enum.

Then reading/writing from/to stream it will try to use textual representation 
if possible. If the value doesn't have corresponding textual representation it
will use integer. For example if value is a bit mask of several enum's values.

Definition example:

  struct Color {
    enum {
      DEFAULT = 0, Red, Green, Blue
    };

    BEGIN_ENUM_INFO(Color)
      ENUM_ITEM(DEFAULT);
      ENUM_ITEM(Red);
      ENUM_ITEM(Green);
      ENUM_ITEM(Blue);
    END_ENUM_INFO
  };


*/
// we use dynamic representaion, it's quite easy to use to static one, 
// but dynamic representation for now is enough
#ifndef SYB_ENUM_H
#define SYB_ENUM_H

#if 0
#include "Syb/TypeAlias.h"
#include "Syb/Syb.h"
#else
#include "TypeAlias.h"
#include "Syb.h"
#endif
#include <map>
#include <algorithm>
#include <cctype>

#include <loki/AssocVector.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/format.hpp>
#include <boost/operators.hpp>

#include <iostream>

/**
\brief Starts enum description block
\param Name of enclosing class
*/
#define BEGIN_ENUM_INFO(n)                                                  \
  typedef void spine_tag_t;                                                 \
  typedef void enum_tag_t;                                                  \
  inline static std::string sybTypeName() { return descr().name(); }        \
  inline std::string strVal() const { return descr().nameByVal(val()); }    \
  inline void strVal(std::string const& v) { val(descr().valByName(v)); }   \
  inline n& operator=(int v) {                                              \
        val(v);                                                             \
        return *this;                                                       \
  }                                                                         \
  template<typename T, typename FunT>                                       \
  inline static void gmap(T& v, FunT const& s) {                            \
    return s(*v);                                                           \
  }                                                                         \
  inline int const& operator*() const { return val_; }                      \
  inline n(int v):val_(v){}                                                 \
  inline n():val_(0){}                                                      \
  inline int& operator*() { return val_; }                                  \
  inline int val() const { return val_; }                                   \
  inline void val(int v) { val_ = v; }                                      \
  inline static qx::syb::EnumDescr descrFill() {                            \
    qx::syb::EnumDescr ret(#n);

/**
\brief Ends enum description block.
*/
#define END_ENUM_INFO return ret; }                                         \
  inline static qx::syb::EnumDescr const& descr() {                         \
    static qx::syb::EnumDescr ret = descrFill();                            \
    return ret;                                                             \
  }                                                                         \
private:                                                                    \
  int val_;

/**
\brief Declares an item of an enum

\param n Name of an item
*/
#define ENUM_ITEM(n) ret.Register(n,#n);

namespace qx {
  namespace syb {

    /**
    \brief Stores information about an enum

    In the current stage this is mapping between string and integer values
    */
    struct EnumDescr {
      inline explicit EnumDescr(std::string const& n):name_(n) {}

      /**
      \brief Adds new option to enum string<->int value map
      */
      inline void Register(int x, std::string const& n) {
        std::string tmp = n;
        boost::to_lower(tmp);
        valByName_[tmp] = x;
        nameByVal_[x] = n;
        vals_.push_back(x);
      }
      /**
      \brief Finds out integer value by string value
      */
      inline int valByName(std::string const& n) const {
        //TODO: optimize this (remove exception)
        
        std::string tmp = n;
        boost::to_lower(tmp);

        Loki::AssocVector<std::string,int>::const_iterator x = valByName_.find(tmp);
        if (x == valByName_.end()) {
          try {
            int v = boost::lexical_cast<int>(n);
            if(v < 0 || v >= valByName_.size())
              throw std::runtime_error("Enum " + name_ 
                + " doesn't have value " + n);
            return v;
          } catch(boost::bad_lexical_cast const&) {
            //TODO: lycia exception
            throw std::runtime_error("Enum " + name_ 
              + " doesn't have value " + n);
          }
        }
        return x->second;
      }
      /**
      \brief Finds out string value by integer value
      */
      inline std::string nameByVal(int x) const {
        Loki::AssocVector<int, std::string>::const_iterator n = nameByVal_.find(x);
        return n == nameByVal_.end() 
          ? boost::lexical_cast<std::string>(x) 
          : n->second;
      }
      /**
      \brief Returns name of enum's type
      */
      inline std::string const& name() const { return name_; }

      /**
      \brief Returns defined variants
      */
      inline std::vector<int> const& variants() const { return vals_; }
    private:
      std::string name_;
      Loki::AssocVector<std::string,int> valByName_;
      Loki::AssocVector<int,std::string> nameByVal_;
      std::vector<int> vals_;
    };

    BOOST_MPL_HAS_XXX_TRAIT_DEF(enum_tag_t)

    /**
    \brief Type trait that tests whether provided type is an enumerator
    */
    template<typename T, typename EnableT=void>
    struct IsEnum:
#ifdef BOOST_MPL_CFG_NO_HAS_XXX
      has_enum_tag_t<T,mpl::true_>
#else
      has_enum_tag_t<T>
#endif
    {};
/*
    template<typename T>
    struct IsPrimitive<T,typename boost::enable_if<IsEnum<T> >::type>:mpl::true_{};
*/
    template<typename U>
    struct StringRepr<U, typename boost::enable_if<IsEnum<U> >::type
    > : mpl::true_
    {
      static bool fromString(U& v, UnicodeString const& s) {
        std::string us;
        s.toUTF8String(us);
        v.strVal(us);
        return true;
      }
      static void toString(U const& v, UnicodeString& s) {
        s = UnicodeString::fromUTF8(v.strVal());
      }
    };

    template<typename U>
    struct StringRepr<boost::optional<U>, typename boost::enable_if<IsEnum<U> >::type
    > : mpl::true_
    {
      static bool fromString(boost::optional<U>& v, UnicodeString const& s) {
        if (s.isEmpty())
          return true;
        std::string us;
        s.toUTF8String(us);
        U vv;
        vv.strVal(us);
        v.reset(vv);
        return true;
      }
      static void toString(boost::optional<U> const& v, UnicodeString& s) {
        if (v)
          s = UnicodeString::fromUTF8(v->strVal());
      }
    };
  }
}

template<typename T>
inline typename boost::enable_if<qx::syb::IsEnum<T>,std::ostream>::type& 
   operator<<(std::ostream& f , T const& t) {
 return f << t.strVal();
}

template<typename T>
inline typename boost::enable_if<qx::syb::IsEnum<T>,std::istream>::type& 
    operator>>(std::istream& f,T& t) {
  std::string val;
  f >> val;
  t.strVal(val);
  return f;
}

template<typename T>
inline typename boost::enable_if<qx::syb::IsEnum<T>,bool>::type 
    operator==(T const& l, T const& r) {
  return *l == *r;
}

template<typename T>
inline typename boost::enable_if<qx::syb::IsEnum<T>,bool>::type 
    operator!=(T const& l, T const& r) {
  return *l != *r;
}

#endif 
