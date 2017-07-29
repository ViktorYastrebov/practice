/** 
\brief Generic function for generating random test data

For generating any object use any variant of \a generate function.

To use it you can either run
\code
  SomeClass c = generate<SomeClass>();
\endcode

or if you need to avoid copy constructor invocation

\code
  SomeClass c;
  generate(c);
\endcode

For overloading default behaviour specialize template \a Arbitrary

\todo rewrite data generators to boost::random
\todo add ability to define distribution of random data generators
\todo first class generators

*/
#ifndef SYB_GENERATE_H
#define SYB_GENERATE_H

#if 0
#include "Syb/Traverse.h"
#include "Syb/StlSyb.h"
#include "Syb/Enum.h"
#include "Syb/Any.h"
#include "Syb/Trace.h"
#else
#include "Traverse.h"
#include "StlSyb.h"
#include "Enum.h"
#include "Any.h"
#include "Trace.h"
#endif
#include <unicode/unistr.h>

namespace qx {
  namespace syb {

    std::string guid();

    template <typename T, typename TagT, typename EnableT=void>
    struct Arbitrary;

    /**
    \brief Generic function for generating random data

    The function mantains implicit stack (\see ImplicitStack)

    \tparam TagT Defines different versions for overloading data
    */
    template<typename TagT>
    struct Generate:Visitor<Generate<TagT> > {

      /**
      \param size Size of output data
      */
      inline explicit Generate(int size = 10):size_(size){}

      /**

      */
      template<typename T>
      inline void operator()(T& v) const {
        Arbitrary<T,TagT>::action(size_,v);
      }

    private:
      int size_;
    };

    /** 
    \brief Helper function, returns int value in [\a min, \a max] range.

    \param min Minimal bound
    \param max Maximum bound

    \todo The function uses standard rand function, we need to rewrite 
    to boost::random
    */
    unsigned int genUInt(unsigned int min, unsigned int max);
    bool genBool();

    /** 
    \brief The template for overloading default behaviour

    \tparam T    Type what is under construction
    \tparam TagT  Optional tag needed for keeping several version of overloading
    \tparam EnableT  Helper parameter for overloading with \c boost::enable_if
    */
    template<typename T, typename TagT, typename EnableT>
    struct Arbitrary:NotDefined {
      inline static void action(int s, T& val) {
        TRACE_SCOPE(Generate<TagT>,"Arbitrary default", 
          s << "/" << TypeName<T>::value());
        gmap(val,Generate<TagT>(s),Unit());
      }
      inline static void action(int s, T const& val) {
        TRACE_SCOPE(Generate<TagT>,"Arbitrary default", 
          s << "/" << TypeName<T>::value());
        gmap(val,Generate<TagT>(s),Unit());
      }
    };

    /** 
    \brief \a Arbitrary for STL containers

    \tparam ConT  Container's type
    \tparam TagT  Optional tag needed for keeping several version of overloading
    */
    template<typename ConT,typename TagT>
    struct ArbitraryCont {
      inline static void action(int s, ConT& ret) {
        TRACE_SCOPE(Generate<TagT>,"ArbitraryContainer",
          s << "/" << TypeName<ConT>::value());
        int sz = genUInt(s >> 1,s<=0?1:s << 1);
        TRACE(Generate<TagT>,"Size: " << sz);
        ret = ConT(sz);
        if (s > 0)
          gmap(ret,Generate<TagT>(s - 1),Unit());
      }
    };

    template<typename ContT,typename TagT>
    struct Arbitrary<ContT,TagT,
      typename boost::enable_if<IsCont<ContT> >::type
    >:ArbitraryCont<ContT,TagT>{};

    
    /**
    \brief Helper function for \see Generate function invocation

    \tparam T   Type of an generated object
    \tparam TagT Defines different versions for overloading data
    */
    template<typename T, typename TagT>
    inline T generate(int size=10) {
      Generate<TagT> gen(size);
      T ret;
      gen(ret);
      return ret;
    }

