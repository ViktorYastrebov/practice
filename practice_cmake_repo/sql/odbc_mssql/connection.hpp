#pragma once


#include <string>

#ifdef WIN32
#include <windows.h>
#endif
#include <sqltypes.h>
#include <sqlext.h>

class Connection  {
public:
  explicit Connection(const std::string  &user, const std::string  &pwd);
  explicit Connection(const std::string  &dsn, const std::string  &uid, const std::string  &pwd);
  virtual ~Connection();

  //std::string getErrorMsg();
  
  Connection(const Connection  &con) = delete;
  Connection  & operator = (const Connection  &con) = delete;

  Connection(Connection  &&con) = default;
  Connection & operator = (Connection  &&con) = default;

  const SQLHENV  getEnv() const;
  SQLHENV  getEnv();

  const SQLHDBC getDBC() const;
  SQLHDBC getDBC();

private:
  SQLHENV env_;
  SQLHDBC hdbc_;
};