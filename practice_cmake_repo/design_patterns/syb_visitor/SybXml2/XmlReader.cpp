#include "XmlReader.h"

#include <algorithm>
#include <istream>
#include <list>

#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>

#include <rapidxml.hpp>

#if 0
#include "AnyElementImpl.h"
#include "Syb/SybAll.h"
#include "Syb/StriCmpAZ.h"
#include "Syb/GenericDiff.h"
#include "Syb/GenericPatch.h"
#else
#include "AnyElementImpl.h"
#include "../Syb/SybAll.h"
#include "../Syb/StriCmpAZ.h"
#include "../Syb/GenericDiff.h"
#include "../Syb/GenericPatch.h"
#endif

extern "C" {
#include "cdecode.h"
}

using namespace qx;
using namespace qx::syb;
using namespace qx::syb::diff;
using namespace qx::syb::priv;

namespace {
  bool isEqualCaseSensitive(const char *p1, std::size_t size1, const char *p2, std::size_t size2) {
    if (size1 != size2)
        return false;
    return !memcmp(p1, p2, size1);
  }

  bool isEqualCaseInsensitive(const char *p1, std::size_t size1, const char *p2, std::size_t size2) {
    if (size1 != size2)
        return false;
    while ( --size1 && (*(unsigned char *)p1 & 0xDF) == (*(unsigned char *)p2 & 0xDF)) {
      p1 = (char *)p1 + 1;
      p2 = (char *)p2 + 1;
    }
    return !((*(unsigned char *)p1 & 0xDF) - (*(unsigned char *)p2 & 0xDF));
  }
}

namespace Loki {
  template<>
  struct ImplOf<qx::syb::priv::XmlReaderImpl> {
    std::vector<char> xmlBuffer_;
    rapidxml::xml_document<char> doc_;
    std::vector<rapidxml::xml_node<> *> state_;
    mutable bool hasXmlIndependentOrder_;
    bool (*isEqualFunc_)(const char *p1, std::size_t size1, const char *p2, std::size_t size2);
    ImplOf() : hasXmlIndependentOrder_(false), isEqualFunc_(isEqualCaseSensitive) {
    }

    void init(std::istream &is, bool isPreprocess) {
      TRACE(XmlReaderImpl,"init from stream");

      is.unsetf(std::ios::skipws);
      xmlBuffer_.assign(std::istreambuf_iterator<char>(is),
                        std::istreambuf_iterator<char>());
      if (is.fail() || is.bad())
        throw std::runtime_error("Error reading XML stream");
      xmlBuffer_.push_back(0);

      try {
        if (isPreprocess)
          doc_.parse<rapidxml::parse_default>(&xmlBuffer_[0]);
        else
          doc_.parse<0>(&xmlBuffer_[0]);
      } catch (rapidxml::parse_error &e) {
        throw std::runtime_error(std::string("Error parsing XML stream : ") + e.what());
      }

      state_.push_back(doc_.first_node());
      traceState();
    }

    void init(AnyElementImpl& i) {
      TRACE(XmlReaderImpl,"init from any");
      state_.push_back(i.node());
    }

    bool checkAttr(const std::pair<QName,std::string> &v,
                   rapidxml::xml_node<> *node) {
      const char *attrName = v.first.name();
      size_t attrNameLenght = v.first.nameLength();
      const char *attrVal = v.second.c_str();
      size_t attrValLenght = v.second.size();
      TRACE(XmlReaderImpl, "Checking an attribute: " << attrName << " == " << attrVal);
      for (rapidxml::xml_attribute<> *attr = node->first_attribute();
           attr; attr = attr->next_attribute()) {
        if (isEqualFunc_(attr->name(), attr->name_size(), attrName, attrNameLenght) &&
            isEqualCaseInsensitive(attr->value(), attr->value_size(), attrVal, attrValLenght)) {
          TRACE(XmlReaderImpl, "found");
          return true;
        }
      }
      TRACE(XmlReaderImpl, "not found");
      return false;
    }

