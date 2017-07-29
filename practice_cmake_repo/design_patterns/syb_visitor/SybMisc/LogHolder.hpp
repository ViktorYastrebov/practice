#pragma once

#include <string>
#include <list>

#include <loki/Singleton.h>

class LogHolder {
public:
  enum SEVERITY {
    ERROR_SEVERITY,
    WARN_SEVERITY,
    INFO_SEVERITY,
    TRACE_SEVERITY
  };
  bool isSeverityEnabled(SEVERITY severity);
  void Log(SEVERITY severity, const std::string  &type, const std::string  &msg);

  void LogExit(SEVERITY severity, const std::string  &type, const std::string  &msg, const std::string  &functionName);
  void LogEnter(SEVERITY severity, const std::string  &type, const std::string  &msg, const std::string  &functionName);
private:
  std::list<std::string>  logs_;
  SEVERITY severity_ = TRACE_SEVERITY;
};

typedef Loki::SingletonHolder<LogHolder, Loki::CreateStatic> LogSingleton;