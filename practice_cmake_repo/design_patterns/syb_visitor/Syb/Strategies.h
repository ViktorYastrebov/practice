/**
\brief THIS DOESN'T WORK. 
just variois experimentations for strategic programming support


it requires SYB changes to work propertly

Too many option to consider.

Storage scheme here is quite ineffective, it performs many copies during 
creation, and it requires many applications of boost::function, error 
handling is done by means of exception, because SYB doesn't support other
way to stop traversal at the moment

The second can be resolved by using template argument as function, thus
compiler will be able to inline operator() for embedded strategy call.

There're many options for storagy policy, for example Strategies.cs is 
implemented by means of passing eta-expanded values.

*/
#ifndef SYB_FORALL_H
#define SYB_FORALL_H
#if 0

#include "Syb/Any.h"
#include "Syb/Syb.h"

#include <boost/function.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/any.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

namespace qx {
  namespace syb {

    namespace strategic {

      /**
      \brief Exception for stopping traversal

      \todo Syb needs to be reimplemented to support traversal stop without
      exception
      */
      struct Exit{};

      /// apply argument to all immediate children
      struct All {
        inline explicit All(dyn_fun_t const& f):fun_(f){}

        inline void operator()(Any const& v) const {
          v.run(fun_);
        }
      private:
        dyn_fun_t fun_;
      };


      /// failure inversion
      struct Inv {
        inline explicit Inv(dyn_fun_t const& f):fun_(f){}
        inline void operator()(Any const& v) const {
          try {
            fun_.action(v);
          } catch (Exit const&) {
            return;
          }
          throw Exit();
        }
      private:
        dyn_fun_t const& fun_;
      };

      /// apply argument to one kid
      struct One : Inv {
        inline explicit One(dyn_fun_t const& s):Inv(All(Inv(s))){};
      };

      /// always fail
      struct Fail {
        inline void operator()(Any const& v) const {
          throw Exit();
        }
      };
      
      /// always succeed
      struct Id {
        inline void operator()(Any const& v) const {
        }
      };
      
      /// perform strategies in sequence
      struct Sequence {
        inline Sequence(dyn_fun_t const& l, dyn_fun_t const& r)
          :left_(l),right_(r){}
        inline void operator()(Any const& v) const {
          left_->action(v);
          right_->action(v);
        }
      private:
        dyn_fun_t left_;
        dyn_fun_t right_;
      };

      /// attempt alternative strategies
      struct Choice {
        inline Choice(dyn_fun_t const& l, dyn_fun_t const& r)
          :left_(l),right_(r){}
        inline void operator()(Any const& v) const {
          try {
            left_(v);
          } catch(Exit const&) {
            right_(v);
          }
        }
      private:
        dyn_fun_t left_;
        dyn_fun_t right_;
      };

      /// recover from failure
      struct Try : Choice {
        inline explicit Try(dyn_fun_t const& s):Choice(s,Id()){}
      };

      /// keep applying s until it fails
      struct Repeat:Try {
        inline explicit Repeat(dyn_fun_t const& s):Try(Sequence(s,*this)){}
      };

      /// apply s in top-down fashion to all nodes
      struct TopDown : Sequence {
        inline explicit TopDown(dyn_fun_t const& s):Sequence(s,All(*this)){} 
      };

      /// apply s in bottom-up fashion to all nodes
      struct BottomUp : Sequence {
        inline explicit BottomUp(dyn_fun_t const& s):Sequence(All(*this),s){}
      };

      /// cutoff traversal below nodes where s succeeds
      struct StopTopDown : Choice {
        inline explicit StopTopDown(dyn_fun_t const& s):Choice(s,All(*this)){}
      };

      /// terminate traversal at first node where s succeeds (in bottom-up fashion)
      struct OnceTopDown : Choice {
        inline explicit OnceTopDown(dyn_fun_t const& s):Choice(s,One(*this)){}
      };

      /// terminate traversal at first node where s succeeds (in bottom-up fashion)
      struct OnceBottomUp : Choice {
        inline explicit OnceBottomUp(dyn_fun_t const& s):Choice(One(*this),s){}
      };

      /// outermost evaluation strategy
      struct OuterMost : Repeat {
        inline explicit OuterMost(dyn_fun_t const& s):Repeat(OnceTopDown(s)){}
      };

      /// innermost evaluation strategy
      struct InnerMost : Repeat {
        inline explicit InnerMost(dyn_fun_t const& s):Repeat(OnceBottomUp(s)){};
      };
    }
  }
}

#endif
#endif