    bool checkNextElement(const ElementId &n) {
      assert(!state_.empty());
      rapidxml::xml_node<> *curNode = state_.back();
      if (curNode) {
        const char *name = n.name().name();
        size_t nameLenght = n.name().nameLength();
        const char *nsPrefix = n.name().nsPrefix();
        if (nsPrefix && nsPrefix[0] != 0) {
          size_t nsPrefixLenght = strlen(nsPrefix);
          size_t fullNameLenght = nameLenght + nsPrefixLenght + 1;
          char *fullName = doc_.allocate_string(0, fullNameLenght);
          memcpy(fullName, nsPrefix, nsPrefixLenght);
          fullName[nsPrefixLenght] = ':';
          memcpy(fullName + nsPrefixLenght + 1, name, nameLenght);
          name = fullName;
          nameLenght = fullNameLenght;
        }

        if (isEqualFunc_(curNode->name(), curNode->name_size(), name, nameLenght))
          if (!n.attr() || checkAttr(*n.attr(), curNode))
            return true;
        if (hasXmlIndependentOrder_)
          for (rapidxml::xml_node<> *node = curNode->next_sibling();
               node; node = node->next_sibling()) {
            if (isEqualFunc_(node->name(), node->name_size(), name, nameLenght)) {
              rapidxml::xml_node<> *clonedNode = doc_.clone_node(node, 0);
              rapidxml::xml_node<> *parentNode = curNode->parent();
              parentNode->remove_node(node);
              parentNode->insert_node(curNode, clonedNode);
              state_.back() = clonedNode;
              if (!n.attr() || checkAttr(*n.attr(), node))
                return true;
              break;
            }
          }
      }
      return false;
    }


    void beginElement(const ElementId &n) {
      assert(!state_.empty());

      TRACE_ENTER(XmlReaderImpl,"BEGIN ELEMENT: " << n.name().name()
        << " / " << state_.size());
      traceState();

      rapidxml::xml_node<> *curNode = state_.back();
      if (curNode == NULL) {
        std::stringstream msg;
        msg << "Wrong XML element: expected <" << n.name() << ">";
        throw std::runtime_error(msg.str());
      }

      TRACE(XmlReaderImpl," ON " << curNode->name());

      state_.push_back(curNode->first_node());
      traceState();
    }

    void iterNext() {
      assert(!state_.empty());
      rapidxml::xml_node<> *node = state_.back();
      if (node)
        state_.back() = node->next_sibling();
    }

    void beginElement() {
      assert(!state_.empty());
      rapidxml::xml_node<> *curNode = state_.back();
      if (curNode == NULL) {
        std::stringstream msg;
        msg << "Wrong XML element: expected any element";
        std::cerr << "ERROR: " << msg.str() << std::endl;
        throw std::runtime_error(msg.str()); // TODO: better error
      }
      TRACE(XmlReaderImpl," ON " << curNode->name());
      state_.push_back(curNode->first_node());
      traceState();
    }

    void endElement() {
      assert(!state_.empty());
      TRACE_EXIT(XmlReaderImpl, "END ELEMENT " << state_.size() << "/" << this);
      traceState();
      state_.pop_back();
      traceState();
      iterNext();
      traceState();
    }

    boost::shared_ptr<AnyElementImpl> getAnyElement() {
      assert(!state_.empty());
      TRACE(XmlReaderImpl,"GET ANY ELEMENT:" << this << "/" << state_.size());
      traceState();

      rapidxml::xml_node<> *node = state_.back();
      size_t size = state_.size();
      if (!node && size > 1)
        node = state_[size - 2];

      return boost::make_shared<AnyElementImpl>(node);
    }

    std::string getBody() {
      std::string ret;
      assert(!state_.empty());
      rapidxml::xml_node<> *dataNode = state_.back();
      if (dataNode) {
          ret.assign(dataNode->value(), dataNode->value_size());
          TRACE(XmlReaderImpl,"GET BODY text:" << ret <<
            "/" << state_.size() << "/" << this);
      }
      return ret;
    }

