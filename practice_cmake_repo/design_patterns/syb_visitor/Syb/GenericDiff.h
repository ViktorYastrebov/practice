/**
\brief The module implements a generic function for computing object's
differences.

This is similar to Unix's diff command, but it works for values of any SYB
enabled type.

The resulting difference (DiffScript) is executable, i.e. it can be applied
to an object with source state to get its target state.

Because new valies are stored as dynamic objects (Any from Syb/Any.h) in the
script it's required to provide dispatching function for it during creation
(i.e. it should be known how the script will be used exactly).

For example the script can be serialized into XML for further deserializing
and patching values on some remote side for object's state synchronization.
(see SybXml2/XmlDiffPatch.h for this dispatching function implementation).

Usage:

The entry point function is gDiff.

MyType obj1;
MyType obj2;
syb::clone<HistoryTag>(obj1,obj2);
....
obj2.field = newValue;
....

DiffScript script = gDiff(obj1, obj2);
// now v has differences between obj1 and obj2
// to make obj2 equal to obj2 just run: (see GenericPatch.h)

gPatch(obj2,script);
...

But this functions are not use directly in the project, for XML diff/patch
use xmlPatch and xmlDiff instead (see XmlDiffPatch.h) This functions call
gDiff and gPatch and provides them required dispatcher function.

*/
#ifndef SYB_GENERIC_DIFF_H
#define SYB_GENERIC_DIFF_H

#if 0
#include "Syb/ObjectIdentity.h"
#include "Syb/StlSyb.h"
#include "Syb/Enum.h"
#include "Syb/Any.h"
#include "Syb/MultiConstr.h"
#include "Syb/Syb.h"
#include "Syb/Poly.h"
#include "Syb/Trace.h"

#include "Syb/BinaryMsgStub.h"
#include "Syb/cJSON.h"
#else
#include "ObjectIdentity.h"
#include "StlSyb.h"
#include "Enum.h"
#include "Any.h"
#include "MultiConstr.h"
#include "Syb.h"
#include "Poly.h"
#include "Trace.h"

#include "BinaryMsgStub.h"
#include "cJSON.h"
#endif

#include <boost/make_shared.hpp>
#include <boost/variant.hpp>
#include <boost/type_traits.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <set>
#include <climits>

namespace qx {
  namespace syb {
    /// \brief Tracing static switch
    struct DiffMsg{};
  }
}

template<>
struct TraceName<qx::syb::DiffMsg> {
  inline static std::string const& value() {
    static std::string ret = "diff:";
    return ret;
  }
};

namespace qx {
  namespace syb {

    /**
    \brief Parent class for a diff script item

    \todo consider changing to boost::variant for runtime optimization
    */
    struct DiffItem {
      virtual ~DiffItem();
      /*virtual void toBinary(std::ostream &output) const {
      }*/
      virtual cJSON *toJson() const {
        return 0;
      }
    private:
      TRAVERSABLE_ABSTRACT_BEGIN(DiffItem)
      TRAVERSABLE_END
    };
    typedef boost::shared_ptr<DiffItem> DiffScript;


    namespace diff {
      /**
      \brief Sets new value and discards its old one
      */
      struct Update : DiffItem {
        inline Any const& val() const { return val_.val(); }
        inline Any& val() { return val_.val(); }
        inline void val(Any const& v) { val_.val(v); }

        //void toBinary(std::ostream &output) const;
        cJSON *toJson() const;
      private:
        NEWTYPE(Val,Any);
        Val val_;

        TRAVERSABLE_CHILD_BEGIN(Update,DiffItem)
          TRAVERSABLE_FIELD(val_)
        TRAVERSABLE_END
      };

      /**
      \brief Merges content of a record
      */
      struct MergeRecord : DiffItem {

        struct Field {
          inline std::string const& name() const { return name_.val(); }
          inline Field& name(std::string const& n) {
            name_.val(n);
            return *this;
          }

