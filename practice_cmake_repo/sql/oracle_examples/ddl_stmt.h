#pragma once

#include <memory>
#include "sql_stmt.hpp"

class DDLSqlStmt : public SqlStatement {
public:
  DDLSqlStmt(const std::shared_ptr<Connection>  &con);
  explicit DDLSqlStmt(const std::shared_ptr<Connection>  &con, const std::string  &sql);
  virtual ~DDLSqlStmt();

  DDLSqlStmt(DDLSqlStmt  &&ddlStmt) = default;

  sword prepare() override;
  void setSql(const std::string  &query) override;

  sword execute() override;
  sword commit() override;

  std::shared_ptr<Connection> getConnection();
  const std::shared_ptr<Connection> getConnection() const;

  const OCIStmt  *getOCIStmt() const;
  OCIStmt  *getOCIStmt();

  std::string  & getSqlString();
  const std::string  & getSqlString() const;

private:
  std::shared_ptr<Connection> con_;
  OCIStmt  *stmt_;
  std::string sql_;
};
