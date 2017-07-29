/**
\brief Generic function for displaying content of traversable object
  in readable form.

The easiest way to use it is to use special ostream manipulator.

For example:

std::cout << \a gshow(c);

there c is traversable object.

Now it's usable only for debug purposes, but it will be extended in 
future for more general tasks.

To overload behaviour for some type specialize template \a ShowVal.

\todo punctuation
\todo different languages
\todo different cases, plural/singular
*/
#ifndef SYB_SHOW_H
#define SYB_SHOW_H

#if 0
#include "Syb/Syb.h"
#include "Syb/SybAll.h"
#include "Syb/Traverse.h"
#else
#include "../Syb/Syb.h"
#include "../Syb/SybAll.h"
#include "../Syb/Traverse.h"
#endif
#include <iostream>

namespace qx {
  namespace syb {

    /**
    \brief Special template for overloading behaviour for various 
    user defined types.

    \tparam T
    \tparam EnableT

    */
    template<typename T, typename EnableT=void>
    struct ShowVal {
      /**
      \brief The function is called before traversing subchilds.

      \param out  The stream where the object should be output
      \param v  The object to output
      \return if return is false then subchilds are not traversed
      */
      inline static bool before(std::ostream& out, T const& v) {
        out << TypeName<T>::value() << " (";
        return true;
      };
      /**
      \brief The function is called after traversing all children.

      If before for the same instances returned false after will not
      be called.

      \param out  The stream where the object should be output
      \param v  The object to output
      */
      inline static void after(std::ostream& out, T const& v) {
        out << ") ";
      }
    };

    template<typename T>
    struct ShowVal<T,typename boost::enable_if<IsPrimitive<T> >::type> {
      inline static bool before(std::ostream& out, T const& v) {
        out << v << " ";
        return true;
      }
      inline static void after(std::ostream& out, T const& v) {}
    };

    /**
    \brief Actual function definition.
    */
    struct GShow:Contextual<GShow>{

      /**
      \param out o
      */
      inline explicit GShow(std::ostream& out):out_(out){}

      /**
      \brief The function from Guarded interface 
      */
      template<typename T>
      inline bool before(T const& v) const {
        return ShowVal<T>::before(out_,v);
      }

      /**
      \brief The function from Guarded interface 
      */
      template<typename T>
      inline void after(T const& v) const {
        ShowVal<T>::after(out_,v);
      }

      /**
      \brief The function from Contextual interface 

      \todo Just for debugging purposes, remove it
      */
      template<typename T, typename U>
      void context(T const& p, U const& c) const {
      }


      /**
      \brief Helper function for easier generic function call
      */
      template<typename T>
      inline static std::ostream& run(std::ostream& f, T const& v) {
        GShow fun(f);
        fun(v);
        return f;
      }
    private:
    /*
    To not deal with return value most generics just return's void,
    but maintain return value in state variable
    */
      std::ostream& out_;
    };

    /**
    \brief Stream manipulator for calling GShow.
    */
    template<typename T>
    struct GShowManip {
      inline explicit GShowManip(T& v):val_(v){}
      friend inline std::ostream& operator<<(std::ostream& f,GShowManip const& v){
        return GShow::run(f,v.val_);
      }
    private:
      T& val_;
    };

    /**
    \brief Helper function for creating manipulator's object.
    */
    template<typename T> 
    GShowManip<T> gshow(T& val) { return GShowManip<T>(val); }

  }
}

#endif
