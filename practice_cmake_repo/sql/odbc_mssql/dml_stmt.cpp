#include "define.hpp"
#include "bind_param.hpp"
#include "dml_stmt.hpp"

DMLStatement::DMLStatement(const std::shared_ptr<Connection>  &con, const std::string  &sql)
  :DDLStatement{con, sql}, bind_pos_(0), def_pos_(0)
{}

DMLStatement::~DMLStatement()
{}

int DMLStatement::fetch() {
  return SQLFetch(getStmt());
}

int DMLStatement::setBind(const std::shared_ptr<base_bind>  &bind) {
  int ret = bind->applyToStmt(*this);
  ++bind_pos_;
  return ret;
}

int DMLStatement::setDef(const std::shared_ptr<base_define>  &def) {
  int ret = def->applyToStmt(*this);
  ++def_pos_;
  return ret;
}

int DMLStatement::setDef(const std::list <std::shared_ptr<base_define>>  &defs) {
  std::list <std::shared_ptr<base_define>>::const_iterator it = defs.begin();
  std::list <std::shared_ptr<base_define>>::const_iterator e = defs.end();

  while (it != e && (*it)->getType() != SQL_NUMERIC) { ++it; };
  bool hasNumeric = it != e;
  
  if (hasNumeric) {
    for (auto &def : defs) {
      int ret = def->applyToStmt(*this);
      if (!SQL_SUCCEEDED(ret)) {
        return ret;
      }
      if (def->getType() != SQL_NUMERIC) {
        int pos = getDefPos();
        //TODO: refactor it with getting ROW desriptor only once and apply it to all fields in the case of SQL_NUMERIC presents in selected record
        def->setDescription(pos, *this);
        get_data_fields_.insert(std::make_pair(pos, def));
      }
      ++def_pos_;
    }
  } else {
    for (auto &def : defs) {
      int ret = def->applyToStmt(*this);
      if (!SQL_SUCCEEDED(ret)) {
        return ret;
      }
      ++def_pos_;
    }
  }
  return SQL_SUCCESS;
}

int DMLStatement::getBindPos() const {
  return bind_pos_ + 1;
}

int DMLStatement::getDefPos() const {
  return def_pos_ + 1;
}

void DMLStatement::resetBindPos() {
  bind_pos_ = 0;
}

void DMLStatement::resetDefPos() {
  def_pos_ = 0;
}

DMLStatement::post_t & DMLStatement::getPostFields() {
  return get_data_fields_;
}

const DMLStatement::post_t & DMLStatement::getPostFields() const {
  return get_data_fields_;
}

int DMLStatement::getData() {
  for (auto &r : get_data_fields_) {
    int ret = r.second->postGetData(r.first, *this);
    if (!SQL_SUCCEEDED(ret)) {
      return ret;
    }
  }
  return SQL_SUCCESS;
}