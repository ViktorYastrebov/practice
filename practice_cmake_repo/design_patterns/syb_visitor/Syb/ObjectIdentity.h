/**
\brief A helper facilities for an object which has platform and process 
independent identities.

The facilities is required for distibuted system where we can not use object's
address for identifying the object.

Usage of the identities also helps resolve recursive references issue for 
serializing/deserializing objects (and reduce result's file size as well). 
In this case the object will be serialized just once all other references for 
same object will output just reference (i.e. object's identity).

Currently to make an object support the facilities just inherit it from WithId
class. It's planned to make template's specialization based solution to avoid
inheritance in future.

*/
#ifndef SYB_OBJECT_IDENTITY_H
#define SYB_OBJECT_IDENTITY_H

#if 0 
#include "Syb/Clone.h"
#include "Syb/TypeAlias.h"
#include "Syb/Show.h"
#else
#include "Clone.h"
#include "TypeAlias.h"
#include "Show.h"
#endif
#include <set>
#include <list>
#include "boost/weak_ptr.hpp"

namespace qx {
  /**
  \brief Identity's type
  */
  NEWTYPE(Oid,cfg::key_t);
  namespace syb {

    /**
    \brief Any class which needs to be referenced by its identity might be 
    inherited from this class, other option is setting HasId trait 
    */
    struct WithId {
      WithId():id_(0) {}
      inline cfg::key_t const& id() const { return id_; }
      inline void id(cfg::key_t const& v) { id_ = v; }
      virtual ~WithId();
    private:
      cfg::key_t id_;
    };

    /** \brief Type trait for signaling the class supports identities

    The class should provide id setter and getter like WithId class
    */
    template<typename T>
    struct HasId:boost::is_base_of<WithId,T>{};

    /**
    \brief Special helper for tracking some generic algorithm has already
    visited the node with specified key.

    This is useful if we need to perform some action only once for 
    an object, this also solves recursive object's references if 
    it's fine if any object is visited only once.

    This is responsibility of host generic function to add and check
    information regarding visited nodes
    */
    struct TravIdHelper {
      /// \brief Adds and identity for visited object
      inline void visited(cfg::key_t const& v) { visited_.insert(v); }
      /// \brief Checks an object is visited
      inline bool isVisited(cfg::key_t const& v) const {
        return !visited_.empty();
      }
    private:
      std::set<cfg::key_t> visited_;
    };

    // TODO: probably one instance is enough
    struct ObjDir {
      
      /// \brief returns an object with provided key (empty if no such key)
      boost::shared_ptr<WithId> lookup(cfg::key_t k);

      /*
      /// \brief deletes an object with provided key
      inline void del(cfg::key_t k) {
        dict_.erase(k);
        // unreg_.push_back(Oid(k));
      }
      */

      void remove(qx::cfg::key_t const& id);

      /// \brief cleans whole storage (for debug purposes)
      void clean();

      /// \brief adds a new object to the storage
      void add(cfg::key_t k, boost::shared_ptr<WithId> const& n);

      /// \brief adds a new object to the storage 
      /// (it takes key from the object)
      void add(boost::shared_ptr<WithId> const& n);

      //TODO: proper singleton
      static ObjDir& instance();

      /** 
      \brief Simple testing helper. It gives name for a current instance 
        of the singleton. 
      */
      static void saveInstance(std::string const& name);
    
      /**
      Simple testing helper. It loads previously saved instances.
      */
      static void loadInstance(std::string const& name);
      std::map<cfg::key_t,boost::weak_ptr<WithId> > const& dict();
      ~ObjDir();
    private:
      ObjDir();

      static boost::shared_ptr<ObjDir> instance_;
      static std::map<std::string,boost::shared_ptr<ObjDir> > 
        testInstances_;

      typedef std::map<cfg::key_t,boost::weak_ptr<WithId> >::iterator iter_t;

      std::map<cfg::key_t,boost::weak_ptr<WithId> > dict_;
    };

    template<typename TagT, typename U>
    struct CloneHookComplex<TagT,boost::shared_ptr<U>,
      typename boost::enable_if<HasId<U> >::type> 
    {
      typedef DataType<boost::shared_ptr<U> > dt_t;
      inline static void action(
        CloneCtx& x, boost::shared_ptr<U> const& i, boost::shared_ptr<U>& o) 
      {
        TRACE_SCOPE(Clone<TagT>,"Clone: object's reference","");
        if(i)
          o = i->getClone();
        else
          o.reset();
        /*dt_t const& d = dataType<boost::shared_ptr<U> >();
        if (!d.hasConstructor(i) && !d.hasConstructor(o))
          return;
        if (!d.hasConstructor(i)) {
          o = i;
          return;
        }
        bool found = x.lookup(i->id(),o);
        if (!d.hasConstructor(o) || o->id() != i->id())
        {
          if(!found) {
            d.fromValue(i).build(o);
            o->id(i->id());
          }
        }
        if(!found)
          x.addRef(boost::any_cast<boost::shared_ptr<U> >(o));

        zip(Clone<TagT>(x),i,o);*/
      }
    };

    /**
    This specialization will work only if shared_ptr for the
    same object exists in the same object's tree
    */
    template<typename TagT, typename U>
    struct CloneHookComplex<TagT,boost::weak_ptr<U>,
      typename boost::enable_if<HasId<U> >::type> 
    {
      inline static void action(
        CloneCtx& x, boost::weak_ptr<U> const& i, boost::weak_ptr<U>& o) 
      {
        TRACE_SCOPE(Clone<TagT>,"Clone: object's weak reference","");
        boost::shared_ptr<U> is = i.lock();
        if(!is) {
          o.reset();
          return;
        }
        
        /*boost::shared_ptr<U> os = i->GetClone();
        if(!x.lookup(is->id(), os))
          throw std::runtime_error("Clone error: Item was not found for cloning.");*/
        o = is->getClone();
      }
    };

    template<typename U>
    struct StringRepr<boost::shared_ptr<U>, typename boost::enable_if<HasId<U> >::type> : mpl::true_ {
      static bool fromString(boost::shared_ptr<U>& v, UnicodeString const& s) {
        std::string us;
        s.toUTF8String(us);
        cfg::key_t vid = boost::lexical_cast<cfg::key_t>(us);
        v = boost::dynamic_pointer_cast<U>(ObjDir::instance().lookup(vid));
        return true;
      }
      static void toString(boost::shared_ptr<U> const& v, UnicodeString& s) {
        if (!v)
          return;
        s = UnicodeString::fromUTF8(boost::lexical_cast<std::string>(v->id()));
      }
    };
    template<typename U>
    struct StringRepr<boost::weak_ptr<U>, typename boost::enable_if<HasId<U> >::type> : mpl::true_ {
      static bool fromString(boost::weak_ptr<U>& v, UnicodeString const& s) {
        std::string us;
        s.toUTF8String(us);
        cfg::key_t vid = boost::lexical_cast<cfg::key_t>(us);
        v = boost::dynamic_pointer_cast<U>(ObjDir::instance().lookup(vid));
        return true;
      }
      static void toString(boost::weak_ptr<U> const& v, UnicodeString& s) {
        boost::shared_ptr<U> vv = v.lock();
        if (!vv)
          return;
        s = UnicodeString::fromUTF8(boost::lexical_cast<std::string>(vv->id()));
      }
    };
  }
}

#endif
