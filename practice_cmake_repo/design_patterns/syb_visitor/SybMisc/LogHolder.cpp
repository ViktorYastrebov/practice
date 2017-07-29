#include "LogHolder.hpp"

bool LogHolder::isSeverityEnabled(SEVERITY severity) {
  return severity_ <= severity;
}

void LogHolder::Log(SEVERITY severity, const std::string  &type, const std::string  &msg) {
  if (severity <= severity) {
    std::string toLog = type + msg;
    logs_.push_back(toLog);
  }
}

void LogHolder::LogExit(SEVERITY severity, const std::string  &type, const std::string  &msg, const std::string  &functionName) {
  if (severity_ <= severity) {
    logs_.push_back(functionName);
  }
  Log(severity, type, msg);
}

void LogHolder::LogEnter(SEVERITY severity, const std::string  &type, const std::string  &msg, const std::string  &functionName) {
  if (severity_ <= severity) {
    logs_.push_back(functionName);
  }
  Log(severity, type, msg);
}