    /**
    \brief Helper function for \see Generate function invocation

    \tparam T   Type of an generated object
    \tparam TagT Defines different versions for overloading data
    */
    template<typename T>
    inline T generate(int size=10) {
      Generate<void> gen(size);
      T ret;
      gen(ret);
      return ret;
    }

    /**
    \brief Helper function for \see Generate function invocation

    \tparam T   Type of an generated object
    \tparam TagT Defines different versions for overloading data
    */
    template<typename T, typename TagT>
    inline void generate(T& ret, int size = 10
      , Empty<TagT> const& v=Empty<TagT>())
    {
      Generate<TagT> gen(size);
      gen(ret);
    }

    /**
    \brief Helper function for \see Generate function invocation

    \tparam T   Type of an generated object
    \tparam TagT Defines different versions for overloading data
    */
    template<typename T>
    inline void generate(T& ret, int size = 10)
    {
      Generate<void> gen(size);
      gen(ret);
    }

    /** 
    \brief \a Arbitrary specialization helper for generating values from 
    a set of predefined values

    For example:
    \code
      template<typename TagT>
      struct Arbitrary<Address,TagT>:OneOf<Address,TagT> {
        inline static void variants() {
          reg(Address("Kharkov"));
          reg(Address("London"));
          reg(Address("Kiev"));
          reg(Address("Southampton"));
          reg(Address("Moscow"));
          reg(Address("Springfield"));
        }
      };
    \endcode

    \tparam T Type of a generated object
    */
    template<typename T, typename TagT=void>
    struct OneOf {
      /**
      \brief Actual type of generated value, i.e. cleaned from aliases 
      or any other manipulators
      */
      typedef T clean_t;

      /**
      \brief Registers possible return value
      */
      inline static void reg(T const& v) { opts().push_back(v); }
      
      inline static std::vector<T>& opts() { 
        static std::vector<T> opts = std::vector<T>();
        return opts;
      }                

      /**
      \brief Overloaded version of \a Arbitrary::operator()
      */
      inline static void action(size_t s, T& val) {
        TRACE(Generate<TagT>,"Arbitrary OneOf: " 
          << s << "/" << TypeName<T>::value());
        if (opts().empty())
          Arbitrary<T,TagT>::variants();
        val = opts()[genUInt(0,static_cast<unsigned int>(opts().size() - 1))];
      }
    };

    /**
    \tparam T Type of a generated object
    */
    template<typename T, typename TagT=void>
    struct GetUniqName {
      /**
      \brief Registers return value
      */
      inline static void reg(UnicodeString const& v) { opts()[v] = 0; }
      
      inline static std::map<UnicodeString, int>& opts() { 
        static std::map<UnicodeString, int> opts = std::map<UnicodeString, int>();
        return opts;
      }                
      
      // TODO: reimplement with OneOf
      /**
      \brief Overloaded version of \a Arbitrary::operator()
      */
      inline static void action(size_t s, T& val) {
        TRACE(Generate<TagT>,"Arbitrary GetUniqName: " 
          << s << "/" << TypeName<T>::value());
        if (opts().empty())
          Arbitrary<T,TagT>::variants();

        int rand = genUInt(0,static_cast<unsigned int>(opts().size() - 1));
        std::map<UnicodeString, int>::iterator i = opts().begin();
        for(i; i != opts().end() && rand; i++, rand--);
        i->second++;
        UnicodeString ret;
        ret = i->first;
        ret += UnicodeString::fromUTF8((boost::lexical_cast<std::string>(i->second)).c_str());
        val = ret;
      }
    };