          inline boost::uint32_t nameIndex() const { return nameIndex_.val(); }
          inline Field& nameIndex(boost::uint32_t n) {
            nameIndex_.val(n);
            return *this;
          }

          inline DiffScript& diff() const { return diff_; }
          inline Field& diff(DiffScript const& v) {
            diff_ = v;
            return *this;
          }
        private:
          NEWTYPE(Name, std::string);
          NEWTYPE(NameIndex, boost::uint32_t);
          Name name_;
          NameIndex nameIndex_;
          mutable DiffScript diff_;

          TRAVERSABLE_BEGIN(Field)
            TRAVERSABLE_FIELD(name_)
            TRAVERSABLE_FIELD(nameIndex_)
            TRAVERSABLE_FIELD(diff_)
          TRAVERSABLE_END
        };

        inline std::list<Field>& fields() { return fields_; }
        inline std::list<Field> const& fields() const { return fields_; }

        //void toBinary(std::ostream &output) const;

        cJSON *toJson() const;
      private:
        std::list<Field> fields_;

        TRAVERSABLE_CHILD_BEGIN(MergeRecord,DiffItem)
            TRAVERSABLE_FIELD(fields_)
        TRAVERSABLE_END
      };

      /**
      Merges content of an array
      */
      struct MergeArray : DiffItem {

        inline boost::optional<unsigned int> const& size() const {
          return size_.val();
        }
        inline void size(boost::optional<unsigned int> const& v) {
          size_.val(v);
        }

        struct Item {
          inline boost::optional<unsigned int> index() const { return index_.val(); }
          inline void index(boost::optional<unsigned int> v) { index_.val(v); }

          inline void diff(DiffScript const& v) { diff_ = v; }
          inline DiffScript const& diff() const { return diff_; }
          inline DiffScript& diff() { return diff_; }

        private:
          NEWTYPE(Index,boost::optional<unsigned int>);

          Index index_;
          DiffScript diff_;

          TRAVERSABLE_BEGIN(Item)
              TRAVERSABLE_FIELD(index_)
              TRAVERSABLE_FIELD(diff_)
          TRAVERSABLE_END

        };

        inline std::list<Item>& items() { return items_; }
        inline std::list<Item> const& items() const { return items_; }

        //void toBinary(std::ostream &output) const;
        cJSON *toJson() const;
      private:
        NEWTYPE(Size,boost::optional<unsigned int>);
        Size size_;
        std::list<Item> items_;

        TRAVERSABLE_CHILD_BEGIN(MergeArray,DiffItem)
            TRAVERSABLE_FIELD(size_)
            TRAVERSABLE_FIELD(items_)
        TRAVERSABLE_END
      };

      /**
      Merges content of an array by set of actions
      */
      struct MergeHistArray : DiffItem {

        struct Item {
          Item() {}
          Item(unsigned int s) : skip_(s), del_(0) {}

          inline std::list<DiffScript>& diffs() { return diffs_; }
          inline std::list<DiffScript>const& diffs() const { return diffs_; }

          inline unsigned int skip() const { return skip_.val(); }
          inline void skip(unsigned int const& v) { skip_ = v; }

          inline unsigned int del() const { return del_.val(); }
          inline void del(unsigned int const& v) { del_ = v; }

        private:
          NEWTYPE(Idx, unsigned int);
          std::list<DiffScript> diffs_;
          Idx skip_;
          Idx del_;

          TRAVERSABLE_BEGIN(Item)
              TRAVERSABLE_FIELD(diffs_)
              TRAVERSABLE_FIELD(skip_)
              TRAVERSABLE_FIELD(del_)
          TRAVERSABLE_END
        };

        inline std::list<Item>& items() { return items_; }
        inline std::list<Item> const& items() const { return items_; }

        //void toBinary(std::ostream &output) const;
        cJSON *toJson() const;
      private:
        std::list<Item> items_;
        TRAVERSABLE_CHILD_BEGIN(MergeHistArray,DiffItem)
            TRAVERSABLE_FIELD(items_)
        TRAVERSABLE_END
      };
    }

