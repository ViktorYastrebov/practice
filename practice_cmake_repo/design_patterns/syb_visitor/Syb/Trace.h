/**
\brief Simple debug tracing facilities

It allows increasing and decreasing indent along with possibility to turn it
on or off at runtime within specified scope.

Scope is a type's name. Usually this is a type of a where the tracing is performed
but this might be any tag type.

Scope is a first parameter of tracing macroses.

It's possible to redefine prefix for a specified scope:

For example:

template<>
struct TraceName<qx::syb::priv::XmlReaderImpl> {
  inline static std::string const& value() {
    static std::string ret = "xml:";
    return ret;
  }
};

Moreover trace can be enabled or disabled at runtime for example with:

TraceOption<syb::priv::XmlReaderImpl>::instance().enable(true);

By default tracing is disabled by it can be enabled with 
TRACE_IS_ENABLED_BY_DEFAULT macros.

All tracing facilities are removed with macros NTRACE or NDEBUG definitions.

\todo

* Stack trace
* XML
* Postmortem log
* Some simple profiling information

*/
#ifndef SYB_TRACE_H
#define SYB_TRACE_H

#include <string>
#include <ostream>
#include <sstream>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#if 0
#include "misc/LogHolder.h"
#else
#include "../SybMisc/LogHolder.hpp"
#endif

template<typename T>
struct LogOption {
  inline LogOption() : enabledCount_(1) {}
  inline bool isEnabled() const { return enabledCount_ > 0; }
  inline void enable(bool v) { if (v) enable(); else disable(); }
  inline void enable()  { enabledCount_++; }
  inline void disable() { enabledCount_--; }
  inline static LogOption& instance() {
    static LogOption ret;
    return ret;
  }
private:
  int enabledCount_;
};

/**
Allows to redefine prefix for a provided scope.
*/
template<typename T, typename EnableT=void>
struct TraceName {
  inline static std::string const& value() {
    static std::string ret;
    return ret;
  }
};

template<typename T, typename EnableT=void>
struct LogName {
  inline static std::string const& value() {
    return TraceName<T, EnableT>::value();
  }
};

#define LOG(severity,t,n)                                                      \
  if (LogSingleton::Instance().isSeverityEnabled(severity)) {                  \
    if (LogOption<t>::instance().isEnabled()) {                                \
      LogHolder &logHolder = LogSingleton::Instance();                         \
      logHolder.Log(severity, LogName<t>::value().c_str(), n);                 \
    }                                                                          \
  }

#define LOG_ERROR(t,n) LOG(LogHolder::ERROR_SEVERITY, t, n)
#define LOG_WARN(t,n)  LOG(LogHolder::WARN_SEVERITY,  t, n)
#define LOG_INFO(t,n)  LOG(LogHolder::INFO_SEVERITY, t, n)
#define LOG_TRACE(t,n) LOG(LogHolder::TRACE_SEVERITY, t, n)

#define LOG_SCOPE(t,onEnter,onExit)                                           \
  if (LogSingleton::Instance().isSeverityEnabled(                             \
     LogHolder::INFO_SEVERITY)) {                                             \
    if (LogOption<t>::instance().isEnabled()) {                               \
      std::stringstream ssEnter;                                              \
      ssEnter << onEnter;                                                     \
      LogSingleton::Instance().LogEnter(                                      \
        LogHolder::INFO_SEVERITY,                                             \
        LogName<t>::value().c_str(),                                          \
        ssEnter.str(),                                                        \
        BOOST_PP_CAT(BOOST_PP_STRINGIZE(++),__FUNCTION__));                   \
    }                                                                         \
  }                                                                           \
  struct BOOST_PP_CAT(cf1e1d, __LINE__) { ~BOOST_PP_CAT(cf1e1d,__LINE__)() {  \
      if (LogSingleton::Instance().isSeverityEnabled(                         \
          LogHolder::INFO_SEVERITY)) {                                        \
        if (LogOption<t>::instance().isEnabled()) {                           \
          std::stringstream ssExit;                                           \
          ssExit << onExit;                                                   \
          LogSingleton::Instance().LogExit(                                   \
            LogHolder::INFO_SEVERITY,                                         \
            LogName<t>::value().c_str(),                                      \
            ssExit.str(),                                                     \
            funcName_);                                                       \
        }                                                                     \
      }                                                                       \
    }                                                                         \
    const char *funcName_;                                                    \
  } BOOST_PP_CAT(a69fd4,__LINE__); BOOST_PP_CAT(a69fd4,__LINE__).funcName_ = BOOST_PP_CAT(BOOST_PP_STRINGIZE(--),__FUNCTION__);

