/** 
\brief Traverse strategies.
\todo This is depricated by Strategy and it's going to be removed soon
*/
#ifndef SYB_TRAVERSE_H
#define SYB_TRAVERSE_H

#if 0
#include "Syb/Poly.h"
#include "Syb/Syb.h"
#include "Syb/Tags.h"
#include "Syb/RefUtils.h"
#else
#include "Poly.h"
#include "Syb.h"
#include "Tags.h"
#include "RefUtils.h"
#endif
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/or.hpp>

namespace qx {
  namespace syb {

    /**
    \brief Guarded traversal

    Calls method \a before before traversing children if it retuns false
    children are not traversed.

    Call method \a after after traversing children if \a before returned
    false \a after will not be called.

    \tparam FunT
    \tparam EnableT
    */
    template<typename FunT,typename EnableT=void>
    struct Guarded:Visitor<FunT> {
#ifdef NOT_INLINES
      template<typename T>
      inline void operator()(T& v) const {
          notInline(*this,v);
      }
      template<typename T>
      inline void runInline(T& v) const {
        if (Visitor<FunT>::fun().before(v)){
          gmap(v,Visitor<FunT>::fun(),Unit()); /* OK */
          Visitor<FunT>::fun().after(v);
        }
      }
#else
      template<typename T>
      inline void operator()(T& v) const {
        if (Visitor<FunT>::fun().before(v)){
          gmap(v,Visitor<FunT>::fun(),Unit()); /* OK */
          Visitor<FunT>::fun().after(v);
        }
      }
#endif

    };

    /**
    \brief Generic function's parameter that is used for modeling
    implicit stack.

    If the parameter is set to true the function object will be copied 
    for each level of traversal. Thus implicit stack will be simulated.
    I.e. all state variable of a function will be returned back after
    some level is traversed.

    \tparam FunT
    \tparam EnableT

    \todo Now works only for Guarded, add the same for Contextual
    */
    template<typename FunT,typename EnableT=void>
    struct ImplicitStack:mpl::false_{};

    template<typename FunT>
    struct Guarded<FunT,typename boost::enable_if<ImplicitStack<FunT> >::type>
      :Visitor<FunT> 
    {
      template<typename T>
      inline void operator()(T& v) const {
        const FunT& f = Visitor<FunT>::fun();
        if (f.before(v)){
          gmap(v,f,Unit());
          f.after(v);
        }
      }
    };


    /** 
    \brief Adds ability to track parent element for a current element

    Contextual is also guarded. It's done only for optimisation purposes.
    on each child method \a context is called where first parameter is 
    parent and the second is current object.

    Thus only one level context is modelled. Technically it's easy to
    implement arbitrary depth content, but we need more use cases to do 
    this.
    */
    template<typename FunT>
    struct Contextual:Guarded<FunT> {

      template<typename T>
      inline void operator()(T& v) const {
        const FunT& f = Guarded<FunT>::fun();
        if (f.before(v)) {
          gmap(v,Child<T>(f,v),Unit());
          f.after(v);
        }
      }

      template<typename T>
      struct Child:Visitor<Child<T> > {
        inline explicit Child(FunT const& t, T& v)
          :this_(t),parent_(v){}

        template<typename U>
        inline void operator()(U& v) const {
          this_.context(parent_,v);
          this_(v);
        }
      private:
        FunT const& this_;
        T& parent_;
      };
    };
  }
}

#endif
