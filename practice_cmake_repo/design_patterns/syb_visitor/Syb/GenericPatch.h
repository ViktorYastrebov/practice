/**
\brief The module implements a generic function for applying previously
computed object's difference.

This is similar to Unix's patch command, but it works for values of any SYB 
enabled type.

Usage:

gPatch(obj2,diffScript);

\todo !!! TODO: !!
\\TODO:
the patch script is expected to be position dependent, but clients think it is actually
independent, it works now just because clients send 1 field update at most, it is quite
easy to make it possition independent, just replace parallel traversal to search in 
std::map. But this will reduce perfomance, since now we make object cloning by applying
a patch to the same object. So we leave it as it is now.

In future, either replace to map and make cloning for C++ part or implement an option
for either position dependent or independent traversal.

*/
#ifndef SYB_GENERIC_PATCH_H
#define SYB_GENERIC_PATCH_H

#if 0
#include "Syb/GenericDiff.h"
#include "Syb/Clone.h"
#else
#include "../Syb/GenericDiff.h"
#include "../Syb/Clone.h"
#endif


namespace qx {
  namespace syb {
    /// \brief Tracing static switch
    struct PatchMsg{};
  }
}

template<>
struct TraceName<qx::syb::PatchMsg> {
  inline static std::string const& value() {
    static std::string ret = "patch:";
    return ret;
  }
};

namespace qx {

  namespace syb {

    namespace priv {
    
      /**
      \brief GPatch traversal options disptacher

      \param FunT Dispatching function
      \param U unaliased type of compared value
      */
      template<
        typename U, 
        typename EnableT = void
      > struct GPatchDisp;

    }

    template<typename U>
    class GPatchDispMap {
      public:
        static GPatchDispMap& getInstance() {
          static GPatchDispMap<U> instance;
          return instance;
        }
        void *getDispAction(const std::string &fieldName) {
          DispMapType::iterator it = dispMap_.lower_bound(fieldName);
          if(it == dispMap_.end() || (dispMap_.key_comp()(fieldName, it->first)))
            it = dispMap_.insert(it, DispMapType::value_type(fieldName,
                 (void*)(&priv::GPatchDisp<U>::action)));
          return it->second;
        }
      private:
        GPatchDispMap() {
        }
        GPatchDispMap(GPatchDispMap const&);
        void operator=(GPatchDispMap const&);

        typedef std::map<std::string, void*> DispMapType;
        DispMapType dispMap_;
    };
    
    /// \brief Actual implementation of patch generic function
    struct GPatch : Visitor<GPatch> {
      inline GPatch(std::list<diff::MergeRecord::Field> const& i)
        :inp_(i),cur_(i.begin()){}
#ifdef NOT_INLINES
      template<typename T>
      inline void operator()(T& l) const {
        notInline(*this,l);
      }

      template<typename T>
      inline void runInline(T& dst) const {
        TRACE_SCOPE(PatchMsg,"patch field",TypeName<T>::value());
        if (cur_ == inp_.end()) { 
          TRACE(PatchMsg,"no more data");
          return;
        }
        if (cur_->name() != TypeName<T>::value()) {
          TRACE(PatchMsg,"next field doesn't match (" 
            << cur_->name() << ")."); 
          return;
        }
        priv::GPatchDisp<F,typename UnAlias<T>::type>
          ::action(cur_->diff(),fun_,unAlias(dst));
        ++cur_;
      }
#else
      template<typename T>
      inline void operator()(T& dst) const {
        doPatch(unAlias(dst),TypeName<T>::value());
      }
#endif
    private:
      std::list<diff::MergeRecord::Field> const& inp_;
      mutable std::list<diff::MergeRecord::Field>::const_iterator cur_;

      template<typename U>
      inline void doPatch(U& dst, std::string const& fieldName) const {
        TRACE_SCOPE(PatchMsg,"patch field",fieldName);
        if (cur_ == inp_.end()) { 
          TRACE(PatchMsg,"no more data");
          return;
        }
        if (cur_->name() != fieldName) {
          TRACE(PatchMsg,"next field doesn't match ("<< cur_->name()<< ").");
          return;
        }

        void (*gPatchDispAction)(DiffScript const&, U&) =
          (void (*)(DiffScript const&, U&))GPatchDispMap<U>::getInstance().getDispAction(fieldName);
        gPatchDispAction(cur_->diff(),dst);
        ++cur_;
      }

      template<typename T, typename EnableT>
      friend struct priv::GPatchDisp;

    };

    /**
    \brief Generic function for applying diff script to an object.
    */
    template<typename T>
    void gPatch(T& dst, DiffScript const& v) {
      if (!v) return;
      priv::GPatchDisp<typename UnAlias<T>::type>::action(v,unAlias(dst));
    }
    
    namespace priv {     

      template<typename U>
      struct PatchSetVal {
        inline static void action(DiffScript const& x, U& d) {
          boost::shared_ptr<diff::Update> nr 
            = boost::dynamic_pointer_cast<diff::Update>(x);
          TRACE_SCOPE(PatchMsg,"Update",TypeName<U>::value());
          if (!nr) {
            TRACE(PatchMsg,"wrong type in patch script");
            throw std::runtime_error("Diff script typing error");
          }
          // nr->val().init(&d,dynInfo<U>());
          initAny(nr->val(),d);
          // TODO: how to set?

        }
      };

