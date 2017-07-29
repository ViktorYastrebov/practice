/**
WARNING: the module doesn't work at the moment!!!
\brief The views are used if we can not get dirrect references for the value.

\todo various other types of setter/getters
\todo more helper functions
\todo type traits/various variants for object references/values transfer

*/
#ifndef SYB_VIEW_H
#define SYB_VIEW H

#include <boost/function.hpp>
#include <boost/type_traits.hpp>

namespace qx {
  namespace syb {

    /** 
    \brief Wrapper that can be used for running arbitrary function on 
      get/set generic data  on not const data.

    It usually shell not be used dirrectly. Only via corresponding
    \a view function.

    \tparam T type name that is actually will be traversed

    */
    template<typename T>
    struct View:TypeWrapper {

      typedef boost::function<T const& ()> get_fun_t;
      typedef boost::function<void 
        (typename boost::remove_const<T>::type &)> set_fun_t;

      /**
      \param getter The function that will be used for getting type for
        traversing
      \param getter The function that will be used for setting result value
        after traversing
      */
      inline View(get_fun_t const& getter,set_fun_t const& setter)
        :get_(getter),set_(setter){}
    private:
      get_fun_t get_;
      set_fun_t set_;

      template<typename U, typename FunT>
      inline friend void gmap(View<U> const* v, FunT const& s, Unit const&) 
      {
        U prm = v->get_();
        s(prm);
        v->set_(prm);
      }

    };

    /** 
    \brief Wrapper that can be used for running arbitrary function on
      get/set generic data on const data.

    It usually shell not be used dirrectly. Only via corresponding 
      \a view function.
    */
    template<typename T>
    struct CView:TypeWrapper {

      typedef boost::function<T const& ()> get_fun_t;

      inline explicit CView(get_fun_t const& g)
        :get_(g){}
      private:
        get_fun_t get_;

    };

    /**
    \brief Helper function for automatic initialization of templates
      parameters for \a View

    \param f    Source parameter that will be viewed using the View
    \param getter  Function that transforms source typed object to
      an object with a type that can be traversed
    \param setter  Function that transforms back traversable type to
      source object
    */
    template<typename ToT, typename FromT>
    inline View<ToT> view(
      FromT& f
      ,boost::function<ToT const& 
        (typename boost::remove_const<FromT>::type const&)> const& g
      ,boost::function<void (typename 
        boost::remove_const<FromT>::type&, ToT const&)> const& s
      )
    {
      return View<ToT>(boost::bind(g
        ,boost::cref(f)),boost::bind(s,boost::ref(f),_1));
    }

    /**
    \brief Helper function for automatic initialization of templates
      parameters for \a View

    \param f    Source parameter that will be viewed using the View
    \param getter  Function that transforms source typed object to
      an object with a type that can be traversed
    \param setter  Function that transforms back traversable type to
      source object
    */
    template<typename ToT, typename FromT>
    inline CView<ToT> view(
      FromT const& f
      ,boost::function<ToT const& 
        (typename boost::remove_const<FromT>::type const&)> const& g
      ,boost::function<void (typename
        boost::remove_const<FromT>::type&, ToT const&)> const& s
      )
    {
      return CView<ToT>(boost::bind(g,boost::cref(f)));
    }

    template<typename T, typename EnableT> struct TypeName<View<T>, EnableT> {
      inline static std::string value() {
        return TypeName<T>::value() + "Wrapper";
      }
    };

    template<typename T, typename EnableT> struct TypeName<CView<T>, EnableT> {
      inline static std::string value() {
        return TypeName<T>::value() + "Wrapper";
      }
    };
  }
}

#endif
