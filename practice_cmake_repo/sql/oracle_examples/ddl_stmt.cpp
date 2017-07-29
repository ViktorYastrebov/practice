#include <exception>

#include "ddl_stmt.h"

DDLSqlStmt::DDLSqlStmt(const std::shared_ptr<Connection>  &con)
  : SqlStatement(), con_(con)
{
  sword ret = OCIHandleAlloc(static_cast<const void*>(con_->getEnvironment()), (dvoid **)&stmt_, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
  if (ret != OCI_SUCCESS) {
    std::string error = con_->getErrorMessage(ret);
    throw std::runtime_error(error);
  }
}

DDLSqlStmt::DDLSqlStmt(const std::shared_ptr<Connection>  &con, const std::string  &sql)
  : DDLSqlStmt{ con }
{
  sql_ = sql;
  sword ret = prepare();
  if (ret != OCI_SUCCESS) {
    std::string error = con->getErrorMessage(ret);
    throw std::runtime_error(error);
  }
}

DDLSqlStmt::~DDLSqlStmt() {
  OCIHandleFree(stmt_, OCI_HTYPE_STMT);
}

sword DDLSqlStmt::prepare() {
  return OCIStmtPrepare(stmt_, con_->getError(), (OraText  *)sql_.c_str(), sql_.length() + 1, OCI_NTV_SYNTAX, OCI_DEFAULT);
}

void DDLSqlStmt::setSql(const std::string  &query) {
  sql_ = query;
}

sword DDLSqlStmt::execute() {
  OCISvcCtx  *ctx = con_->getContext();
  OCIError  *err = con_->getError();
  return OCIStmtExecute(ctx, stmt_, err, 1, 0, 0, 0, OCI_DEFAULT);
}

sword DDLSqlStmt::commit() {
  OCISvcCtx  *ctx = con_->getContext();
  OCIError  *err = con_->getError();
  return OCITransCommit(ctx, err, OCI_DEFAULT);
}

std::shared_ptr<Connection> DDLSqlStmt::getConnection() {
  return con_;
}

const std::shared_ptr<Connection> DDLSqlStmt::getConnection() const {
  return con_;
}

const OCIStmt  *DDLSqlStmt::getOCIStmt() const {
  return stmt_;
}

OCIStmt  *DDLSqlStmt::getOCIStmt() {
  return stmt_;
}

std::string  & DDLSqlStmt::getSqlString() {
  return sql_;
}

const std::string  & DDLSqlStmt::getSqlString() const {
  return sql_;
}