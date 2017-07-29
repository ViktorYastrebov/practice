#ifndef SYB_XML2_ANY_ELEMENT_H
#define SYB_XML2_ANY_ELEMENT_H

#include <iterator>

#include <rapidxml.hpp>
#include <rapidxml_print.hpp>

namespace qx {
  namespace syb {
    // TODO: Just keep state
    struct AnyElementImpl {
      inline AnyElementImpl(rapidxml::xml_node<> *node) : node_(node) {
        if (node)
          rapidxml::print(std::back_insert_iterator<std::string>(buf_), *node,
                          rapidxml::print_no_indenting);
      }
      inline rapidxml::xml_node<> *node() {
        return node_;
      }
      inline const std::string &buf() {
        return buf_;
      }
    private:
      rapidxml::xml_node<> *node_;
      std::string buf_;
    };
  }
}

#endif