    namespace priv {

      /**
      \brief GDiff traversal options disptacher

      \param FunT Dispatching function
      \param U unaliased type of compared value
      */
      template<
        typename U,
        typename EnT=void>
      struct GDiffDisp;

    }

    /**
    \brief Generig function for getting Diff script from any two SYB
    traversable objects
    */
    struct GDiff {
      inline explicit GDiff(TravIdHelper& h):traversed_(h){}
      inline GDiff(GDiff const& t):traversed_(t.traversed_){}

      static const bool is_left_const = true;
      static const bool is_right_const = true;

      template<typename T>
      inline void operator()(T const& src, T const& dst) const {
        doDiff(unAlias(src),unAlias(dst),TypeName<T>::value(),EntId<T>::value);
      }

      template<typename T>
      inline void operator()(boost::optional<T> const& src, boost::optional<T> const& dst) const {
        doDiff(unAlias(src),unAlias(dst),TypeName<T>::value(),EntId<T>::value);
      }

      inline std::list<diff::MergeRecord::Field>& res() {
        return res_;
      }

      inline std::list<diff::MergeRecord::Field> const& res() const {
        return const_cast<std::list<diff::MergeRecord::Field>&>(
          static_cast<const GDiff&>(*this).res());
      }

    private:
      mutable std::list<diff::MergeRecord::Field> res_;
      TravIdHelper& traversed_;

      template<typename T>
      inline DiffScript setVal(T const& v) const {
        boost::shared_ptr<diff::Update> r(boost::make_shared<diff::Update>());
        r->val(toAny(v));
        return r;
      }

      template<typename U>
      void doDiff(U const& src, U const& dst, std::string const& fieldName, boost::uint32_t fieldIndex) const {
        TRACE_SCOPE(DiffMsg,"field traverse",fieldName);
        DiffScript res = priv::GDiffDisp<U>::action(*this,src,dst);
        TRACE(DiffMsg,(res ? "found a difference" : "difference wasn't found")
            << " for " << fieldName)
        if(res) {
          res_.push_back(diff::MergeRecord::Field());
          res_.back().name(fieldName).nameIndex(fieldIndex).diff().swap(res);
        }
      }

      template<typename U2, typename EnableT2>
      friend struct priv::GDiffDisp;
    };

    /**
    \brief Collect a difference of objects to a diff-script
    */
    template<typename T>
    DiffScript gDiff(T const& src, T const& dst) {
      TravIdHelper idh;
      GDiff gfun(idh);
      return priv::GDiffDisp<typename UnAlias<T>::type>
        ::action(gfun,unAlias(src),unAlias(dst));
    }

    namespace priv {

      template<typename U>
      struct MergeRecHelper {
        static DiffScript action(GDiff const& ctx, U const& s, U const& d) {
          TRACE_SCOPE(DiffMsg,"Record Dispatcher",TypeName<U>::value());
          GDiff nctx(ctx);
          zip(nctx,s,d);
          TRACE(DiffMsg,
            (nctx.res().empty()
              ? "no differences"
              : "some fields are different"
              ) << " for " << TypeName<U>::value()
            );
          if(nctx.res().empty())
            return DiffScript();
          boost::shared_ptr<diff::MergeRecord> val(
            boost::make_shared<diff::MergeRecord>());
          val->fields().swap(nctx.res());
          return val;
        }
      };

      template<typename U, typename EnableT>
      struct GDiffDisp {
        static DiffScript action(GDiff const& ctx, U const& s, U const& d) {
          //TODO: Single constr version
          DataType<U> const& dt = dataType<U>();
          TRACE_SCOPE(DiffMsg,"default dispatcher"
            ,TypeName<U>::value()
            << (dt.hasConstructor(s)?"":" (source is empty)")
            << (dt.hasConstructor(d)?"":" (destination is empty)")
            );

          if (!dt.hasConstructor(s)
            && !dt.hasConstructor(d))
              return DiffScript();

          if (!dt.hasConstructor(s)
            || !dt.hasConstructor(d)
            || !(dt.fromValue(s) == dt.fromValue(d)))
          {
            TRACE(DiffMsg,"new val");
            return ctx.setVal(d);
          }

          return MergeRecHelper<U>::action(ctx,s,d);
        }
      };

