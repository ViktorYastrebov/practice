#pragma once


#include <type_traits>
#include <array>

#include "dml_stmt.hpp"
#include "casts.h"

struct base_define : public std::enable_shared_from_this<base_define> {
  virtual ~base_define() = 0 {}
  virtual int applyToStmt(DMLStatement  &stmt) = 0;
  virtual std::string  to_string() const = 0;
  virtual bool isNull() const = 0;
  //Lovely ODBC
  virtual int postGetData(int pos, DMLStatement  &stmt) = 0;
  virtual const int getType() const = 0;
  virtual int setDescription(int pos, DMLStatement  &stmt) const = 0;
};

template<class T, short Type, std::size_t size = 0, typename EnableT = void>
struct define : public base_define {
  define()
    :value_(), isNull_(false)
  {}

  virtual int applyToStmt(DMLStatement  &stmt) override {
    int pos = stmt.getDefPos();
    SQLLEN out = 0;
    SQLLEN size = sizeof(value_);
    SQLRETURN ret = SQLBindCol(stmt.getStmt(), pos, Type, (SQLPOINTER)&value_, size, &out);
    isNull_ = (out == SQL_NULL_DATA);

    return ret;
  }

  virtual bool isNull() const override {
    return isNull_;
  }

  std::string  to_string() const override {
    return std::to_string(value_);
  }

  virtual int postGetData(int pos, DMLStatement  &stmt) override {
    SQLLEN out_len = 0;
    SQLRETURN ret = SQLGetData(stmt.getStmt(), pos, Type, (SQLPOINTER)&value_, sizeof(T), &out_len);
    isNull_ = (out_len == SQL_NULL_DATA);
    return ret;
  }

  virtual const int getType() const override {
    return Type;
  }

  //far from good. It'll be called n-times. Has to be refactored later.
  virtual int setDescription(int pos, DMLStatement  &stmt) const override {
    SQLHDESC hdesc;
    SQLINTEGER len;
    SQLRETURN ret = SQLGetStmtAttrA(stmt.getStmt(), SQL_ATTR_APP_ROW_DESC, &hdesc, SQL_IS_POINTER, &len);
    if (!SQL_SUCCEEDED(ret)) {
      return ret;
    }
    ret = SQLSetDescField(hdesc, pos, SQL_DESC_TYPE, (SQLPOINTER)Type, SQL_IS_INTEGER);
    return ret;
  }

private:
  T value_;
  bool isNull_;
};

template<class CharT, short Type, std::size_t size>
struct define<CharT, Type, size, typename std::enable_if<std::is_same<CharT, char>::value || std::is_same<CharT, std::wstring::value_type>::value>::type> : public base_define {
  define()
    :value_(), isNull_(false)
  {}

  virtual int applyToStmt(DMLStatement  &stmt) override {
    int pos = stmt.getDefPos();
    SQLLEN out = 0;
    SQLRETURN ret = SQLBindCol(stmt.getStmt(), pos, Type, (SQLPOINTER)&value_[0], value_.size() * sizeof(CharT), &out);
    isNull_ = (out == SQL_NULL_DATA);

    return ret;
  }

  virtual bool isNull() const override {
    return isNull_;
  }

  virtual std::string  to_string() const override {
    return std::string(value_.begin(), value_.end());
  }

  virtual int postGetData(int pos, DMLStatement  &stmt) override {
    SQLLEN out_len = 0;
    SQLRETURN ret = SQLGetData(stmt.getStmt(), pos, Type, (SQLPOINTER)&value_[0], value_.size() * sizeof(CharT), &out_len);
    isNull_ = (out_len == SQL_NULL_DATA);
    return ret;
  }

  virtual const int getType() const override {
    return Type;
  }

  virtual int setDescription(int pos, DMLStatement  &stmt) const override {
    SQLHDESC hdesc;
    SQLINTEGER len;
    SQLRETURN ret = SQLGetStmtAttrA(stmt.getStmt(), SQL_ATTR_APP_ROW_DESC, &hdesc, SQL_IS_POINTER, &len);
    if (!SQL_SUCCEEDED(ret)) {
      return ret;
    }
    ret = SQLSetDescField(hdesc, pos, SQL_DESC_TYPE, (SQLPOINTER)Type, SQL_IS_INTEGER);
    if (!SQL_SUCCEEDED(ret)) {
      return ret;
    }
    ret = SQLSetDescField(hdesc, pos, SQL_DESC_LENGTH, (SQLPOINTER)value_.size(), SQL_IS_INTEGER);
    return ret;
  }

private:
  std::array<CharT, size> value_;
  bool isNull_;
};



int define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>::applyToStmt(DMLStatement  &stmt) {
  int pos = stmt.getDefPos();
  SQLLEN out = 0;

  SQLLEN precBuf = 0;
  SQLLEN scaleBuf = 0;
  SQLRETURN ret = SQLColAttributeA(stmt.getStmt(), pos, SQL_DESC_PRECISION, NULL, NULL, NULL, &precBuf);
  if (!SQL_SUCCEEDED(ret)) {
    return ret;
  }
  ret = SQLColAttributeA(stmt.getStmt(), pos, SQL_DESC_SCALE, NULL, NULL, NULL, &scaleBuf);
  if (!SQL_SUCCEEDED(ret)) {
    return ret;
  }

  SQLLEN size = sizeof(value_);
  ret = SQLBindCol(stmt.getStmt(), pos, SQL_NUMERIC, (SQLPOINTER)&value_, size, &out);
  if (SQL_SUCCEEDED(ret)) {
    SQLHDESC hdesc;
    SQLINTEGER len;
    ret = SQLGetStmtAttrA(stmt.getStmt(), SQL_ATTR_APP_ROW_DESC, &hdesc, SQL_IS_POINTER, &len);
    if (!SQL_SUCCEEDED(ret)) {
      return ret;
    }
    ret = SQLSetDescField(hdesc, pos, SQL_DESC_TYPE, (SQLPOINTER)SQL_C_NUMERIC, SQL_IS_INTEGER);
    if (!SQL_SUCCEEDED(ret)) {
      return ret;
    }
    //SQLINTEGER intPrec = static_cast<SQLINTEGER>(precBuf);
    //SQLINTEGER intScale = static_cast<SQLINTEGER>(scaleBuf);
    ret = SQLSetDescField(hdesc, pos, SQL_DESC_PRECISION, (SQLPOINTER)precBuf, SQL_IS_INTEGER);
    if (!SQL_SUCCEEDED(ret)) {
      return ret;
    }
    ret = SQLSetDescField(hdesc, pos, SQL_DESC_SCALE, (SQLPOINTER)scaleBuf, SQL_IS_INTEGER);
    if (!SQL_SUCCEEDED(ret)) {
      return ret;
    }
    stmt.getPostFields().insert(std::make_pair(pos,  shared_from_this()));
  }
  return ret;
}

std::string define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>::to_string() const {
  return numberToString(value_);
}

std::string define<SQL_DATE_STRUCT, SQL_DATE>::to_string() const {
  return dateToString(value_);
}

int define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>::postGetData(int pos, DMLStatement  &stmt) {
  
  SQLSMALLINT type = SQL_ARD_TYPE;
  SQLLEN out_len = 0;
  SQLLEN size = sizeof(value_);
  SQLRETURN ret = SQLGetData(stmt.getStmt(), pos, type, (SQLPOINTER)&value_, size, &out_len);
  isNull_ = (out_len == SQL_NULL_DATA);
  return ret;
}