#pragma once

#include <list>
#include <map>
#include "ddl_stmt.hpp"

struct base_bind;
struct base_define;

class DMLStatement : public DDLStatement {
public:
  using post_t = std::map<int, std::shared_ptr<base_define>>;

  DMLStatement(const std::shared_ptr<Connection>  &con, const std::string  &sql);
  virtual ~DMLStatement();


  DMLStatement(const DMLStatement  &ddlStmt) = delete;
  DMLStatement & operator = (DMLStatement  &ddlStmt) = delete;

  DMLStatement(DMLStatement  &&dmlStmt) = default;
  DMLStatement & operator = (DMLStatement  &&ddlStmt) = default;

  int fetch();
  int setBind(const std::shared_ptr<base_bind>  &bind);
  int setDef(const std::shared_ptr<base_define>  &def);
  int setDef(const std::list <std::shared_ptr<base_define>>  &defs);

  int getBindPos() const;
  int getDefPos() const;

  void resetBindPos();
  void resetDefPos();

  post_t & getPostFields();
  const post_t & getPostFields() const;

//private:
  int getData();

private:
  int bind_pos_;
  int def_pos_;
  post_t get_data_fields_;
};