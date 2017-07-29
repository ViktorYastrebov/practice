/**
\brief Common types for XML reading/writing

Current Limitations:
  * Only XML elements are allowed for encoding MultiConstr and Container
  * Only one XML text encoded value may appear in one scope
  * Use scoped idioms for entering leaving elements
  * Recursive values allowed only for objects with history (otherwise it hangs)
  * Array operations are index based, i.e. deleting element from a middle will
    cause transfer all elements after it (we need more advanced diff or explicitly
    track changes).
  * Arrays of bool don't work correctly
  * Equal names of fields
  * No Any elements for objects with history
  * Variant and boost::optional don't work for non foldable types

Performance problems:

\todo More

*/
#ifndef SYB_XML_TYPES_H
#define SYB_XML_TYPES_H

#include <boost/format.hpp>
#include <boost/utility/in_place_factory.hpp>
#if 0
#include "Syb/SybAll.h"
#include "Syb/Enum.h"
#include "Syb/Any.h"
#else
#include "../Syb/SybAll.h"
#include "../Syb/Enum.h"
#include "../Syb/Any.h"
#endif
namespace qx {
  static const size_t MAX_QNAME_LOCAL_PART = 32;
  /**
  \brief Static option for providing default XML namespace

  The namespace provided only for XML elements. XML attributes always
  have empty namespace by default.
  */
  const char *defaultXmlNs();

  /**
  \brief Static option for providing default XML prefix
  */
  const char *defaultXmlNsPrefix();

  namespace syb {

    /**
    \brief Marks a type to be encoded as XML element

    \see XmlKind
    */
    struct XmlElementTag:Ret<XmlElementTag>{};

    /**
    \brief Marks a type to be encoded as XML attribute

    \see XmlKind
    */
    struct XmlAttributeTag:Ret<XmlAttributeTag>{};

    /**
    \brief Marks a type to be encoded as XML text

    \see XmlKind
    */
    struct XmlTextTag:Ret<XmlTextTag>{};

    /**
    \brief Returns encoding method for provided type
    */
    template<typename T, typename TagT,
      typename U = typename UnAlias<T>::type,
      typename EnableT=void
    >
    struct XmlKind:XmlElementTag{};

    template<typename T, typename TagT>
    struct XmlKind<T,TagT,T,typename boost::enable_if<IsPrimitive<T> >::type>
      :XmlTextTag{};

    template<typename T, typename U, typename TagT>
    struct XmlKind<T,TagT,boost::optional<U>,
      typename boost::enable_if<
        IsPrimitive<U>
      >::type>:XmlAttributeTag{};

    template<typename T, typename U, typename TagT>
    struct XmlKind<T,TagT,U,
      typename boost::enable_if<
        IsPrimitive<U>
      >::type>:XmlAttributeTag{};

    template<typename T, typename TagT, typename U>
    struct XmlKind<T,TagT,U,typename boost::enable_if<IsEnum<U> >::type>
      :XmlAttributeTag{};

    template<typename T, typename TagT, typename U>
    struct XmlKind<T,TagT,boost::optional<U>,
        typename boost::enable_if<IsEnum<U> >::type>
      :XmlAttributeTag{};

    template<typename U, typename EnableT = void>
    struct XmlPrim:IsPrimitive<U>{};

    /**
    \brief Qualified name
    */

    struct QName {
      QName(const QName &qname)
          : name_(qname.name_), nameLength_(qname.nameLength_), ns_(qname.ns_), nsPrefix_(qname.nsPrefix_), isOwner_(qname.isOwner_) {
        if (isOwner_)
          memcpy(nameInPlace_, qname.nameInPlace_, nameLength_ + 1);
      }

      QName() : name_(0), nameLength_(0), ns_(0), nsPrefix_(0), isOwner_(false) {
      }

      QName(const char *name, size_t length = 0, bool isOwner = false)
          : name_(name), nameLength_(length), ns_(0), nsPrefix_(0), isOwner_(isOwner) {
        init();
      }

      QName(const char *name, const char *ns, size_t length = 0, bool isOwner = false)
          : name_(name), nameLength_(strlen(name)), ns_(ns), nsPrefix_(0), isOwner_(isOwner) {
        init();
      }

      QName(const char *name, const char *ns, const char *nsPrefix, size_t length = 0, bool isOwner = false)
          : name_(name), nameLength_(strlen(name)), ns_(ns), nsPrefix_(nsPrefix), isOwner_(isOwner) {
        init();
      }

