/**
\brief Syb instances for STL containers

\todo Implement full set of containers
*/
#ifndef SYB_STLSYB_H
#define SYB_STLSYB_H

#include "Any.h"
//#include "Syb/MultiConstr.h"
#include "MultiConstr.h"

#include <list>
#include <vector>
#include <map>
#include <numeric>
#include <boost/call_traits.hpp>

namespace qx {
  namespace syb {


    /**
    \brief Meta predicate for testing if the type is STL container
    */
    template<typename ContT,typename EnableT=void>
    struct IsCont:mpl::false_{};


    //TODO: remove this!!!
    template<typename ContT,typename EnableT=void>
    struct IsMap:mpl::false_{};

    template<typename T,typename EnableT>
    struct IsCont<std::list<T>, EnableT>:mpl::true_{};

    template<typename T,typename EnableT>
    struct IsCont<std::vector<T>, EnableT>:mpl::true_{};

    //template<typename T,typename EnableT>
    //struct IsCont<HistList<T>, EnableT>:mpl::true_{};

    //template<typename K, typename V, typename C>
    //struct IsCont<std::map<K,V,C> >:mpl::true_{};

    //TODO:: remove this!!
    template<typename K, typename V, typename C, typename EnableT>
    struct IsMap<std::map<K,V,C>, EnableT>:mpl::true_{};
    template<typename K, typename V, typename C, typename EnableT>
    struct IsMap<std::multimap<K,V,C>, EnableT>:mpl::true_{};

    template<typename K, typename V, typename C, typename EnableT>
    struct IsCont<std::map<K, V, C>, EnableT >:mpl::true_{};

    template<typename K, typename V, typename C, typename EnableT>
    struct IsCont<std::multimap<K, V, C>, EnableT >:mpl::true_{};

    /**
    \brief Emulator of Cons lists.

    Cons list can be seen as classic C list where several fields along with pointer
    to next element exists.
    */

   /* template<typename ContT, typename EnableT = void>
    struct ConsListView {
      inline explicit ConsListView(ContT& c){}

      inline void insert() const {}
    };*/

//    template<typename ContT>
//    struct ConsListView<ContT, typename boost::enable_if< IsCont<ContT> >::type> {

    template<typename ContT /*, typename EnableT = void*/>
    struct ConsListView {

      typedef typename mpl::if_<boost::is_const<ContT>
        ,typename ContT::const_iterator
        ,typename ContT::iterator>::type iterator_t;

      typedef typename mpl::if_<boost::is_const<ContT>
        ,typename ContT::const_reference
        ,typename ContT::reference>::type reference_t;

      //typedef typename mpl::if_<boost::is_const<ContT>
      //  ,typename ContT::value_type
      //  ,typename ContT::value_type>::type value_t;

      //typedef typename ContT::value_type value_t;

      inline explicit ConsListView(ContT& c):cont_(c),cur_(c.begin()){}

      /**
      \brief Checks there is element left.
      */
      inline bool isEmpty() const { return cur_ == cont_.end(); }

      /**
      \brief Usual dereference. Returns current element.
      */
      inline reference_t operator*() const { return *cur_; }

      /**
      \brief Increments iterator position.
      */
      inline ConsListView const& operator++() const { ++cur_; return *this; }

      /**
      \brief Inserts new element into wrapped container.
      */
      inline void insert() const {
        cur_=cont_.insert(cont_.end(),typename ContT::value_type());
      }

      ///**
      //\brief Inserts new element into wrapped container.
      //*/
      //inline void remove() const {
      //  cur_=cont_.pop_back();
      //}

      /**
      \brief Returns wrapped container.
      */
      inline ContT& cont() const { return cont_; }

      /**
      \brief Returns current iterator.
      */
      inline iterator_t current() const { return cur_; }
    private:
      ContT& cont_;
      mutable iterator_t cur_;
    };




    template<typename T, typename EnableT>
    struct TypeName<ConsListView<T>, EnableT>
      :TypeName<typename boost::remove_cv<T>::type>{};

    /**
    \brief Cons list creation helper
    */
    template<typename ContT>
    inline ConsListView<ContT> consListView(ContT& v) { return ConsListView<ContT>(v); }

    /**
    \brief Analogue to std::for_each for calling spine's apply function.

    \todo remove it, do the same by means of std::for_each
    */
    template<typename ResT,typename IterT,typename SpineT>
    void spineForEach(ResT& res,IterT b,IterT e,SpineT const& s) {
      for (IterT i = b; i!=e; ++i) {
        res=s.apply(res,*i);
      }
    }

    template<typename ContT, typename FunT>
    struct GmapRedir<ContT, FunT, typename boost::enable_if<
      IsCont<typename boost::remove_const<ContT>::type>
      >::type>
    {
      inline static void action(ContT& p, FunT const& s) {
        std::for_each(p.begin(),p.end(),s);
      }
    };
    
    template<typename ContT, typename FunT>
    struct GmapRedir<HistList<ContT>, FunT> {
      inline static void action(HistList<ContT>& p, FunT const& s) {
        std::for_each(p.val().begin(),p.val().end(),s);
      }
    };

    template<typename T, typename EnableT>
    struct TypeName<std::list<T>, EnableT>:TypeNamePlural<T> {};

    template<typename T, typename EnableT>
    struct TypeName<std::vector<T>, EnableT>:TypeNamePlural<T> {};

    template<typename T1, typename T2>
    struct TypeName<std::pair<T1, T2> > {
      inline static std::string const& value() {
        static std::string ret = "pair";
        return ret;
      }
    };

    template<>
    struct TypeName<std::string> {
      inline static std::string const& value() {
        static std::string ret = "string";
        return ret;
      }
    };

    template<typename ContT, typename EnableT>
    struct ConstrInfo<ConsListView<ContT>, EnableT>
      : ConstrInfoSpec<ConsListView<ContT> >
    {
      inline explicit ConstrInfo(bool isNil):isNil_(isNil){}

      inline void build(ConsListView<ContT> const& v) const {
        if (!isNil_) v.insert();
      }
      inline std::string name() const { return isNil_?"nil":"cons"; }
      //inline bool operator==(ConstrInfo<ConsListView<ContT> > const& r) {
      //  return isNil_ == r.isNil_;
      //}
    private:
      bool isNil_;
    };

    template<typename ContT, typename EnableT>
    struct DataType<ConsListView<ContT>, EnableT>
      : DataTypeSpec<ConsListView<ContT> >
    {
      typedef ConstrInfo<ConsListView<ContT> > constr_t;

      inline DataType():nil_(true),cons_(false){}

      inline size_t numOfConstrs() const { return 2; }
      inline constr_t const& operator[](size_t x) const {
        assert(x<2);
        return x?nil_:cons_;
      }
      inline boost::optional<constr_t> operator[](std::string const& name) {
        if (name == "nil") {
          return boost::optional<constr_t>(nil_);
        }
        if (name == "cons") {
          return boost::optional<constr_t>(cons_);
        }
        return boost::optional<constr_t>();
      }
    private:
      constr_t nil_;
      constr_t cons_;
    };
  }
}

#endif
