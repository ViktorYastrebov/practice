/**
\brief Implements a generic function for object's copying
*/
#ifndef SYB_CLONE_H
#define SYB_CLONE_H

#include <boost/any.hpp>
#include <boost/unordered_map.hpp>

#if 0
#include "Syb/Zip.h"
#include "Syb/Syb.h"
#include "Syb/MultiConstr.h"
#include "Syb/StlSyb.h"
#include "Syb/TypeAlias.h"
#include "Syb/Trace.h"
#include "Syb/Any.h"
#else
#include "Zip.h"
#include "Syb.h"
#include "MultiConstr.h"
#include "StlSyb.h"
#include "TypeAlias.h"
#include "Trace.h"
#include "Any.h"
#endif

namespace qx {
  namespace syb {

    /**
    \brief Cloning behavour dispatcher for different kinds of values
    */
    template<typename TagT, typename U, typename EnalbleT=void>
    struct CloneHook;

    struct CloneCtx {
      template<typename U>
      inline void addRef(boost::shared_ptr<U> const& v) {
        dict_.insert(std::pair<cfg::key_t, boost::any>(v->id(), boost::any(v)));
      }

      template<typename U>
      inline bool lookup(cfg::key_t const& k, boost::shared_ptr<U>& v) {
        boost::unordered_map<cfg::key_t,boost::any>::const_iterator i = dict_.find(k);
        if (i == dict_.end())
          return false;
        v = boost::any_cast<boost::shared_ptr<U> >(i->second);
        return true;
      }

    private:
      boost::unordered_map<cfg::key_t,boost::any> dict_;
    };

    /**
    \brief Generic functions for copying whole object structure into
    another object
    */
    template<typename TagT>
    struct Clone:Visitor<Clone<TagT> > {
      inline explicit Clone(CloneCtx& x):ctx_(x) {
      }

      template<typename T>
      void operator()(T const& i, T& o) const {
        TRACE_ENTER(Clone<TagT>,">> clone:" << typeid(T).name());
        CloneHook<TagT,typename UnAlias<T>::type>
          ::action(ctx_,unAlias(i),unAlias(o));
        TRACE_EXIT(Clone<TagT>,"<< clone:" << typeid(T).name());
      }
      // NOTE: unfortunately this is required because of dynamic nature of Zip
      template<typename T>
      void operator()(T const& t, T const& v) const {
        TRACE_ENTER(Clone<TagT>,">> clone(const):" << typeid(T).name());
        zip(*this,unAlias(t),unAlias(v)); // probably we'll have some mutable subfields
        TRACE_EXIT(Clone<TagT>,"<< clone (const):" << typeid(T).name());
      }

      static const bool is_left_const = true;
      static const bool is_right_const = false;

    private:
      CloneCtx& ctx_;
    };

    /**
    \brief A helper for running clone generic function
    */
    template<typename TagT, typename T, typename U>
    typename boost::enable_if<boost::is_same<
      T,typename boost::remove_const<U>::type
    > >::type clone(T const& in, U& out) {
      CloneCtx ctx;
      Clone<TagT> fun(ctx);
      fun(unAlias(in),unAlias(out));
    }

    template<typename TagT, typename U, typename EnalbleT=void>
    struct CloneHookConstr {
      inline static void action(CloneCtx& x, U const& i, U& o) {
        TRACE(Clone<TagT>,"Clone: default");
        zip(Clone<TagT>(x),i,o);
      }
    };

    template<typename TagT, typename U>
    struct CloneHookConstr<TagT,boost::shared_ptr<U>,
      typename boost::disable_if<IsMultiConstr<boost::shared_ptr<U> > >::type>
    {
      inline static void action(CloneCtx& x,
        boost::shared_ptr<U> const& i, boost::shared_ptr<U>& o)
      {
        TRACE(Clone<TagT>,"Clone: default");
        if (!i && !o)
          return;
        if (!o)
          o.reset(new U);
        zip(Clone<TagT>(x),*i,*o);
      }
    };

    template<typename TagT>
    struct CloneHookConstr<TagT,Any>
    {
      /// Any is a kind of weak reference type so cloning just copies
      /// the reference
      inline static void action(CloneCtx& x, Any const& i, Any& o)
      {
        TRACE(Clone<TagT>,"Clone: any");
        o = i;
      }
    };

    template<typename TagT, typename U>
    struct CloneHookConstr<TagT,U,
      typename boost::enable_if<IsMultiConstr<U> >::type>
    {
      inline static void action(CloneCtx& x, U const& i, U& o) {
        TRACE(Clone<TagT>,"Clone: poly");
        DataType<U> const& d = dataType<U>();
        if (!d.hasConstructor(i) && !d.hasConstructor(o))
          return;
        if (!d.hasConstructor(i)) {
          o = i;
          return;
        }
        if (!d.hasConstructor(o) ||
          !(d.fromValue(o) == d.fromValue(i)))
        {
          d.fromValue(i).build(o);
        }
        zip(Clone<TagT>(x),i,o);
      }
    };

    /**
    \brief A helpers for adjusting default cloning behavior
    */
    template<typename TagT, typename U, typename EnalbleT=void>
    struct CloneHookComplex:CloneHookConstr<TagT,U> {};

    template<typename TagT, typename U, typename EnalbleT>
    struct CloneHook:CloneHookComplex<TagT,U>{};

    template<typename TagT, typename U>
    struct CloneHook<TagT, U
      ,typename boost::enable_if<IsPrimitive<U> >::type
    > {
      inline static void action(CloneCtx&, U const& i, U& o) {
        TRACE(Clone<TagT>,"Clone: primitive");
        o = i;
      }
    };

    template<typename TagT, typename U>
    struct CloneHook<TagT, boost::optional<U>
      ,typename boost::enable_if<IsPrimitive<U> >::type
    > {
      inline static void action(
        CloneCtx&, boost::optional<U> const& i, boost::optional<U>& o)
      {
        TRACE(Clone<TagT>,"Clone: primitive");
        o = i;
      }
    };

    template<typename TagT, typename U>
    struct CloneHook<TagT,U,typename boost::enable_if<IsCont<U> >::type> {
      typedef typename U::const_iterator iter_t;
      struct Run {
        inline Run(CloneCtx& x, iter_t& i):ctx_(x),iter_(i){}
        inline void operator()(typename U::value_type& v) const {
          Clone<TagT> f(ctx_);
          f(*iter_,v);
          iter_++;
        }
      private:
        CloneCtx& ctx_;
        iter_t& iter_;
      };
      inline static void action(CloneCtx& x, U const& i, U& o) {
        TRACE(Clone<TagT>,"Clone: cont");
        // o.swap(U(i.size()));
        o.resize(i.size());
        // zip(Clone<TagT>(x),i,o);
        typename U::const_iterator ii = i.begin();
        std::for_each(o.begin(),o.end(),Run(x,ii ));
      }
    };
  }
}

#endif
