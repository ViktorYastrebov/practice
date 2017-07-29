/**
\brief Provides functionality for converting any C++ functor with one argument
to generic function

Unlike Syb generic functions it uses dynamic type information for traversing, 
if you need a better perfomance it's better to create own generic function 
from scratch

\todo Probably it worth considering changin boost::function to template arguments
for giving compiler a chance to inline function's call
*/
#ifndef SYB_FORALL_H
#define SYB_FORALL_H

#if 0
#include "Syb/Any.h"
#include "Syb/Syb.h"
#else
#include "Any.h"
#include "Syb.h"
#endif

#include <boost/function.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/any.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

namespace qx {
  namespace syb {
      /**
      \brief The same is also possible.

      This is actually equivalent to TopDown from strategies, and 
      it will be replaced with TopDown after strategies is stabilized
      */
    struct ForEach : DynFun {
        inline explicit ForEach(dyn_fun_t const& f):fun_(f){}
        void action(Any const& v) const;
      private:
        dyn_fun_t fun_;
      };

      template<typename T>
      void forEach(dyn_fun_t const& f, T& v) {
        ForEach fun(f);
        Any av = toAny(v);
        av.run<dyn_fun_t const>(&fun);
      }
  }
}

#endif
