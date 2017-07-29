#pragma once

#include "ddl_stmt.h"

#include <list>

struct base_bind;
struct base_define;

class DMLSqlStmt : public DDLSqlStmt {
public:
  explicit DMLSqlStmt(const std::shared_ptr<Connection>  &con, const std::string  &sql);
  virtual ~DMLSqlStmt();

  DMLSqlStmt(DMLSqlStmt  &&dmlStmt) = default;

  sword fetch();
  sword setBind(const std::shared_ptr<base_bind>  &bind);
  sword setDef(const std::shared_ptr<base_define>  &def);

  unsigned int getBindPos() const;
  unsigned int getDefPos() const;

  void resetBindPos();
  void resetDefPos();

private:
  unsigned int bindPos_;
  unsigned int defPos_;
};