      template<typename U, typename EnT = void>
      struct AtomicDisp:mpl::or_<IsPrimitive<U>,IsEnum<U> >{};

      template<typename U>
      struct GDiffDisp<U,
        typename boost::enable_if<AtomicDisp<U> >::type>
      {
        static DiffScript action(GDiff const& ctx, U const& s, U const& d) {
          TRACE_SCOPE(DiffMsg,"atomic value diff",TypeName<U>::value());
          if (s != d) {
            TRACE(DiffMsg,"new value");
            return ctx.setVal(d);
          }
          return DiffScript();
        }
      };

      //TODO: make it primitive
      template<>
      struct GDiffDisp<std::vector<unsigned char>, void>
      {
        static DiffScript action(GDiff const& ctx, std::vector<unsigned char> const& s, std::vector<unsigned char> const& d) {
          TRACE_SCOPE(DiffMsg,"blob value diff","BLOB");
          if (s != d) {
            TRACE(DiffMsg,"new value");
            return ctx.setVal(d);
          }
          return DiffScript();
        }
      };

      template<typename U>
      struct GDiffDisp<boost::optional<U>,
        typename boost::enable_if<AtomicDisp<U> >::type>
      {
        static DiffScript action(GDiff const& ctx, boost::optional<U> const& s, boost::optional<U> const& d) {
          TRACE_SCOPE(DiffMsg,"atomic value diff",TypeName<U>::value());
          if (!s && !d)
            return DiffScript();
          if (!s || !d || *s != *d) {
            TRACE(DiffMsg,"new value");
            return ctx.setVal(d);
          }
          return DiffScript();
        }
      };

      template<typename U>
      struct GDiffDisp<U,typename boost::enable_if<IsCont<U> >::type> {
        typedef typename U::value_type val_t;
        typedef typename U::const_iterator iter_t;
        static DiffScript action(GDiff const& ctx, U const& s, U const& d) {
          //TODO: extract not type dependent parts
          size_t x = 0;
          size_t len = 0;
          size_t ds = d.size();
          size_t ss = s.size();
          TRACE_SCOPE(DiffMsg,"Array's diff",TypeName<U>::value() <<
            " ( destination's size: " << ds
            << ", sources's size: " << ss << ")"
            );
          std::list<diff::MergeArray::Item> items;
          for (iter_t di = d.begin(), si = s.begin();di!=d.end();++di,x++) {
            if (si != s.end()) {
              DiffScript el = gDiff(unAlias(*si),unAlias(*di));
              if (el) {
                items.push_back(diff::MergeArray::Item());
                items.back().diff(el);
                if (x != len)
                  items.back().index(x);
                len++;
              }
              ++si;
            } else {
                TRACE(DiffMsg,"new array's element");
                items.push_back(diff::MergeArray::Item());
                items.back().diff(ctx.setVal(*di));
                if (x != len)
                  items.back().index(x);
                len++;
            }
          }
          if (items.empty() && ds == ss) {
            TRACE(DiffMsg,"no changes");
            return DiffScript();
          }
          boost::shared_ptr<diff::MergeArray> ret(
            boost::make_shared<diff::MergeArray>());
          ret->items().swap(items);
          if (ds != ss) {
            TRACE(DiffMsg,"needs resizing");
            ret->size(ds);
          }
          return ret;
        }
      };

