#pragma once

#include <string>
#include <memory>

#include "oci.h"

/*
class Connection {
public:
  Connection(const std::string  &user, const std::string  &pwd, const std::string  &serverName = "");
  virtual ~Connection();

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
*/
namespace oracle_oci {
  class Env {
  public:
    Env() {
      sword result = OCI_SUCCESS;
      result = OCIEnvCreate(&env_, OCI_DEFAULT, (dvoid *)0, 0, 0, 0, (size_t)0, (dvoid **)0);
      if (result != OCI_SUCCESS) {
        std::string error("OCIEnvCreate failed with error:");
        error += std::to_string(result);
        throw std::runtime_error(error);
      }
    }
    virtual ~Env() {
      OCIHandleFree(env_, OCI_HTYPE_ENV);
    }
    const OCIEnv  *get() const;
    OCIEnv  *get();

  private:
    OCIEnv  *env_;
  };

  class Error : public Env {
  public:
    Error() 
      :Env()
    {
      sword result = OCIHandleAlloc(static_cast<const void*>(Env::get()), (void  **)&errhp_, (ub4)OCI_HTYPE_ERROR, 0, (dvoid **)0);
      if (result != OCI_SUCCESS) {
        std::string error("Creation error handler is failer with error: ");
        error += std::to_string(result);
        throw std::runtime_error(error);
      }
    }
    virtual ~Error() {
      OCIHandleFree(errhp_, OCI_HTYPE_ERROR);
    }
    const OCIError  *get() const;
    OCIError  *get();
  private:
    OCIError  *errhp_;
  };

  class Server : public Env {
  public:

  };

  class connection {
  public:
    explicit connection(const std::string  &user, const std::string  &pwd, const std::string server);
  };
}