#pragma once

#include <memory>
#include <string>


#include <list>
#include <map>
#include <functional>
#include <tuple>

#include "dml_stmt.h"
#include "bind.hpp"
#include "define.hpp"


template<typename Function, typename ... Args>
decltype(auto) currying(Function f, Args &&... args) {
  return[=](auto &&... rest) {
    return f(args..., rest...);
  };
}

/*std::shared_ptr<DMLSqlStmt> buildSelectStmt(const std::shared_ptr<Connection> &con, const std::list<std::tuple<std::string, std::shared_ptr<base_define>>>  &selCols, const std::string  &table);
std::shared_ptr<DMLSqlStmt> buildInsertStmt(const std::shared_ptr<Connection>  &con, const std::string  &table, std::list<std::shared_ptr<base_bind>>  &colsValues);
std::shared_ptr<DDLSqlStmt> buildCreateStmt(const std::shared_ptr<Connection>  &con, const std::string  &table, std::list<std::tuple<std::string, std::string>> &cols_and_types);
std::shared_ptr<DDLSqlStmt> buildDropTable(const std::shared_ptr<Connection>  &con, const std::string  &table);*/

DMLSqlStmt && buildSelectStmt(const std::shared_ptr<Connection> &con, const std::list<std::tuple<std::string, std::shared_ptr<base_define>>>  &selCols, const std::string  &table);
DMLSqlStmt && buildSelectStmt(const std::shared_ptr<Connection> &con, const std::list<std::tuple<std::string, std::shared_ptr<base_define>>>  &selCols, const std::string  &table);
DMLSqlStmt && buildInsertStmt(const std::shared_ptr<Connection>  &con, const std::string  &table, std::list<std::shared_ptr<base_bind>>  &colsValues);
DDLSqlStmt && buildCreateStmt(const std::shared_ptr<Connection>  &con, const std::string  &table, std::list<std::tuple<std::string, std::string>> &cols_and_types);
DDLSqlStmt && buildDropTable(const std::shared_ptr<Connection>  &con, const std::string  &table);