    void getBody(std::vector<unsigned char> &v) {
      assert(!state_.empty());
      rapidxml::xml_node<> *dataNode = state_.back();
      if (dataNode) {
        const char *base64Encoded = dataNode->value();
        size_t base64EncodedSize = dataNode->value_size();
          if (base64EncodedSize) {
            base64_decodestate decState;
            base64_init_decodestate(&decState);
        
            boost::scoped_array<char> base64Decoded(new char[base64EncodedSize]);
        
            int length = base64_decode_block(base64Encoded, base64EncodedSize,
              base64Decoded.get(), &decState);
        
            v.assign(base64Decoded.get(), base64Decoded.get() + length);
          }
      }
    }

    UnicodeString getUnicodeBody() {
      assert(!state_.empty());
      UnicodeString ret;
      rapidxml::xml_node<> *dataNode = state_.back();
      if (dataNode) {
        const char *v = dataNode->value();
        if (v) {
          TRACE(XmlReaderImpl,"GET UNICODE BODY:" << ret << "/"
            << state_.size() << "/" << this);
          ret = UnicodeString(v, dataNode->value_size(), "UTF-8");
        }
      }
      return ret;
    }

    std::string getAttribute(const QName &n) {
      std::string ret;
      rapidxml::xml_node<> *node = 0;
      int size = state_.size();
      if (size > 1)
        node = *(state_.rbegin() + 1);
      else if (size == 1)
        node = state_.back();
      if (node) {
        const char *name = n.name();
        size_t nameLenght = n.nameLength();
        for (rapidxml::xml_attribute<> *attr = node->first_attribute();
             attr; attr = attr->next_attribute())
          if (isEqualFunc_(attr->name(), attr->name_size(), name, nameLenght)) {
            ret.assign(attr->value(), attr->value_size());
            break;
          }
      }
      return ret;
    }

    boost::optional<std::string> getOptionalAttribute(const QName &n) {
      rapidxml::xml_node<> *node = 0;
      int size = state_.size();
      if (size > 1)
        node = *(state_.rbegin() + 1);
      else if (size == 1)
        node = state_.back();
      if (node) {
        const char *name = n.name();
        size_t nameLenght = n.nameLength();
        for (rapidxml::xml_attribute<> *attr = node->first_attribute();
             attr; attr = attr->next_attribute())
          if (isEqualFunc_(attr->name(), attr->name_size(), name, nameLenght))
            return std::string(attr->value(), attr->value_size());
      }
      return boost::optional<std::string>();
    }    

    void loadAttributes(std::list<std::pair<std::string, UnicodeString> > &v) {
      rapidxml::xml_node<> *node = 0;
      int size = state_.size();
      if (size > 1)
        node = *(state_.rbegin() + 1);
      else if (size == 1)
        node = state_.back();
      for (rapidxml::xml_attribute<> *attr = node->first_attribute();
           attr; attr = attr->next_attribute()) {
        v.push_back(std::make_pair(std::string(attr->name(), attr->name_size()),
                    UnicodeString(attr->value(), attr->value_size(), "UTF-8")));
      }
    }

    std::string currentElementLocalName() {
      assert(!state_.empty());
	  rapidxml::xml_node<> *node = state_.back();
	  if (!node && state_.size() > 1) {
		node = state_[state_.size() - 2];
	  }
      return std::string(node->name(), node->name_size());
    }

    QName getElementName() {
      assert(!state_.empty());
      const char *name = state_.back()->name();
      size_t nameSize = state_.back()->name_size();
      const char *it = (const char*)memchr(name, ':', nameSize);
      if (it) {
        nameSize = nameSize - (it - name) - 1;
        name = it + 1;
      }
      return QName(name, nameSize);
    }

    UnicodeString getUnicodeAttribute(const QName &n) {
      std::string attrVal = getAttribute(n);
      return UnicodeString::fromUTF8(attrVal);
    }

    boost::optional<UnicodeString> getOptionalUnicodeAttribute(const QName &n) {
      boost::optional<std::string> attrVal = getOptionalAttribute(n);
      return attrVal ? UnicodeString::fromUTF8(attrVal.get()) : boost::optional<UnicodeString>();
    }