      template<typename U>
      struct GDiffDisp<HistList<U>, void> {
        typedef typename std::list<ArrayTrace<U> > trace_lst;
        typedef typename trace_lst::const_iterator iter_t;
        static DiffScript action(GDiff const& ctx, HistList<U> const& s, HistList<U> const& d) {
          const trace_lst& traces = d.getTracesForDiff();
          if (traces.empty()) {
            TRACE(DiffMsg,"Array's diff: no changes");
            return DiffScript();
          }
          TRACE_SCOPE(DiffMsg,"Array's diff",TypeName<HistList<U> >::value() <<
            " ( sources's size: " << s.size() << ")");
          boost::shared_ptr<diff::MergeHistArray> ret(
            boost::make_shared<diff::MergeHistArray>());
          std::list<diff::MergeHistArray::Item>& items = ret->items();
          unsigned int in = UINT_MAX, rm = UINT_MAX;
          for (iter_t i = traces.begin(); i != traces.end(); ++i) {
            if(i->type() == Rem) {
              if(i->index() != rm) {
                in = rm = i->index();
                items.push_back(diff::MergeHistArray::Item(i->index()));
              }
              items.back().del(items.back().del() + 1);
            } else {
              if(i->index() != in) {
                in = i->index();
                items.push_back(diff::MergeHistArray::Item(i->index()));
              }
              items.back().diffs().push_back(ctx.setVal(i->val()));
              ++in;
            }
          }
          return ret;
        }
      };

      // TODO: here we're in slightly worse situation than the previous
      // solution, because we don't have object storage here, so
      // any assignment of a reference will serialize whole object
      // and if it has assigned somewhere else this will serialize the
      // object twice, we can easily return object storage back here
      // but this is the thing we need to think off, probably just
      // using weak_ptr's will be better solution and this will imply
      // additional contracts on type representation
      template<typename U>
      struct GDiffDisp<boost::shared_ptr<U>,
          typename boost::enable_if<HasId<U> >::type>
      {
        static DiffScript action(GDiff const& ctx,
            boost::shared_ptr<U> const& s, boost::shared_ptr<U> const& d)
        {
          TRACE_SCOPE(DiffMsg,"reference",TypeName<U>::value()
            << ", source: "
            << (s?boost::lexical_cast<std::string>(s->id()):"empty")
            << ", destination: "
            << (d?boost::lexical_cast<std::string>(d->id()):"empty")
            );

          if (!s && !d)
            return DiffScript();
          if (!s || !d) {
            return ctx.setVal(d);
          }
          if (s->id() != d->id()) {
            return ctx.setVal(d);
          }
          /*if (ctx.traversed_.isVisited(s->id())) {
            TRACE(DiffMsg,"already visited");
            return DiffScript();
          }
          return MergeRecHelper<boost::shared_ptr<U> >
            ::action(ctx,s,d);*/
          return DiffScript();
        }
      };

      template<typename U>
      struct GDiffDisp<boost::weak_ptr<U>,
          typename boost::enable_if<HasId<U> >::type>
      {
        typedef boost::shared_ptr<U> locked_t;
        static DiffScript action(GDiff const& ctx,
            boost::weak_ptr<U> const& s, boost::weak_ptr<U> const& d)
        {
          locked_t sl = s.lock();
          locked_t dl = d.lock();
          TRACE_SCOPE(DiffMsg,"weak reference",TypeName<U>::value()
            << ", source: "
            << (sl?boost::lexical_cast<std::string>(sl->id()):"empty")
            << ", destination: "
            << (dl?boost::lexical_cast<std::string>(dl->id()):"empty")
            );

          if (!sl && !dl)
            return DiffScript();
          if (!sl || !dl)
            return ctx.setVal(d);
          if (sl->id() == dl->id())
            return DiffScript();
          return ctx.setVal(d);
        }
      };

      //TODO: this should be treated as atomic value
      template<>
      struct GDiffDisp<Any>
      {
        static DiffScript action(GDiff const& ctx, Any const& s, Any const& d)
        {
          TRACE_SCOPE(DiffMsg,"atomic Any value diff","any");
          if (s != d) {
            TRACE(DiffMsg,"new value");
            return ctx.setVal(d);
          }
          return DiffScript();
        }
      };
    }
  }
}


#endif