      template<typename U>
      struct PatchMergeRecord {
        inline static void action(diff::MergeRecord const& x, U& d) {
          GPatch gp(x.fields());
          gmap(d,gp,Unit());
        }
      };

      template<typename U, typename EnableT>
      struct GPatchDisp {
        inline static void action(DiffScript const& x, U& d) {
          TRACE_SCOPE(PatchMsg,"default dispatcher", TypeName<U>::value());
          boost::shared_ptr<diff::MergeRecord> vr 
            = boost::dynamic_pointer_cast<diff::MergeRecord>(x);
          if (vr) {
            TRACE_SCOPE(PatchMsg,"record merging",vr->fields().size());
            PatchMergeRecord<U>::action(*vr,d);
            return;
          }
          PatchSetVal<U>::action(x,d);
        }
      };

      template<typename U>
      struct GPatchDisp<U,typename boost::enable_if<
        mpl::or_<IsPrimitive<U>, IsEnum<U> > >::type>
        :PatchSetVal<U>{};

      template<>
      struct GPatchDisp<Any>:PatchSetVal<Any>{};

      template<>
      struct GPatchDisp<std::vector<unsigned char>, void>
        :PatchSetVal<std::vector<unsigned char> >{};

      template<typename U>
      struct GPatchDisp<U,typename boost::enable_if<IsCont<U> >::type> {
        
        typedef std::list<diff::MergeArray::Item>::const_iterator m_iter_t;
        typedef typename U::iterator iter_t;
        typedef typename U::value_type val_t;

        inline static void resize(size_t s, U& v) {
          size_t cs = v.size();
          TRACE(PatchMsg,"Resizing from " << cs << " to " << s);
          if (s == cs) return;
          if (s > cs) {
            v.insert(v.end(),s-cs,val_t());
          } else {
            iter_t i = v.begin();
            std::advance(i,s);
            v.erase(i,v.end());
          }
        }

        inline static void action(DiffScript const& x, U& d) {
          TRACE_SCOPE(PatchMsg,"array merging",TypeName<U>::value());
          boost::shared_ptr<diff::MergeArray> na
            = boost::dynamic_pointer_cast<diff::MergeArray>(x);
          if (!na) {
            TRACE(PatchMsg,"Wrong type in the patch script");
            throw std::runtime_error("Diff script typing error");
          }
          if (na->size()) {
            resize(*na->size(),unAlias(d));
          }
          iter_t di = unAlias(d).begin();
          size_t ix = 0;
          size_t curx = 0;
          for (m_iter_t i = na->items().begin(); i!=na->items().end(); ++i, ++curx) {
            TRACE_SCOPE(PatchMsg,"patching arrays's element",
              "index: " << ix << "/" << i->index());
            if (i->index() && (*i->index() < ix || *i->index() >= unAlias(d).size())) {
              TRACE(PatchMsg,"Wrong array's index in the patch script");
              throw std::runtime_error("Diff script typing error");
            }
            size_t cx = i->index() ? *i->index() : curx;
            size_t jx = cx - ix;
            if (jx)
              std::advance(di,jx);
            ix = cx;
            GPatchDisp<typename UnAlias<val_t>::type>
              ::action(i->diff(),unAlias(*di));
          }
        }
      };

      template<typename U>
      struct GPatchDisp<HistList<U>, void> {
        
        typedef std::list<diff::MergeArray::Item>::const_iterator m_iter_t;
        typedef typename std::list<U>::iterator iter_t;

        inline static void resize(size_t s, HistList<U>& v) {
          size_t cs = v.val().size();
          TRACE(PatchMsg,"Resizing from " << cs << " to " << s);
          if (s == cs) return;
          if (s > cs) {
            v.valForPatch().insert(v.valForPatch().end(),s-cs,U());
          } else {
            iter_t i = v.valForPatch().begin();
            std::advance(i,s);
            v.valForPatch().erase(i,v.valForPatch().end());
          }
        }

        inline static void action(DiffScript const& x, HistList<U>& d) {
          TRACE_SCOPE(PatchMsg,"array merging",TypeName<U>::value());
          if(boost::dynamic_pointer_cast<diff::MergeHistArray>(x))
            return; // Commit action: hist list doesn't need to be commited
          boost::shared_ptr<diff::MergeArray> na
            = boost::dynamic_pointer_cast<diff::MergeArray>(x);
          if (!na) {
            TRACE(PatchMsg,"Wrong type in the patch script");
            throw std::runtime_error("Diff script typing error");
          }
          if (na->size()) {
            resize(*na->size(),unAlias(d));
          }
          iter_t di = d.valForPatch().begin();
          size_t ix = 0;
          size_t curx = 0;
          for (m_iter_t i = na->items().begin(); i!=na->items().end(); ++i, ++curx) {
            TRACE_SCOPE(PatchMsg,"patching arrays's element",
              "index: " << ix << "/" << i->index());
            if (i->index() && (*i->index() < ix || *i->index() >= unAlias(d).size())) {
              TRACE(PatchMsg,"Wrong array's index in the patch script");
              throw std::runtime_error("Diff script typing error");
            }
            size_t cx = i->index() ? *i->index() : curx;
            size_t jx = cx - ix;
            if (jx)
              std::advance(di,jx);
            ix = cx;
            GPatchDisp<typename UnAlias<U>::type>
              ::action(i->diff(),unAlias(*di));
          }
        }
      };
    }
  }
}

#endif
