#include "connection.hpp"
#include <stdexcept>

#include <boost/format.hpp>

Connection::Connection(const std::string  &user, const std::string  &pwd)
{}

Connection::Connection(const std::string  &dsn, const std::string  &uid, const std::string  &pwd)
{
  SQLRETURN ret = SQLAllocEnv(&env_);
  if (ret != SQL_SUCCESS) {
    throw std::runtime_error("can't allocate SQLHENV");
  }
  ret = SQLSetEnvAttr(env_, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3_80, SQL_IS_INTEGER);
  if (ret != SQL_SUCCESS) {
    SQLFreeEnv(env_);
    throw std::runtime_error("can't set ODBC 3.8 version");
  }
  ret = SQLAllocHandle(SQL_HANDLE_DBC, env_, &hdbc_);
  if (ret != SQL_SUCCESS) {
    SQLFreeEnv(env_);
    throw std::runtime_error("can't set HDBC");
  }

  std::string connectionString = boost::str(boost::format("DSN=%1%;UID=%2%;PWD=%3%") %  dsn % uid % pwd);
  const int buff_size = 256;
  SQLCHAR buff[buff_size];
  //std::copy(connectionString.begin(), connectionString.end(), buff);
  SQLSMALLINT  outLen = 0;
  ret = SQLDriverConnectA(hdbc_, NULL, (SQLCHAR*)connectionString.c_str(), SQL_NTS, buff, buff_size, &outLen, SQL_DRIVER_COMPLETE);

  if (!SQL_SUCCEEDED(ret)) {
    SQLSMALLINT rec_id = 1;
    SQLCHAR  sql_state[6];
    SQLINTEGER native_error_code = 0;
    SQLCHAR error_msg[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT actual_msg_len = 0;

    SQLRETURN ret = SQLGetDiagRecA(SQL_HANDLE_DBC, hdbc_, rec_id, sql_state, &native_error_code, error_msg, sizeof(error_msg), &actual_msg_len);
    std::string res;
    if (ret == SQL_SUCCESS) {
      res.append(reinterpret_cast<const char*>(error_msg), actual_msg_len);
    }
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
    SQLFreeEnv(env_);
    throw std::runtime_error("SQLDriverConnect has failed, " + res);
  }
  
}

Connection::~Connection()
{
  SQLRETURN ret = SQLDisconnect(hdbc_);
  ret = SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
  ret = SQLFreeEnv(env_);
}

const SQLHENV  Connection::getEnv() const {
  return env_;
}

SQLHENV  Connection::getEnv() {
  return env_;
}

const SQLHDBC Connection::getDBC() const {
  return hdbc_;
}

SQLHDBC Connection::getDBC() {
  return hdbc_;
}
