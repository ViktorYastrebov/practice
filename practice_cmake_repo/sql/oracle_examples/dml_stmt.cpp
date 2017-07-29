#include "dml_stmt.h"
#include "bind.hpp"
#include "define.hpp"


DMLSqlStmt::DMLSqlStmt(const std::shared_ptr<Connection>  &con, const std::string  &sql)
  : DDLSqlStmt{ con, sql }, bindPos_(0), defPos_(0)
{}

DMLSqlStmt::~DMLSqlStmt()
{}

sword DMLSqlStmt::fetch() {
  OCISvcCtx  *ctx = getConnection()->getContext();
  OCIError  *err = getConnection()->getError();
  return OCIStmtFetch2(DDLSqlStmt::getOCIStmt(), err, 1, OCI_FETCH_NEXT, 0, OCI_DEFAULT);
}

sword DMLSqlStmt::setBind(const std::shared_ptr<base_bind>  &bind) {
  sword ret = bind->applyBindToStmt(*this);
  ++bindPos_;
  return ret;
}

sword DMLSqlStmt::setDef(const std::shared_ptr<base_define>  &def) {
  sword ret = def->applyDefToStmt(*this);
  ++defPos_;
  return ret;
}

unsigned int DMLSqlStmt::getBindPos() const {
  return bindPos_ + 1;
}

unsigned int DMLSqlStmt::getDefPos() const {
  return defPos_ + 1;
}

void DMLSqlStmt::resetBindPos() {
  bindPos_ = 0;
}

void DMLSqlStmt::resetDefPos() {
  defPos_ = 0;
}
