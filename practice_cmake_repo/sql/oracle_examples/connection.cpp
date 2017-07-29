#include <exception>
#include "connection.h"


Connection::Connection(const std::string  &user, const std::string  &pwd, const std::string  &serverName)
{
  //it can be implemented as chain reposnibility
  sword result = OCI_SUCCESS;
  result = OCIEnvCreate(&env, OCI_DEFAULT, (dvoid *)0, 0, 0, 0, (size_t)0, (dvoid **)0);
  if (result != OCI_SUCCESS) {
    std::string error("OCIEnvCreate failed with error:");
    error += std::to_string(result);
    throw std::runtime_error(error);
  }
  result = OCIHandleAlloc(static_cast<const void*>(env), (void  **)&errhp, (ub4)OCI_HTYPE_ERROR, 0, (dvoid **)0);
  if (result != OCI_SUCCESS) {
    std::string error("Creation error handler is failer with error: ");
    error += std::to_string(result);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(error);
  }
  result = OCIHandleAlloc(static_cast<const void*>(env), (void **)&server, (ub4)OCI_HTYPE_SERVER, 0, (dvoid **)0);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(error);
  }

  result = OCIServerAttach(server, errhp, (const OraText  *)serverName.c_str(), (sb4)serverName.length(), (ub4)OCI_DEFAULT);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(error);
  }

  result = OCIHandleAlloc((const void *)env, (dvoid **)&svcCtx, (ub4)OCI_HTYPE_SVCCTX, 0, (dvoid **)0);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(getErrorMessage(result));
  }

  result = OCIAttrSet((dvoid *)svcCtx, OCI_HTYPE_SVCCTX, (dvoid *)server, (ub4)0, OCI_ATTR_SERVER, (OCIError *)errhp);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);
    result = OCIHandleFree(svcCtx, OCI_HTYPE_SVCCTX);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(error);
  }

  result = OCIHandleAlloc(static_cast<const void*>(env), (dvoid **)&session, (ub4)OCI_HTYPE_SESSION, (size_t)0, (dvoid **)0);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);
    result = OCIHandleFree(svcCtx, OCI_HTYPE_SVCCTX);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(getErrorMessage(result));
  }
  result = OCIAttrSet((dvoid *)session, (ub4)OCI_HTYPE_SESSION, (dvoid *)user.c_str(), (ub4)user.length(), (ub4)OCI_ATTR_USERNAME, errhp);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);
    result = OCIHandleFree(svcCtx, OCI_HTYPE_SVCCTX);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(error);
  }
  result = OCIAttrSet((dvoid *)session, (ub4)OCI_HTYPE_SESSION, (dvoid *)pwd.c_str(), (ub4)pwd.length(), (ub4)OCI_ATTR_PASSWORD, errhp);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);
    result = OCIHandleFree(svcCtx, OCI_HTYPE_SVCCTX);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(error);
  }
  result = OCISessionBegin(svcCtx, errhp, session, OCI_CRED_RDBMS, (ub4)OCI_DEFAULT);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);
    result = OCIHandleFree(svcCtx, OCI_HTYPE_SVCCTX);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(error);
  }

  result = OCIAttrSet((dvoid *)svcCtx, (ub4)OCI_HTYPE_SVCCTX, (dvoid *)session, (ub4)0, (ub4)OCI_ATTR_SESSION, errhp);
  if (result != OCI_SUCCESS) {
    std::string error = getErrorMessage(result);

    sword result = OCISessionEnd(svcCtx, errhp, session, 0);
    result = OCIHandleFree(session, OCI_HTYPE_SESSION);

    result = OCIHandleFree(svcCtx, OCI_HTYPE_SVCCTX);
    result = OCIHandleFree(server, OCI_HTYPE_SERVER);
    result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
    result = OCIHandleFree(env, OCI_HTYPE_ENV);
    throw std::runtime_error(error);
  }
}


Connection::~Connection() {
  sword result = OCISessionEnd(svcCtx, errhp, session, 0);
  result = OCIHandleFree(session, OCI_HTYPE_SESSION);

  result = OCIServerDetach(server, errhp, (ub4)OCI_DEFAULT);
  result = OCIHandleFree(svcCtx, OCI_HTYPE_SVCCTX);
  result = OCIHandleFree(server, OCI_HTYPE_SERVER);
  result = OCIHandleFree(errhp, OCI_HTYPE_ERROR);
  result = OCIHandleFree(env, OCI_HTYPE_ENV);
}

OCIError  *Connection::getError() {
  return errhp;
}

const OCIError  *Connection::getError() const {
  return errhp;
}

OCISvcCtx  *Connection::getContext() {
  return svcCtx;
}

const OCISvcCtx  *Connection::getContext() const {
  return svcCtx;
}

OCIEnv  *Connection::getEnvironment() {
  return env;
}

const OCIEnv  *Connection::getEnvironment() const {
  return env;
}


std::string Connection::getErrorMessage(sword status) {
  text errbuf[512];
  sb4 errcode = 0;

  if (status == OCI_ERROR || status == OCI_INVALID_HANDLE) {
    sword res = OCIErrorGet(static_cast<void*>(errhp), (ub4)1, nullptr, &errcode, (OraText  *)errbuf, sizeof(errbuf), OCI_HTYPE_ERROR);
    return std::string((const char  *)errbuf);
  }
  return std::string();
}