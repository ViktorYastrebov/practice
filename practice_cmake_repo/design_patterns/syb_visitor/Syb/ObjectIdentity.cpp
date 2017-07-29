#include "ObjectIdentity.h"

namespace qx {
  namespace syb {
  
    boost::shared_ptr<WithId> ObjDir::lookup(cfg::key_t k) {
      iter_t i = dict_.find(k);
      if (i == dict_.end()) 
        return boost::shared_ptr<WithId>();
      boost::weak_ptr<WithId> tmp = i->second;
      return tmp.lock();
    }

/*
      void ObjDir::del(cfg::key_t k) {
        dict_.erase(k);
        // unreg_.push_back(Oid(k));
      }
*/

    void ObjDir::remove(qx::cfg::key_t const& id) {
      TRACE(ObjDir, "remove : idx = " << id);
      dict_.erase(id);
    }

    void ObjDir::clean() {
      dict_ = std::map<cfg::key_t,boost::weak_ptr<WithId> >();
    }

    void ObjDir::add(cfg::key_t k, boost::shared_ptr<WithId> const& n) {
      if (!n) return;
      n->id(k);
      add(n);
    }

    void ObjDir::add(boost::shared_ptr<WithId> const& n) {
      // dict_[n->id()] = n;
      TRACE(ObjDir, "add : idx = " << n->id());
      if (!dict_.insert(std::make_pair(n->id(),n)).second) 
        throw std::runtime_error("Object's alredy been registered: "
          + boost::lexical_cast<std::string>(n->id()));
    }

    ObjDir& ObjDir::instance() {
      if (!instance_) {
        instance_.reset(new ObjDir);
      }
      return *instance_;
    }

    void ObjDir::saveInstance(std::string const& name) {
      testInstances_[name] = instance_;
    }

    void ObjDir::loadInstance(std::string const& name) {
      boost::shared_ptr<ObjDir>& inst = testInstances_[name];
      if (!inst) inst.reset(new ObjDir);
      instance_ = inst;
    }

    std::map<cfg::key_t,boost::weak_ptr<WithId> > const& ObjDir::dict() { 
      return dict_; 
    }

    ObjDir::ObjDir(){}

    ObjDir::~ObjDir(){
      instance_ = boost::shared_ptr<ObjDir>();
    }

    boost::shared_ptr<ObjDir> objDirInstance = boost::shared_ptr<ObjDir>();
    boost::shared_ptr<ObjDir> ObjDir::instance_ = objDirInstance;
    std::map<std::string,boost::shared_ptr<ObjDir> > ObjDir::testInstances_ 
      = std::map<std::string,boost::shared_ptr<ObjDir> >();
    
    WithId::~WithId(){}

  }
}