    std::string getNextElAttribute(const QName &n) {
      assert(!state_.empty());
      TRACE(XmlReaderImpl,"GET ATTRIBUTE OF INNER ELEMENT:" << n.name());
      std::string ret;
      rapidxml::xml_node<> *node = state_.back();
      if (node) {
        const char *name = n.name();
        size_t nameLenght = n.nameLength();
        for (rapidxml::xml_attribute<> *attr = node->first_attribute();
             attr; attr = attr->next_attribute())
          if (isEqualFunc_(attr->name(), attr->name_size(), name, nameLenght)) {
            ret.assign(attr->value(), attr->value_size());
            break;
          }
      }
      return ret;
    }

    //just for debug purposes
    void traceState() {
    }
  };

  typedef ImplOf<qx::syb::priv::XmlReaderImpl> impl_t;
}

namespace qx {
  namespace syb {
    namespace priv {

      XmlReaderImpl::XmlReaderImpl(std::istream& i, bool isPreprocess) {
        impl_->init(i, isPreprocess);
      }

      XmlReaderImpl::XmlReaderImpl(boost::shared_ptr<AnyElementImpl> const& o){
        if (o && o->node()) impl_->init(*o);
      }

      XmlReaderImpl::~XmlReaderImpl() {
      }

      void XmlReaderImpl::beginElement(ElementId const& n) {
        impl_->beginElement(n);
      }

      void XmlReaderImpl::beginElement() {
        impl_->beginElement();
      }

      void XmlReaderImpl::loadAttributes(std::list<std::pair<std::string,UnicodeString> >& v) {
        impl_->loadAttributes(v);
      }

      boost::shared_ptr<AnyElementImpl> XmlReaderImpl::getAnyElement() {
        return impl_->getAnyElement();
      }

      bool XmlReaderImpl::checkNextElement(ElementId const& n) {
        return impl_->checkNextElement(n);
      }

      bool XmlReaderImpl::setUnpos(bool save) const {
        bool prevSave = impl_->hasXmlIndependentOrder_;
        impl_->hasXmlIndependentOrder_ = save;
        return prevSave;
      }

      void XmlReaderImpl::setCaseSensitive(bool isCaseSensitive) {
        impl_->isEqualFunc_ = isCaseSensitive ? isEqualCaseSensitive : isEqualCaseInsensitive;
      }

      void XmlReaderImpl::endElement() {
        impl_->endElement();
      }

      std::string XmlReaderImpl::getBody() {
        return impl_->getBody();
      }

      void XmlReaderImpl::getBody(std::vector<unsigned char>& v) {
        return impl_->getBody(v);
      }

      UnicodeString XmlReaderImpl::getUnicodeBody() {
        return impl_->getUnicodeBody();
      }

      std::string XmlReaderImpl::getNextElAttribute(QName const& n) {
        return impl_->getNextElAttribute(n);
      }

      std::string XmlReaderImpl::getAttribute(QName const& n) {
        return impl_->getAttribute(n);
      }

      QName XmlReaderImpl::getElementName() {
        return impl_->getElementName();
      }

      UnicodeString XmlReaderImpl::getUnicodeAttribute(QName const& n) {
        return impl_->getUnicodeAttribute(n);
      }

      boost::optional<UnicodeString> XmlReaderImpl::getOptionalUnicodeAttribute(QName const& n) {
        return impl_->getOptionalUnicodeAttribute(n);
      }

      void XmlReaderImpl::flush() {
      }

      bool XmlReaderImpl::hasNext() {
        assert(!impl_->state_.empty());
        return impl_->state_.back() != 0;
      }

      void XmlReaderImpl::skipElement() {
        impl_->iterNext();
      }

      std::string XmlReaderImpl::currentElementLocalName() {
        return impl_->currentElementLocalName();
      }

    }

    void XmlOptions::action(priv::XmlReaderImpl &r) const {}
    XmlOptions::~XmlOptions() {}

    void CaseSensitive::action(XmlReaderImpl &r) const {
      r.setCaseSensitive(true);
    }

    void CaseInsensitive::action(XmlReaderImpl &r) const {
      r.setCaseSensitive(false);
    }

    QName defaultPlural(QName const& v) {
      std::string tmpstr = v.name();
      tmpstr.append("s");
      return QName(tmpstr, v.ns(), v.nsPrefix(), true);
    }

  }
}
