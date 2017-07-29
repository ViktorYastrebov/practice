//#include "Syb/GenericDiff.h"
#include "GenericDiff.h"

namespace qx {
  namespace syb {

     namespace diff {
       /*void Update::toBinary(std::ostream &output) const {
         qx::msg::write(output, UPDATE);
         qx::msg::write(output, val());
       }*/

       cJSON *Update::toJson() const {
         return qx::json::write(val(), true);
       }

        /*
        void MergeRecord::toBinary(std::ostream &output) const {
          qx::msg::write(output, RECORD);
          qx::msg::write(output, static_cast<boost::uint32_t>(fields().size()));
          BOOST_FOREACH(const qx::syb::diff::MergeRecord::Field &field, fields()) {
            qx::msg::write(output, field.nameIndex());
            field.diff()->toBinary(output);
          }
        }
        */

        cJSON *MergeRecord::toJson() const {
          cJSON *node = cJSON_CreateObject();

          cJSON_AddItemToObject(node, "_m", cJSON_CreateString("r"));

          BOOST_FOREACH(const Field &field, fields())
            cJSON_AddItemToObject(node, field.name().c_str(), field.diff()->toJson());

          return node;
        }
        
        /*
        static void writeOptInt(std::ostream& s, boost::optional<boost::uint32_t> const& v) {
          bool hasValue = v;
          qx::msg::write(s, hasValue);
          if (hasValue)
            qx::msg::write(s, *v);
        }

        void MergeArray::toBinary(std::ostream &output) const {
          qx::msg::write(output, ARRAY);
          writeOptInt(output, size());
          qx::msg::write(output, static_cast<unsigned int>(items().size()));
          BOOST_FOREACH(const qx::syb::diff::MergeArray::Item &item, items()) {
            writeOptInt(output, item.index());
            item.diff()->toBinary(output);
          }
        }
        */

        cJSON *MergeArray::toJson() const {
          cJSON *node = cJSON_CreateObject();

          cJSON_AddItemToObject(node, "_m", cJSON_CreateString("a"));

          if (size())
            cJSON_AddItemToObject(node, "s", cJSON_CreateNumber(*size()));

          if (!items().empty()) {
            cJSON *array = cJSON_CreateArray();
            BOOST_FOREACH(const qx::syb::diff::MergeArray::Item &item, items()) {
              cJSON *diffScriptNode = 0;
              diffScriptNode = item.diff()->toJson();
              if (diffScriptNode) {
                if (item.index()) {
                  cJSON *itemObject = cJSON_CreateObject();

                  cJSON_AddItemToObject(itemObject, "_x", cJSON_CreateNumber(*item.index()));
                  cJSON_AddItemToObject(itemObject, "v", diffScriptNode);

                  cJSON_AddItemToArray(array, itemObject);
                } else {
                  cJSON_AddItemToArray(array, diffScriptNode);
                }
              }
            }
            cJSON_AddItemToObject(node, "i", array);
          }
          return node;
        }

        /*void MergeHistArray::toBinary(std::ostream &output) const {
          qx::msg::write(output, HIST_ARRAY);
          qx::msg::write(output, static_cast<unsigned int>(items().size()));
          BOOST_FOREACH(const qx::syb::diff::MergeHistArray::Item &item, items()) {
            qx::msg::write(output, item.skip());
            qx::msg::write(output, item.del());
            qx::msg::write(output, static_cast<unsigned int>(item.diffs().size()));
            BOOST_FOREACH(const DiffScript &ds, item.diffs())
              ds->toBinary(output);
          }
        }*/

        cJSON *MergeHistArray::toJson() const {
          cJSON *node = cJSON_CreateObject();

          cJSON_AddItemToObject(node, "_m", cJSON_CreateString("h"));

          if (!items().empty()) {
            cJSON *array = cJSON_CreateArray();
            BOOST_FOREACH(const qx::syb::diff::MergeHistArray::Item &item, items()) {
              cJSON *itemObject = cJSON_CreateObject();
              if(item.skip() > 0)
                cJSON_AddItemToObject(itemObject, "s", cJSON_CreateNumber(item.skip()));
              if(item.del() > 0)
                cJSON_AddItemToObject(itemObject, "d", cJSON_CreateNumber(item.del()));
              cJSON *insItems = cJSON_CreateArray();
              BOOST_FOREACH(const DiffScript &ds, item.diffs())
                cJSON_AddItemToArray(insItems, ds->toJson());
              cJSON_AddItemToObject(itemObject, "i", insItems);
              cJSON_AddItemToArray(array, itemObject);
            }
            cJSON_AddItemToObject(node, "e", array);
          }
          return node;
        }

     }
  }
}