      QName(std::string const& name, bool isOwner = false)
          : name_(name.c_str()), nameLength_(name.length()), ns_(0), nsPrefix_(0), isOwner_(isOwner) {
        init();
      }

      QName(std::string const& name, const char *ns, bool isOwner = false)
          : name_(name.c_str()), nameLength_(name.length()), ns_(ns), nsPrefix_(0), isOwner_(isOwner) {
        init();
      }

      QName(std::string const& name, const char *ns, const char *nsPrefix, bool isOwner = false)
          : name_(name.c_str()), nameLength_(name.length()), ns_(ns), nsPrefix_(nsPrefix), isOwner_(isOwner) {
        init();
      }

      QName& operator = (const QName &rhs) {
        name_ = rhs.name_;
        nameLength_ = rhs.nameLength_;
        ns_ = rhs.ns_;
        nsPrefix_ = rhs.nsPrefix_;
        isOwner_ = rhs.isOwner_;
        if (isOwner_)
          memcpy(nameInPlace_, name_, nameLength_ + 1);
      }

      /**
      \brief Local name
      */
      const char *name() const {
        return isOwner_ ? nameInPlace_ : name_;
      }

      size_t nameLength() const {
        return nameLength_;
      }
      /**
      \brief Namespace URL
      */
      const char *ns() const {
        return ns_;
      }
      /**
      \brief Namespace prefix
      */
      const char *nsPrefix() const {
        return nsPrefix_;
      }
    private:
      void init() {
        if (!nameLength_)
           nameLength_ = strlen(name_);
        if (isOwner_) {
          if (nameLength_ + 1 <= MAX_QNAME_LOCAL_PART)
            memcpy(nameInPlace_, name_, nameLength_);
          else
            nameLength_ = 0;
          nameInPlace_[nameLength_] = '\0';
        }
      }

      const char *ns_;
      const char *nsPrefix_;
      bool isOwner_;
      size_t nameLength_;

      union {
        const char *name_;
        char nameInPlace_[MAX_QNAME_LOCAL_PART];
      };

      friend std::ostream& operator<<(std::ostream& f, QName const& v) {
        return f << (v.name_ ? std::string(v.name_, v.nameLength_) : v.nameInPlace_) << "(" << v.ns_ << "/" << v.nsPrefix_ << ")";
      }
    };

    struct NotDefinedNodeAttribute : NotDefined {
      static inline QName const& value() {
        static QName ret;
        return ret;
      }
    };

    template<typename T, typename TagT,
      typename U = typename UnAlias<T>::type,
      typename EnableT=void>
    struct NodeAttribute:Ret<NotDefinedNodeAttribute> {
    };

    /**
    \brief Returns name of a type for XML element
    */
    template<typename T, typename TagT=void, typename EnableT=void>
    struct XmlElementName {
      inline static QName const& value() {
        static QName ret(TypeName<T>::value(), defaultXmlNs(), defaultXmlNsPrefix());
        return ret;
      }
    };

    template<typename T, typename TagT>
    struct XmlElementName<boost::optional<T>,TagT>:XmlElementName<T,TagT>{};

    /**
    \brief Returns name of a type for XML element
    */
    template<typename TagT>
    struct XmlElementName<std::vector<unsigned char>, TagT> {
      inline static QName const& value() {
        static QName ret("Binary", defaultXmlNs(), defaultXmlNsPrefix());
        return ret;
      }
    };

    template<typename T, typename TagT, typename EnableT>
    struct XmlElementName<boost::shared_ptr<T>, TagT, EnableT> {
      inline static QName const& value() {
        static QName ret(XmlElementName<T,TagT>::value());
        return ret;
      }
    };

    template<typename T, typename TagT, typename EnableT>
    struct XmlElementName<boost::optional<T>, TagT, EnableT> {
      inline static QName const& value() {
        static QName ret(XmlElementName<T,TagT>::value());
        return ret;
      }
    };

    QName defaultPlural(QName const& v);

    template<typename T, typename TagT, typename EnableT>
    struct XmlElementName<std::list<T>, TagT, EnableT> {
      inline static QName const& value() {
        static QName ret(defaultPlural(XmlElementName<T,TagT>::value()));
        return ret;
      }
    };

