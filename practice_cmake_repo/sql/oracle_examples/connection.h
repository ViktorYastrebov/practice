#pragma once

#include <string>
#include "oci.h"

class Connection {
public:
  Connection(const std::string  &user, const std::string  &pwd, const std::string  &serverName = "");
  virtual ~Connection();

  Connection(const Connection  &con) = delete;
  Connection &  operator = (const Connection  &con) = delete;
  Connection(Connection  &&con) = default;

  OCIError  *getError();
  const OCIError  *getError() const;

  OCISvcCtx  *getContext();
  const OCISvcCtx  *getContext() const;

  OCIEnv  *getEnvironment();
  const OCIEnv  *getEnvironment() const;

  std::string getErrorMessage(sword status);

private:
  OCIEnv  *env;
  OCIError  *errhp;
  OCIServer  *server;
  OCISvcCtx  *svcCtx;
  OCISession  *session;
};
