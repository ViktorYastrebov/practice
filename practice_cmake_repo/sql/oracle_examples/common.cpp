#include "common.hpp"
/*
std::shared_ptr<DMLSqlStmt> buildSelectStmt(const std::shared_ptr<Connection> &con, const std::list<std::tuple<std::string, std::shared_ptr<base_define>>>  &selCols, const std::string  &table)
{
  std::string sql("SELECT ");

  sql += std::get<0>(*selCols.begin());
  std::for_each(++selCols.begin(), selCols.end(),
    [&](const std::tuple<std::string, std::shared_ptr<base_define>>  &tuple)
  {
    sql += ", " + std::get<0>(tuple);
  }
  );

  sql += " FROM " + table;
  std::shared_ptr<DMLSqlStmt> dml = std::make_shared<DMLSqlStmt>(con, sql);
  std::for_each(selCols.begin(), selCols.end(), [&](const std::tuple<std::string, std::shared_ptr<base_define>> &tuple) {
    dml->setDef(std::get<1>(tuple));
  });
  return dml;
}

std::shared_ptr<DMLSqlStmt> buildInsertStmt(const std::shared_ptr<Connection>  &con, const std::string  &table, std::list<std::shared_ptr<base_bind>>  &colsValues) {
  std::string sql("INSERT INTO ");
  sql += table + " VALUES(";

  int length = colsValues.size();
  int idx = 1;
  while (idx < length) {
    sql += ":" + std::to_string(idx) + ",";
    ++idx;
  }
  sql += ":" + std::to_string(idx) + ")";

  std::shared_ptr<DMLSqlStmt> dml = std::make_shared<DMLSqlStmt>(con, sql);
  std::for_each(colsValues.begin(), colsValues.end(), [&](const std::shared_ptr<base_bind>  &def) {dml->setBind(def); });
  return dml;
}

std::shared_ptr<DDLSqlStmt> buildCreateStmt(const std::shared_ptr<Connection>  &con, const std::string  &table, std::list<std::tuple<std::string, std::string>> &cols_and_types) {
  std::string sql("CREATE TABLE ");
  sql += table + "(";

  sql += std::get<0>(*cols_and_types.begin()) + " " + std::get<1>(*cols_and_types.begin());

  std::for_each(++cols_and_types.begin(), cols_and_types.end(), [&](const std::tuple<std::string, std::string>  &name_and_type)
  {
    sql += ", " + std::get<0>(name_and_type) + " " + std::get<1>(name_and_type);
  });
  sql += ")";
  return std::make_shared<DDLSqlStmt>(con, sql);
}

std::shared_ptr<DDLSqlStmt> buildDropTable(const std::shared_ptr<Connection>  &con, const std::string  &table) {
  std::string sql("DROP TABLE ");
  return std::make_shared<DDLSqlStmt>(con, sql + table);
}*/


DMLSqlStmt && buildSelectStmt(const std::shared_ptr<Connection> &con, const std::list<std::tuple<std::string, std::shared_ptr<base_define>>>  &selCols, const std::string  &table)
{
  std::string sql("SELECT ");

  sql += std::get<0>(*selCols.begin());
  std::for_each(++selCols.begin(), selCols.end(),
    [&](const std::tuple<std::string, std::shared_ptr<base_define>>  &tuple)
  {
    sql += ", " + std::get<0>(tuple);
  }
  );

  sql += " FROM " + table;
  DMLSqlStmt dml(con, sql);
  std::for_each(selCols.begin(), selCols.end(), [&](const std::tuple<std::string, std::shared_ptr<base_define>> &tuple) {
    dml.setDef(std::get<1>(tuple));
  });
  return std::move(dml);
}

DMLSqlStmt && buildInsertStmt(const std::shared_ptr<Connection>  &con, const std::string  &table, std::list<std::shared_ptr<base_bind>>  &colsValues) {
  std::string sql("INSERT INTO ");
  sql += table + " VALUES(";

  int length = colsValues.size();
  int idx = 1;
  while (idx < length) {
    sql += ":" + std::to_string(idx) + ",";
    ++idx;
  }
  sql += ":" + std::to_string(idx) + ")";

  DMLSqlStmt dml(con, sql);
  std::for_each(colsValues.begin(), colsValues.end(), [&](const std::shared_ptr<base_bind>  &def) {dml.setBind(def); });
  return std::move(dml);
}

DDLSqlStmt && buildCreateStmt(const std::shared_ptr<Connection>  &con, const std::string  &table, std::list<std::tuple<std::string, std::string>> &cols_and_types) {
  std::string sql("CREATE TABLE ");
  sql += table + "(";

  sql += std::get<0>(*cols_and_types.begin()) + " " + std::get<1>(*cols_and_types.begin());

  std::for_each(++cols_and_types.begin(), cols_and_types.end(), [&](const std::tuple<std::string, std::string>  &name_and_type)
  {
    sql += ", " + std::get<0>(name_and_type) + " " + std::get<1>(name_and_type);
  });
  sql += ")";
  return std::move(DDLSqlStmt{ con, sql });
}

DDLSqlStmt && buildDropTable(const std::shared_ptr<Connection>  &con, const std::string  &table) {
  std::string sql("DROP TABLE ");
  return std::move(DDLSqlStmt{ con, sql + table });
}