    /**
    \brief Returns name of a type for XML attribute
    */
    template<typename T, typename TagT, typename EnableT=void>
    struct XmlAttributeName {
      inline static QName const& value() {
        static QName ret(TypeName<typename boost::remove_const<T>::type>::value());
        return ret;
      }
    };

    template<typename T, typename TagT>
    struct XmlAttributeName<boost::optional<T>, TagT>:XmlAttributeName<T,TagT>{};

    template<typename T, typename TagT, typename EnableT = void>
    struct XmlName:XmlElementName<T,TagT>{};

    template<typename T, typename TagT>
    struct XmlName<T,TagT,
        typename boost::enable_if<
          boost::is_base_of<XmlAttributeTag,XmlKind<T,TagT> >
        >::type
    >:XmlAttributeName<T,TagT>{};

    struct ElementId {
      typedef boost::optional<std::pair<QName,std::string> > attr_t;
      ElementId() {
      }
      explicit ElementId(QName const& n) : name_(n) {
      }
      explicit ElementId(const char *n) : name_(n) {
      }
      ElementId(QName const& n, QName const& attr, std::string const& attrVal)
        : name_(n), attr_(boost::in_place(QName(attr), attrVal)) {
      }
      ElementId(const char *n, const char *attr, std::string const& attrVal)
        : name_(n), attr_(boost::in_place(QName(attr), attrVal)) {
      }
      ElementId(const char *n, const char *attr, const char *attrVal)
        : name_(n), attr_(boost::in_place(QName(attr), attrVal)) {
      }

      QName const& name() const { return name_; }
      QName& name() { return name_; }
      attr_t const& attr() const { return attr_; }
      attr_t& attr() { return attr_; }
    private:
      //TODO: to const refs
      QName name_;
      boost::optional<std::pair<QName,std::string> > attr_;
    };
    bool operator==(QName const& l, QName const& r);
    bool operator==(ElementId const& l, ElementId const& r);
#if 0
    /**
    \brief Returns identifier of an object.

    It's required for specializing type of an object dispatching behavior.

    */
    template<
      typename T,
      typename TagT,
      typename U = typename UnAlias<T>::type,
      typename EnT = void>
    struct XmlElementId {
      /**
      \brief Returns preliminary Identity of an object. It's used
      for decision regarding type of an object without searching it
      in all children
      */
      inline static boost::optional<ElementId> const& fixedElementId() {
        static boost::optional<ElementId> ret
          = boost::optional<ElementId>(
            ElementId(XmlElementName<T,TagT>::value()));
        return ret;
      }

      struct type {
        /**
        \brief This refines type of an object after it's child type
        is known
        */
        typedef typename NodeAttribute<T,TagT>::type attr_t;
        inline ElementId operator()(QName const& e) const {
          return ElementId(
            XmlElementName<T,TagT>::value(),attr_t::value(),e.name());
        }
      };
    };

    /**
    One of implementations of XmlElementId
    */
    struct SetByChildName {
      inline ElementId operator()(QName const& v) const {
        return ElementId(v);
      }
    };

    template<typename T, typename U, typename TagT>
    struct XmlElementId<T,TagT,U
      ,typename boost::enable_if<mpl::and_<
        boost::is_base_of<NotDefined,typename NodeAttribute<T,TagT>::type>,
        mpl::and_<
          NotOpt<U>,
          IsMultiConstr<T>
        > >
      >::type>:Ret<SetByChildName>
    {
      inline static boost::optional<ElementId> const& fixedElementId() {
        static boost::optional<ElementId> ret = boost::none;
        return ret;
      }
    };

    template<typename T, typename U, typename TagT>
    struct XmlElementId<T,TagT,U
      ,typename boost::enable_if<mpl::not_<IsMultiConstr<T> >
      >::type>:Ret<SetByChildName>
    {
      inline static boost::optional<ElementId> const& fixedElementId() {
        static boost::optional<ElementId> ret
          = boost::optional<ElementId>(
            ElementId(XmlElementName<T,TagT>::value()));
        return ret;
      }

      struct type {
        inline ElementId operator()(QName const& e) const {
          return ElementId(XmlElementName<T,TagT>::value());
        }
      };
    };
#endif
    /**
    \brief Returns attribute for type specifying
    */
    struct TypeAttributeName {
      static QName const& value();
    };


    template<typename T, typename TagT, typename U>
    struct NodeAttribute<T,TagT,U,typename boost::disable_if<
      boost::is_same<T,U>
    >::type>:Ret<TypeAttributeName>{};

