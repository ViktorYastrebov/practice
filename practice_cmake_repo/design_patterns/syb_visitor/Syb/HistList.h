#ifndef SYB_HIST_LIST_H
#define SYB_HIST_LIST_H


//#include "Syb/Syb.h"
#include "Syb.h"

#include <boost/call_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/value_init.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <loki/Singleton.h>
//#include "misc/getenv.h"
#include <list>

namespace qx {
  namespace syb {

    enum ArrayTraceType {
      Ins = 0,
      Rem
    };

    struct ICommitable {
      virtual void commit() {}
    };

    struct CommitInfo {
      explicit CommitInfo(ICommitable* ref):ref_(ref) {}
      void commit() {
        if (ref_)
          ref_->commit();
      }
      void reset() { ref_ = NULL; }
    private:
      ICommitable* ref_;
    };

    struct CommitActions {
      inline CommitInfo* add(ICommitable* f) {
        lst_.push_back(CommitInfo(f));
        return &lst_.back();
      }
      inline void commit() {
        BOOST_FOREACH(CommitInfo& f, lst_)
          f.commit();
        lst_.clear();
      }
    private:
      std::list<CommitInfo> lst_;
    };


    template<typename T>
    struct ArrayTrace {
      inline ArrayTrace(ArrayTraceType t, unsigned int i) : type_(t), index_(i) {}
      inline ArrayTrace(ArrayTraceType t, unsigned int i, const T& v) : type_(t), index_(i), val_(&v) {}
      inline unsigned int index() const { return index_; }
      inline void index(unsigned int i) { index_ = i; }
      inline T const& val() const { return *val_; }
      inline ArrayTraceType type() const { return type_; }
      bool operator< (const ArrayTrace& t) const {
        return (index() < t.index() || (index() == t.index() && type() > t.type()));
      }
    private:
      ArrayTraceType type_;
      unsigned int index_;
      const T* val_;
    };

    template<typename T>
    struct HistList : ICommitable {
      typedef T ValueType;
      typedef std::list<T> List;
      typedef std::list<ArrayTrace<T> > ArrayTraceList;

      inline HistList() : commitInfo_(NULL) {}

      inline HistList(const HistList& l) : commitInfo_(NULL) {
        set(l.val());
      }

      inline ~HistList() {
        if (commitInfo_)
          commitInfo_->reset();
      }

      HistList<T>& operator=(const HistList<T>& r) {
        if (this == &r)
            return *this;
        set(r.val());
        return *this;
      }

      inline void insert(int index, const T& vl) {
        unsigned int size = val_.size();
        if(size < index || index < 0)
          return;
        for(typename ArrayTraceList::iterator i = traces_.begin(); i != traces_.end(); ++i)
          if(i->index() > index || (i->index() == index && i->type() != Rem))
            i->index(i->index() + 1);
        if(size == index) {
          val_.push_back(vl);
          addTraces(ArrayTrace<T>(Ins, index, val_.back()));
        } else if(size > index) {
          typename List::iterator it = val_.begin();
          std::advance(it, index);
          it = val_.insert(it, vl);
          addTraces(ArrayTrace<T>(Ins, index, *it));
        }
      }

      inline void removeAt(int index) {
        if(val_.size() <= index || index < 0)
          return;
        bool addTrace = true;
        typename ArrayTraceList::iterator i = traces_.begin();
        while(i != traces_.end()) {
          if(i->index() == index && i->type() == Ins) {
            assert(addTrace);
            i = traces_.erase(i);
            addTrace = false;
            continue;
          }
          if(i->index() > index)
            i->index(i->index() - 1);
          ++i;
        }
        if(addTrace)
          addTraces(ArrayTrace<T>(Rem, index));
        typename List::iterator it = val_.begin();
        std::advance(it, index);
        val_.erase(it);
      }

      inline void remove(const T& val) {
        int i = indexOf(val);
        if(i != -1)
          removeAt(i);
      }

      inline void update(int index, const T& val) {
        if(size() > index && getAt(index) == val)
          return;
        removeAt(index);
        insert(index, val);
      }

      inline T const& getAt(int index) const {
        typename List::const_iterator i = val_.begin();
        std::advance(i, index);
        return *i;
      }

      inline int indexOf(const T& v) const {
        typename List::const_iterator it = val_.begin();
        for(unsigned int i = 0; it != val_.end(); ++i, ++it)
          if(*it == v)
            return i;
        return -1;
      }

      inline bool contains(const T& v) const {
        return std::find(val_.begin(), val_.end(), v) != val_.end();
      }

      inline void erase(int first, int last) {
        for(int i = first; i <= last; ++i)
          removeAt(first);
      }

      inline void add(const T& val) {
        insert(val_.size(), val);
      }

      inline void clear() {
        erase(0, val_.size());
      }

      inline size_t size() const {
        return val_.size();
      }

      inline bool empty() const {
        return val_.empty();
      }

      inline std::list<ArrayTrace<T> > const& getTracesForDiff() const {
        traces_.sort();
        return traces_;
      }

      inline void set(List const& v) {
        clear();
        typename List::const_iterator it = v.begin();
        for(unsigned int i = 0; it != v.end(); ++i, ++it)
          insert(i, *it);
      }

      /// \brief setter
      inline void val(List const& v) {
        set(v);
      }

      /// \brief getter
      inline List const& val() const {
        return val_;
      }

      /// \brief reference getter
      inline List& valForPatch() {
        return val_;
      }

      inline void commit() {
        traces_.clear();
        commitInfo_ = NULL;
      }

    private:
      CommitInfo* commitInfo_;
      inline void addTraces(const ArrayTrace<T>& t) {
        if(traces_.empty())
          commitInfo_ = Loki::SingletonHolder<CommitActions, Loki::CreateUsingNew, Loki::PhoenixSingleton>::Instance().add(this);
        traces_.push_back(t);
      }

      List val_;
      mutable ArrayTraceList traces_;
    };
  }
}

#endif