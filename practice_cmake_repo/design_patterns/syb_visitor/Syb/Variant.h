/**
\brief The module implements SYB support for boost::variant.

boost::variant is treated as MultiConstr type.
*/
#ifndef SYB_VARIANT_H
#define SYB_VARIANT_H

#include "Syb/MultiConstr.h"
#include "Syb/Syb.h"
#include <boost/variant.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/ref.hpp>
#include <map>

namespace qx {
  namespace syb {
    template<typename T,BOOST_VARIANT_ENUM_PARAMS(typename U),typename EnT>
    struct IsMultiConstr<T,boost::variant<BOOST_VARIANT_ENUM_PARAMS(U) >,EnT>
      :mpl::true_{};

    template<BOOST_VARIANT_ENUM_PARAMS(typename U), typename EnT>
    struct ConstrInfo<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>,EnT> :
      ConstrInfoSpec<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> >
    {
      typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> data_t;
      typedef typename boost::make_variant_over<typename mpl::transform<
        typename data_t::types, mpl::quote1<Empty>
      >::type>::type data_sample_t;

      template<typename T>
      inline explicit ConstrInfo(Empty<T> const& v)
        :name_(TypeName<T>::value())
        ,sample_(v)
        {}

      inline void build(data_t& v) const {
        // or we can keep sample as already built value
        // but this will call constructor only once 
        boost::apply_visitor(RedirBuild(v),sample_);
      }

      inline std::string name() const {
        return name_;
      }
      template<typename FtrT>
      inline void accept(FtrT const& f) const {
          boost::apply_visitor(RedirAccept<FtrT>(f),sample_);
      }
      inline bool operator==(ConstrInfo const& v) const {
        return sample_.which() == v.sample_.which();
      }

      std::string xmlName() const {
        return name_;
      }
    private:
    /*
      inline ConstrInfo(ConstrInfo const&) {}
      inline ConstrInfo& operator=(ConstrInfo const&) {}
   */
    //  bool isEmpty_;
      struct RedirBuild:boost::static_visitor<> {
        inline explicit RedirBuild(data_t& d):data_(d){}
        template<typename T>
        inline void operator()(Empty<T> const& v) const {
          data_ = T();
        }
      private:
        data_t& data_;
      };

      template<typename FtrT>
      struct RedirAccept:boost::static_visitor<>{
        inline explicit RedirAccept(FtrT const& f):ftr_(f){}
        template<typename T>
        inline void operator()(Empty<T> const& v) const {
            ftr_.visit(v);
        }
      private:
        FtrT const& ftr_;
      };
      std::string name_;
      data_sample_t sample_;
    };

    template<BOOST_VARIANT_ENUM_PARAMS(typename U),typename EnT>
    struct DataType<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U) >,EnT>
      : DataTypeSpec<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U) > >
    {

      typedef boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> data_t;
      typedef typename data_t::types types_t;
      typedef ConstrInfo<data_t> constr_t;

      inline DataType() {
        typedef typename mpl::fold<types_t
          ,StopFillIn
          ,mpl::quote2<FillIn>
        >::type run_t;
        constrs_.reserve(mpl::size<types_t>::value);
        run_t::action(*this);
      }

      inline size_t numOfConstrs() const { 
        return mpl::size<typename data_t::types>::value; 
      }
      inline constr_t const& operator[](size_t x) const {
        assert(x < numOfConstrs());
        return constrs_[x];
      }
      inline constr_t const* byName(std::string const& name) const {
        typename std::map<std::string,constr_t const*>::const_iterator i 
          = constrsByName_.find(name);
        if (i == constrsByName_.end()) 
          return NULL;
        return i->second;
      }
      inline bool hasConstructor(data_t const& x) const { 
        return x.type() != typeid(Unit); //TODO: change this
      }
      inline constr_t const& fromValue(data_t const& x) const {
        return constrs_[x.which()];
      }
      inline constr_t const* byXmlName(std::string const& name) const {
        typename std::map<std::string,constr_t const*>::const_iterator i 
          = constrsByName_.find(name);
        if (i == constrsByName_.end()) 
          return NULL;
        return i->second;
      }

    private:
      std::vector<constr_t> constrs_;
      std::map<std::string,constr_t const*> 
        constrsByName_;

      template<typename U, typename NxtT>
      friend struct FillIn;

      struct StopFillIn { inline static void action(DataType<data_t>& t) {} };

      template<typename NxtT,typename U>
      struct FillIn {
        inline static void action(DataType<data_t>& t) {
          NxtT::action(t);
          t.constrs_.push_back(ConstrInfo<data_t>(Empty<U>()));
          t.constrsByName_.insert(std::make_pair(
            t.constrs_.back().name(),
            &t.constrs_.back()
            ));
        }
      };
    };


    template<typename DataT, typename FunT> 
    struct VariantGmap : boost::static_visitor<> {
      inline explicit VariantGmap(FunT const& f):fun_(f){}
      template<typename T>
      inline void operator()(T& v) const {
        fun_.polyVisit(v);
      }
    private:
      FunT const& fun_;
    };

    template<BOOST_VARIANT_ENUM_PARAMS(typename U)>
    struct TypeName<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> > {
      inline static std::string const& value() { 
        static std::string ret = "Variant";
        return ret;
      }
    };
/*
    template<BOOST_VARIANT_ENUM_PARAMS(typename U), 
      typename FunT, typename EnT>
    void gmap(boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>& v, 
      FunT const& f,EnT const&) 
    {
      boost::apply_visitor(
        VariantGmap<
          boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>,FunT>(f),v);
    }
    template<BOOST_VARIANT_ENUM_PARAMS(typename U), 
      typename FunT, typename EnT>
    void gmap(boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> const& v, 
      FunT const& f,EnT const&) 
    {
      boost::apply_visitor(
        VariantGmap<
          boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>,FunT>(f),v);
    }
*/

    template<BOOST_VARIANT_ENUM_PARAMS(typename U), 
      typename FunT, typename EnT>
    struct GmapRedir<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>, FunT, EnT> {
      inline static void action(boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>& v, FunT const& f) {
        boost::apply_visitor(
          VariantGmap<
            boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>,FunT>(f),v);
      }
    };

    template<BOOST_VARIANT_ENUM_PARAMS(typename U), 
      typename FunT, typename EnT>
    struct GmapRedir<boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> const, FunT, EnT> {
      inline static void action(boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> const& v, FunT const& f) {
        boost::apply_visitor(
          VariantGmap<
            boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>,FunT>(f),v);
      }
    };

    template<BOOST_VARIANT_ENUM_PARAMS(typename U), typename ChT>
    inline ChT const& downCast(
        boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> const& v, 
        Empty<ChT> const&
      ) {
      return boost::get<ChT>(v);
    }

    template<BOOST_VARIANT_ENUM_PARAMS(typename U), typename ChT>
    inline ChT& downCast(
        boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>& v, 
        Empty<ChT> const&
        ) {
      return boost::get<ChT>(v);
    }

    /*
    void gmap(boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>& v, 
      FunT const& f,EnT const&) 
    {
      boost::apply_visitor(
        VariantGmap<
          boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>,FunT>(f),v);
    }
    template<BOOST_VARIANT_ENUM_PARAMS(typename U), 
      typename FunT, typename EnT>
    void gmap(boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)> const& v, 
      FunT const& f,EnT const&) 
    {
      boost::apply_visitor(
        VariantGmap<
          boost::variant<BOOST_VARIANT_ENUM_PARAMS(U)>,FunT>(f),v);
    }
*/    

  }
}

#endif
