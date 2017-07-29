#pragma once

#include "sql_stmt.hpp"
#include <memory>

class DDLStatement : public SqlStatement {
public:
  DDLStatement(const std::shared_ptr<Connection>  &con);
  DDLStatement(const std::shared_ptr<Connection>  &con, const std::string  &sql);
  virtual ~DDLStatement();

  DDLStatement(const DDLStatement  &ddlStmt) = delete;
  DDLStatement & operator = (DDLStatement  &ddlStmt) = delete;

  DDLStatement(DDLStatement  &&ddlStmt) = default;
  DDLStatement & operator = (DDLStatement  &&ddlStmt) = default;

  int prepare() override;
  int execute() override;
  int commit() override;
  void setSql(const std::string  &query) override;
  std::string getErrorMessage() const override;

  const SQLHSTMT getStmt() const;
  SQLHSTMT getStmt();

  const std::shared_ptr<Connection>  &getConnection() const;
  std::shared_ptr<Connection>  &getConnection();

private:
  std::shared_ptr<Connection> con_;
  std::string strStmt_;
  SQLHSTMT stmt_;
};