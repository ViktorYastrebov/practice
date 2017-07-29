#pragma once
#include "sql_stmt.hpp"

//https://docs.oracle.com/cd/B19306_01/appdev.102/b14250/oci08sca.htm

class Transaction {
public:
  //Transaction(SqlStatement  &stmt);
  Transaction(const std::shared_ptr<Connection>  &con);
  ~Transaction();

  const std::shared_ptr<SqlStatement>  &stmt() const;
  std::shared_ptr<SqlStatement>  &stmt();
  bool execute();

  const std::string  &getName() const;
  std::string  &getName();

  void commit();
  void rollback();

private:
  void genTransName();
private:
  OCITrans *trans_;
  std::shared_ptr<SqlStatement>  stmt_;
  std::string trans_name_;
};