/**
\brief Implements a generic function for object's equality comparison
*/
#ifndef SYB_EQUALS_H
#define SYB_EQUALS_H

#if 0
#include "Syb/Zip.h"
#include "Syb/StlSyb.h"

#include "Syb/Trace.h"
#else 
#include "Zip.h"
#include "StlSyb.h"

#include "Trace.h"
#endif

namespace qx {
  namespace syb {

    namespace priv {
      template<typename U, typename EnableT = void>
      struct EqualsDisp;
    }

    /** \brief Generic function for comparing two objects for equality
    */
    struct Equals {
      inline explicit Equals(bool& r):res_(r){}
      template<typename T>
      inline void operator()(T const& l, T const& r) const {
        priv::EqualsDisp<typename UnAlias<T>::type>
          ::action(*this,unAlias(l),unAlias(r));
      }
      inline bool& res() const { return res_; }

      static const bool is_left_const = true;
      static const bool is_right_const = true;

    private:
      bool& res_;

      template<typename U, typename EnableT>
      friend struct priv::EqualsDisp;

    };

    namespace priv {
      //TODO: separate case for multiconstr (opt)
      template<typename U>
      struct EqualsDisp<U,typename boost::enable_if<IsMultiConstr<U> >::type> {
        inline static void action(Equals const& e, U const& l, U const& r) {
          TRACE_ENTER(Equals,">>poly compare:" << typeid(U).name());
          if (e.res()) {
            DataType<U> const& dt = dataType<U>();
            bool lc = dt.hasConstructor(l);
            bool rc = dt.hasConstructor(r);
            if (!lc && !rc) return;
            if (!lc || !rc) {
              e.res() = false;
              return;
            }
            ConstrInfo<U> c = dt.fromValue(l);
            if (c == dt.fromValue(r)) {
              polyZip(e,l,r,c);
            } else {
              e.res() = false;
            }
          }
          TRACE_EXIT(Equals,"<<poly compare:" << typeid(U).name() << "/" << e.res_)
        }
      };

      template<typename U, typename EnT>
      struct EqualsDisp {
        inline static void action(Equals const& e, U const& l, U const& r) {
          TRACE_ENTER(Equals,">>compare record:" << typeid(U).name());
          zip(e,l,r);
          TRACE_EXIT(Equals,"<< compare record:" << typeid(U).name() << "/" << e.res_)
        }
      };

      template<typename U>
      struct EqualsDisp<boost::shared_ptr<U>, 
        typename boost::disable_if<IsMultiConstr<boost::shared_ptr<U> > >::type>
      {
        inline static void action(Equals const& e, 
          boost::shared_ptr<U> const& l, 
          boost::shared_ptr<U> const& r
        ) {
          TRACE_ENTER(Equals,">>compare pointer:" << typeid(U).name());
          if (e.res()) {
            if (!l && !r) return;
            if (!l || !r) {
              e.res() = false;
              return;
            }
            zip(e,*l,*r);
          }
          TRACE_EXIT(Equals,"<< compare pointer:" << typeid(U).name() << "/" << e.res_)
        }
      };

      template<typename U>
      struct EqualsDisp<U,typename boost::enable_if<IsPrimitive<U> >::type>{
        inline static void action(Equals const& e, U const& l, U const& r) {
          //TRACE(Equals,"compare simple:" << typeid(U).name() << " \"" << l << "\" = \"" << r
          //                               << "\" (" << static_cast<int>(l == r) << ")")
            if (e.res_) 
              e.res_ = (l == r) != 0; //removing warning
        }
      };

      template<typename U>
      struct EqualsDisp<U,typename boost::enable_if<IsCont<U> >::type> {
        inline static void action(Equals const& e, U const& l, U const& r) {
          TRACE_ENTER(Equals,"compare array:" << typeid(U).name() 
            << "[" << l.size() << "] with [" << r.size() << "]/" << e.res_);
          if (e.res_) {
            if (l.size() != r.size()) {
              TRACE(Equals,"sizes are different: " 
                << l.size() << " /= " << r.size());
              e.res_ = false;
              return;
            }
            contZip(e,l,r);
          }
          TRACE_EXIT(Equals,"<< compare array:" << typeid(U).name() << "/" << e.res_)
        }
      };
    }

    /**
    \brief A helper for running Equals generic function
    */
    template<typename T>
    bool eq(T const& l, T const& r) {
      bool res = true;
      Equals eqf(res);
      eqf(unAlias(l),unAlias(r));
      return res;
    }
  }
}


#endif

