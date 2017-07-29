#ifndef SYB_RECORD_FIELD_H
#define SYB_RECORD_FIELD_H

#include <string>
#include <utility>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

#include <loki/Singleton.h>

namespace qx {
  namespace msg {
    typedef std::vector<std::string> EntDictType;

    class RecordFieldImpl {
    public:
      std::string getName(boost::uint32_t& index) const {
        std::string name;
        name = entDict_.at(index);
        if (name.empty())
          throw std::runtime_error("Incorrect Version of Server");
        return name;
      }
    private:
      RecordFieldImpl();
      ~RecordFieldImpl() {
      }
      friend struct Loki::CreateUsingNew<RecordFieldImpl>;

      EntDictType entDict_;
    };
    typedef Loki::SingletonHolder<RecordFieldImpl> RecordField;
  }
}
#endif
