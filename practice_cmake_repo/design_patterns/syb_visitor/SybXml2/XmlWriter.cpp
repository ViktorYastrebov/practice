#include "XmlWriter.h"

#include <set>
#include <ostream>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include <rapidxml.hpp>
#include <rapidxml_print.hpp>

#include "AnyElementImpl.h"

#if 0
#include "Syb/Trace.h"
#include "Syb/GenericDiff.h"
#include "Syb/GenericPatch.h"

#include "Syb/SybAll.h"
#else
#include "../Syb/Trace.h"
#include "../Syb/GenericDiff.h"
#include "../Syb/GenericPatch.h"

#include "../Syb/SybAll.h"
#endif

extern "C" {
#include "cencode.h"
}

using namespace qx;
using namespace qx::syb;
using namespace qx::syb::diff;

namespace Loki {

  template<>
  struct ImplOf<qx::syb::priv::XmlWriterImpl> {
    ImplOf(): node_(&doc_) {
      rapidxml::xml_node<> *declNode = doc_.allocate_node(rapidxml::node_declaration);
      declNode->append_attribute(doc_.allocate_attribute(
        doc_.allocate_string("version"), "1.0", sizeof("version") - 1, sizeof("1.0") - 1));
      declNode->append_attribute(doc_.allocate_attribute(
        doc_.allocate_string("encoding"), "UTF-8", sizeof("encoding") - 1, sizeof("UTF-8") - 1));
      doc_.append_node(declNode);
    }

    ~ImplOf() {
    }

    std::set<int> computed_;
    rapidxml::xml_document<> doc_;
    rapidxml::xml_node<> *node_;

    void setRootNamespace(const char *rootNs) {
      if (rootNs && rootNs[0] != '\0') {
        rapidxml::xml_node<> *node = 0;
        if ((node = doc_.first_node()) && (node = node->next_sibling()))
          node->append_attribute(
            doc_.allocate_attribute(doc_.allocate_string("xmlns"), rootNs, sizeof("xmlns") - 1));
      }
    }

    void beginElement(const ElementId &n) {
      size_t nameRawLength = n.name().nameLength();
      rapidxml::xml_node<> *newNode =
        doc_.allocate_node(
          rapidxml::node_element,
          doc_.allocate_string(n.name().name(), nameRawLength),
          0,
          nameRawLength);
      node_->append_node(newNode);
      node_ = newNode;
      if (n.attr())
        setAttribute(n.attr()->first,n.attr()->second);
    }

    void setAttribute(QName const& n, std::string const& v) {
      TRACE(priv::XmlWriterImpl,"SET ATTRIBUTE:" << n.name()
        << "=\"" << v << "\"");
      size_t nameRawLength = n.nameLength();
      size_t valRawLength = v.size();
      node_->append_attribute(doc_.allocate_attribute(
        doc_.allocate_string(n.name(), nameRawLength),
        doc_.allocate_string(v.c_str(), valRawLength),
        nameRawLength,
        valRawLength));
    }

    void setAttribute(QName const& n, UnicodeString const& v) {
      std::string str;
      v.toUTF8String(str);
      setAttribute(n, str);
    }

    void endElement() {
      rapidxml::xml_node<> *parentNode = node_->parent();
      if (parentNode)
        node_ = parentNode;
    }

    void addAnyElement(boost::shared_ptr<AnyElementImpl> const& v) {
      TRACE(qx::syb::priv::XmlWriterImpl,"ADD TEXT:");
      assert(node_);
      const std::string &buf = v->buf();
      rapidxml::xml_document<char> doc;
      doc.parse<rapidxml::parse_fastest>(const_cast<char *>(buf.c_str()));
      rapidxml::xml_node<> *node = doc.first_node();
      if (node && (node = doc_.clone_node(node))) {
        rapidxml::xml_node<> *parentNode = node_->parent();
        if (parentNode && parentNode->name_size() == node->name_size() &&
            !memcmp(parentNode->name(), node->name(), node->name_size())) {
          parentNode->remove_node(node_);
          parentNode->append_node(node);
        } else {
          node_->append_node(node);
        }
      }
    }