    /**
    \brief Static option which returns name for nil attribute
    (similar to xsi:nil)
    */
    QName const& getNilAttrName();

    template<typename T, typename TagT>
    inline ElementId const& getNilElementId() {
      static ElementId r =
        ElementId(XmlElementName<T,TagT>::value(),getNilAttrName(),"yes");
      return r;
    }

    /**
    \brief This trait marks types for which special containing XML element
    is not created.

    name clashes avoidance is user's responsibility
    */
    template<typename T, typename EnableT=void>
    struct SkipXmlElement:mpl::false_{};

    /**
    \brief Static option which returns name for identity attribute
    */
    QName const& getIdAttrName();

    /**
    \brief Static option which returns name for array index attribute
    */
    QName const& getIndexAttrName();

    /**
    \brief Static option which returns name for array size attribute
    */
    QName const& getSizeAttrName();

    /**
    \brief Static option which returns name for array size attribute
    */
    QName const& getRefAttrName();

    /**
    \brief Returns name of an element for encoding array's
      item in delta updates
    */
    QName const& getArrayItemElement();

    /**
    \brief Static option for wrapping resized arrays
    (similar to xsi:nil)
    */
    QName const& getArrayResizeElement();

    template<bool isMultiConstrT>
    struct TypeIdPolicy;

    struct XmlElementTypeReader {
      virtual std::string getAttribute(QName const& v) = 0;
      virtual std::string getNextElAttribute(QName const& v) = 0;
      virtual QName getElementName() = 0;
    };

    template<>
    struct TypeIdPolicy<true>{
      virtual ElementId elementId(QName const& baseName, QName const& identity) const = 0;
      virtual std::string getTypeId(XmlElementTypeReader& r) const = 0;
    };

    template<>
    struct TypeIdPolicy<false>{
      virtual ElementId elementId(QName const& baseName) const {
        return ElementId(baseName);
      }
      inline static TypeIdPolicy<false> const& instance() {
        static TypeIdPolicy<false> ret = TypeIdPolicy<false>();
        return ret;
      }
    };

    template<bool isMultiConstrT> struct AnyElementTypeId;

    template<typename TagT>
    struct XmlReader;

    template<typename TagT>
    struct XmlWriter;

    struct AbsXmlInfo {
      inline AbsXmlInfo(
          QName const& name,
          ElementId const& nilElId,
          bool indOrd
          ) :name_(name)
          ,nilElementId_(nilElId)
          ,hasIndependentOrder_(indOrd)
      {}

      inline QName const& getName() const {
        return name_;
      }

      inline ElementId const& getNilElementId() const {
        return nilElementId_;
      }

      inline bool hasIndependentOrder() const {
        return hasIndependentOrder_;
      }

      virtual size_t entityIndex() const = 0;

      /**
      \brief Reads an document into provided value

      The value should be initialized with an object of corresponding type
      */
      virtual bool read(XmlReader<void> const&, Any const&) const = 0;

      /**
      \brief Reads an element in it's unfolded format, i.e. an object doesn't
      have opening and closing XML tags
      */
      virtual bool readUnfolded(XmlReader<void> const&, Any const&) const = 0;

      /**
      \brief Writes a document into provided value
      */
      virtual bool write(XmlWriter<void> const&, Any const&) const = 0;

      /**
      \brief Writes an element in it's unfolded format, i.e. an object doesn't
      have opening and closing XML tags
      */
      virtual bool writeUnfolded(XmlWriter<void> const&, Any const&) const = 0;
    private:
      QName name_;
      ElementId nilElementId_;
      bool hasIndependentOrder_;
    };

    struct XmlConstrHandler {
      virtual bool read(ElementId const& c) const = 0;
    };


    template<bool isMultiConstrT>
    struct XmlInfo:AbsXmlInfo {

      static const bool isMultiConstr = isMultiConstrT;

      inline XmlInfo(
          QName const& name,
          ElementId const& nilElId,
          bool indOrd,
          TypeIdPolicy<isMultiConstrT> const& p
        ):AbsXmlInfo(name,nilElId,indOrd),typeIdPolicy_(p)
      {}

      inline TypeIdPolicy<isMultiConstrT> const& typeIdPolicy() const {
        return typeIdPolicy_;
      }

    private:
      TypeIdPolicy<isMultiConstrT> const& typeIdPolicy_;
    };

  }
}

#endif