#ifndef NTRACE
#ifdef NDEBUG
#define NTRACE
#endif
#endif

#ifndef NTRACE

/**
Outputs a message n in scope of a type t and increase indent.
*/
#define TRACE_ENTER(t,n) if (TraceOption<t >::instance().isEnabled()) { \
  std::stringstream ss;                                                 \
  ss << TraceName<t >::value();                                         \
  Trace::instance().outIndent(ss);                                      \
  Trace::instance().incrIndent();                                       \
  ss << n << std::endl;                                                 \
  LOG_TRACE(t, ss.str());                                               \
}

/**
Outputs a message n in scope of type t.
*/
#define TRACE_EXIT(t,n) if (TraceOption<t>::instance().isEnabled()) {\
  std::stringstream ss;                                              \
  Trace::instance().decrIndent();                                    \
  ss << TraceName<t>::value();                                       \
  Trace::instance().outIndent(ss);                                   \
  ss << n << std::endl;                                              \
  LOG_TRACE(t, ss.str());                                            \
}

/**
Decreases indent and outputs a message n in scope of type t.
*/
#define TRACE(t,n) if (TraceOption<t>::instance().isEnabled()) {  \
  std::stringstream ss;                                           \
  ss << TraceName<t>::value();                                    \
  Trace::instance().outIndent(ss);                                \
  ss << n << std::endl;                                           \
  LOG_TRACE(t, ss.str());                                         \
}

/**
Indents all inner C++ scope and output message n on the scope exiting
*/
#define TRACE_SCOPE(t,n,msg) if (TraceOption<t>::instance().isEnabled()) {     \
  std::stringstream ss;                                                        \
  TRACE_ENTER(t, "> " << n << ":" << msg)                                      \
  ScopedTrace<t> scopedTrace(n);                                               \
  LOG_TRACE(t, ss.str());                                                      \
}
#else

#define TRACE_ENTER(t,n)
#define TRACE_EXIT(t,n)
#define TRACE(t,n)
#define TRACE_SCOPE(t,n,msg)

#endif

/**
\brief Provides facilities for changing tracing options at runtime.

Currently it allows only enable or disable trace log.
*/
template<typename T>
struct TraceOption
{
  inline TraceOption():enabledCount_(
#ifndef TRACE_IS_ENABLED_BY_DEFAULT
    0
#else 
    1
#endif
    ){}
  inline bool isEnabled() const { return enabledCount_ > 0; }
  inline void enable(bool v) { if (v) enable(); else disable(); }
  inline void enable()  { enabledCount_++; }
  inline void disable() { enabledCount_--; }
  inline static TraceOption& instance() {
    static TraceOption ret;
    return ret;
  }
private:
  int enabledCount_;
};

//#define TRACE_IS_ENABLED_BY_DEFAULT

/**
This enables trace for the current scope.
*/
template<typename T>
struct EnableTrace {
  inline EnableTrace() {
    TraceOption<T>::instance().enable();
  }
  inline ~EnableTrace() {
    TraceOption<T>::instance().disable();
  }
};

namespace qx {
namespace syb {
  std::ostream& nullOutput();
  std::ostream& stdcerrOutput();
  std::ostream& fileOutput(std::string const &param);
  std::ostream& lyciaOutput(std::string const &param);
}  // syb
}  // qx

template<typename T, typename EnableT=void>
struct TraceOutput {
  inline static std::ostream& value() {
    return qx::syb::stdcerrOutput();
  }
};

/** 
\brief Handles trace state

It usually isn't used by a user.
*/
struct Trace {
  inline Trace():indent_(0){}
  inline void incrIndent() {
    indent_++;
  }
  inline void decrIndent() {
    indent_--;
  }
  inline void outIndent(std::ostream& o) {
    if (indent_ < 0)
      o << "~";
    else for (int i = 0; i < indent_; i++) o << "\t";
  }
  inline static Trace& instance() {
    static Trace ret;
    return ret;
  }
private:
  int indent_;
};

/**
\brief Helper for enabling trace just for its current scope.
*/
template<typename TagT>
struct ScopedTrace {
  inline explicit ScopedTrace(std::string const& msg):msg_(msg) {}
  inline ~ScopedTrace() {
    TRACE_EXIT(TagT,"<" << msg_);
  }
private:
  std::string msg_;
};

#endif