    void xmlToLower(rapidxml::xml_node<> *node) {
      for (rapidxml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) {
        std::transform(child->name(), child->name() + child->name_size(), child->name(), ::tolower);
        for (rapidxml::xml_attribute<> *attr = child->first_attribute(); attr; attr = attr->next_attribute()) {
          attr->name()[0] = ::tolower(attr->name()[0]);
          if (strncmp(attr->name(), "type", attr->name_size()) == 0) {
            std::transform(attr->value(), attr->value() + attr->value_size(), attr->value(), std::tolower);
          }
        }
        xmlToLower(child);
      }
    }

    void flush(std::ostream& o, bool isIndent, bool isLowerCase) {
      TRACE(qx::syb::priv::XmlWriterImpl,"FLUSH");
      if (isLowerCase)
        xmlToLower(&doc_);
      rapidxml::print(o, doc_, isIndent ? 0 : rapidxml::print_no_indenting);
      o << std::flush;
    }
  };

}

namespace qx {
  namespace syb {
    bool operator==(QName const& l, QName const& r) {
      return l.name() == r.name() && l.ns() == r.ns();
    }

    bool operator==(ElementId const& l, ElementId const& r) {
      if (!(l.name() == r.name()))
        return false;
      if (!l.attr() && !r.attr())
        return true;
      if (l.attr() && r.attr()) {
        return l.attr()->first == r.attr()->first
          && l.attr()->second == r.attr()->second;
      }
      return false;
    }

    namespace priv {

      XmlWriterImpl::XmlWriterImpl() {}

      XmlWriterImpl::~XmlWriterImpl(){}

      XmlWriterImpl::XmlWriterImpl(const XmlWriterImpl& ){
      }

      void XmlWriterImpl::beginElement(ElementId const& n) {
        TRACE_ENTER(XmlWriterImpl,">BEGIN ELEMENT:" << n.name().name());
        impl_->beginElement(n);
      }

      void XmlWriterImpl::endElement() {
        TRACE_EXIT(XmlWriterImpl,"<END ELEMENT" << impl_->node_->name());
        impl_->endElement();
      }

      void XmlWriterImpl::setBody(std::string const& v) {
        TRACE(XmlWriterImpl,"SET BODY:" << v);
        size_t valRawLength = v.size();
        rapidxml::xml_node<> *dataNode_ =
          impl_->doc_.allocate_node(rapidxml::node_data);
        dataNode_->value(impl_->doc_.allocate_string(v.c_str(), valRawLength),
                         valRawLength);
        impl_->node_->append_node(dataNode_);
      }

      void XmlWriterImpl::setBody(std::vector<unsigned char> const& v) {
        TRACE(XmlWriterImpl,"SET BODY for array<uint>");

        rapidxml::xml_node<> *dataNode =
          impl_->doc_.allocate_node(rapidxml::node_cdata);
        impl_->node_->append_node(dataNode);

        if (!v.empty()) {
          std::size_t size = ((v.size() * 4) / 3) + (v.size() / 96) + 6;
          char *base64Encoded = impl_->doc_.allocate_string(0, size);

          base64_encodestate state;
          base64_init_encodestate(&state);
          int length = base64_encode_block(reinterpret_cast<const char*>(&v[0]),
                                           v.size(), base64Encoded, &state);
          length += base64_encode_blockend(base64Encoded + length, &state);

          dataNode->value(base64Encoded, length - 1);
        }
      }

      void XmlWriterImpl::setBody(UnicodeString const& v) {
        std::string str;
        v.toUTF8String(str);
        TRACE(XmlWriterImpl,"SET BODY:" << str.c_str());
        setBody(str);
      }

      void XmlWriterImpl::setAttribute(QName const& n, std::string const& v) {
        impl_->setAttribute(n,v);
      }

      void XmlWriterImpl::setAttribute(QName const& n ,UnicodeString const& v) {
        impl_->setAttribute(n,v);
      }

      void XmlWriterImpl::setRootNamespace(const char *rootNs) {
        impl_->setRootNamespace(rootNs);
      }

      void XmlWriterImpl::flush(std::ostream& o, bool isIndent, bool isLowerCase) {
        TRACE(XmlWriterImpl,"FLUSH");
        impl_->flush(o, isIndent, isLowerCase);
      }

      bool XmlWriterImpl::isComputed(cfg::key_t v) {
        return impl_->computed_.find(v) != impl_->computed_.end();
      }

      void XmlWriterImpl::setComputed(cfg::key_t v) {
        impl_->computed_.insert(v);
      }

      void XmlWriterImpl::addAnyElement(boost::shared_ptr<AnyElementImpl> const& n) {
        impl_->addAnyElement(n);
      }
    }
  }
}