    template<typename T, typename TagT, typename EnableT=void>
    struct PolyArbitrary {
      inline static void action(int size, T const& v) {
        // TODO: empty types can be also random
        //TRACE_SCOPE(Generate<TagT>,"Arbitrary polytype", 
        //  s << "/" << TypeName<T>::value());
        if (size > 0) {
          DataType<typename UnAlias<T>::type> const& dt 
            = dataType<typename UnAlias<T>::type>();
          unsigned int s = 
            genUInt(0,static_cast<unsigned int>(dt.numOfConstrs()-1));
          dt[s].build(unAlias(v));
          TRACE(Generate<TagT>,"Constructor: " << dt[s].name());
          gmap(unAlias(v),Generate<TagT>(size - 1),Unit());
        }
      }
      inline static void action(int size, T& v) {
        TRACE_SCOPE(Generate<TagT>,"Arbitrary polytype", 
          size << "/" << TypeName<T>::value());
        if (size > 0) {
          DataType<typename UnAlias<T>::type> const& dt 
            = dataType<typename UnAlias<T>::type>();
          unsigned int s = 
            genUInt(0,static_cast<unsigned int>(dt.numOfConstrs()-1));
          dt[s].dynBuild(toAny(v));
          TRACE(Generate<TagT>,"Constructor: " << dt[s].getName());
          gmap(unAlias(v),Generate<TagT>(size - 1),Unit());
        }
      }
    };

    template<typename T,typename TagT>
    struct Arbitrary<T,TagT,typename boost::enable_if<mpl::and_<
          NotOpt<T>,
          IsMultiConstr<T>
        >  >::type>
    :PolyArbitrary<T,TagT>
    {
    };

    template<typename TagT, typename EnableT>
    struct Arbitrary<std::string,TagT,EnableT> {
      inline static void action(int size, std::string& val) {
        TRACE(Generate<TagT>,"Arbitrary unicdode string");
        val = guid();
      }
    };

    template<typename TagT, typename EnableT>
    struct Arbitrary<UnicodeString,TagT,EnableT> {
      inline static void action(int size, UnicodeString& val) {
        TRACE(Generate<TagT>,"Arbitrary string");
        val = UnicodeString::fromUTF8(guid());
      }
    };

    template<typename TagT, typename T>
    struct Arbitrary<boost::optional<T>,TagT> {
      inline static void action(int size, boost::optional<T>& val) {
        // poor man distribution simulation
        int v = genUInt(0,5);
        TRACE_SCOPE(Generate<TagT>,"Arbitrary optional", 
          size << "/" << TypeName<T>::value() << (v > 1 ? "{inited}" : "{empty}"));
        if (v > 1) {
          val = T();
          generate(*val,size);
        }
      }
    };

    template<typename TagT, typename EnableT>
    struct Arbitrary<float,TagT,EnableT> {
      inline static void action(int s, float& val) {
        val = static_cast<float>(rand());
        TRACE(Generate<TagT>,"Arbitrary string: " << val);
      }
    };

    template<typename TagT, typename EnableT>
    struct Arbitrary<int,TagT,EnableT> {
      inline static void action(int s, int& val) {
        val = rand();
        TRACE(Generate<TagT>,"Arbitrary int: " << val);
      }
    };

    template<typename TagT, typename EnableT>
    struct Arbitrary<unsigned int,TagT,EnableT> {
      inline static void action(int s, unsigned int& val) {
        val = rand();
        TRACE(Generate<TagT>,"Arbitrary unsigned int: " << val);
      }
    };

    template<typename TagT, typename EnableT>
    struct Arbitrary<bool,TagT,EnableT> {
      inline static void action(int s, bool& val) {
        val = genBool();
        TRACE(Generate<TagT>,"Arbitrary bool: " << val);
      }
    };

    template<typename T, typename TagT>
    struct Arbitrary<T,TagT,typename boost::enable_if<IsEnum<T> >::type> {
      inline static void action(int s, T& v) {
        std::vector<int> const d = T::descr().variants();
        TRACE(Generate<TagT>,"Arbitrary Enum: " 
          << s << "/" << TypeName<T>::value());
        v = d[genUInt(0,static_cast<unsigned int>(d.size() - 1))];
      }
    };
  }
}

template<typename TagT>
struct TraceName<qx::syb::Generate<TagT> > {
  inline static std::string const& value() {
    static std::string ret = "generate:";
    return ret;
  }
};

#endif
