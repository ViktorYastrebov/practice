/**
\brief Small types used for meta-funcitons to adjust default behaviours
*/
#ifndef SYB_TAGS_H
#define SYB_TAGS_H

#include <boost/type_traits.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/if.hpp>

namespace qx {
  namespace syb {

    namespace mpl = boost::mpl;

    /**
    \brief Transform any type to empty type for functions parameters
      overloading.
    */
    template<typename T>
    struct Empty {
      typedef Empty type;
    };

    /**
    \brief meta-function that simply returns its parameter.
    */
    template<typename T>
    struct Ret {
      typedef T type;
    };

    /**
    \brief void type substitute.

    This will be removed after void functions will be detected 
      with type traits.
    */
    struct Unit {
      inline Unit(){}
      inline operator bool() const { return true; }
    };

    /**
    \brief Marks something as not defined.
    */
    struct NotDefined{};

    /**
    \brief Returns unique integer identifier for an entity (if defined)
    */
    template<typename T>
    struct EntId : NotDefined, mpl::integral_c<size_t, 0> {
    };

    /**
    \brief Meta predicate for checking \a T is not maked as NotDefined.

    \tparam T type to check.
    */
    template<typename T, typename EnableT=void>
    struct IsDefined:mpl::not_<boost::is_base_of<NotDefined,T> >{};

    /**
    \brief The type's purpose is to adjust behaviour of generic function, and
    it doesn't actually contain any information.
    */
    struct TypeWrapper{};

    /**
    \brief Meta predicate for TypeWrapper tag
    */
    template<typename T, typename EnableT=void>
    struct IsTypeWrapper:boost::is_base_of<TypeWrapper,T>{};

    /**
    \brief Meta-function that returns output value from a Spine 

    \todo Use boost function to get result type.
    \todo Use traits and void instead of Unit
    \todo Implement constraints about result type
    */
    template<typename FunT,typename ArgT,typename EnableT=void>
    struct ResultType:Ret<Unit>{
    };

    /**
    \brief Returns its second argument with const qualifier if 
    its first argument has it
    */
    template<typename T, typename V>
    struct SetConst:mpl::if_<boost::is_const<T>,
      typename boost::add_const<V>::type,
      typename boost::remove_const<V>::type
    >{};
  }
}

#endif
