#include "ddl_stmt.hpp"

DDLStatement::DDLStatement(const std::shared_ptr<Connection>  &con)
  :SqlStatement(), con_(con)
{
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, con->getDBC(), &stmt_);
  if (ret != SQL_SUCCESS) {
    throw std::runtime_error("can't allocate stmt handle");
  }
}

DDLStatement::DDLStatement(const std::shared_ptr<Connection>  &con, const std::string  &sql)
  : DDLStatement{con}
{
  strStmt_ = sql;
  int ret = prepare();
  if (ret != SQL_SUCCESS) {
    throw std::runtime_error("error");
  }
}

DDLStatement::~DDLStatement() {
  SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
}

int DDLStatement::prepare()
{
  //I'm not sure it's valid for all cases
  SQLRETURN ret =  SQLPrepareA(stmt_, (SQLCHAR*)strStmt_.data(), strStmt_.length());
  return ret;
}

int DDLStatement::execute()
{
  SQLRETURN ret = SQLExecute(stmt_);
  return ret;
}

int DDLStatement::commit()
{
  //http://www.easysoft.com/developer/languages/c/examples/Transactions.html
  SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, con_->getDBC(), SQL_COMMIT);
  return ret;
}

void DDLStatement::setSql(const std::string  &query)
{
  strStmt_ = query;
}

const std::shared_ptr<Connection>  &DDLStatement::getConnection() const {
  return con_;
}

std::shared_ptr<Connection>  &DDLStatement::getConnection() {
  return con_;
}

const SQLHSTMT DDLStatement::getStmt() const {
  return stmt_;
}

SQLHSTMT DDLStatement::getStmt() {
  return stmt_;
}

std::string DDLStatement::getErrorMessage() const {
  SQLSMALLINT rec_id = 1;
  SQLCHAR  sql_state[6];
  SQLINTEGER native_error_code = 0;
  SQLCHAR error_msg[SQL_MAX_MESSAGE_LENGTH];
  SQLSMALLINT actual_msg_len = 0;

  SQLRETURN ret = SQLGetDiagRecA(SQL_HANDLE_STMT, stmt_, rec_id, sql_state, &native_error_code, error_msg, sizeof(error_msg), &actual_msg_len);
  if (ret == SQL_NO_DATA) {
    return std::string();
  }

  if (ret != SQL_SUCCESS) {
    throw std::logic_error("Can't get the error message");
  }
  return std::string(reinterpret_cast<const char*>(error_msg), actual_msg_len);
}