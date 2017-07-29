#pragma once
#include <array>

#include "dml_stmt.h"

//TODO: make compiletime mapping

//in the general can it can be splited to hierarchy such as:
// AnyType= > {IntegralType, LiteralType, Compound (related to Oracle DB it can be CLOBS/BLOBS etc, Arrays) }

//TODO: SelectForUpdate needs instantiation of define with OCIRef obj

//TODO: implement NULL verification
//TODO: need refactoring since isNull has been implemented

struct base_define {
  virtual sword applyDefToStmt(DMLSqlStmt  &stmt) = 0;
  virtual std::string  to_string() const = 0;
  virtual bool isNull() const = 0;
  virtual ~base_define() {}
};

template<class T, int Type>
struct define : public base_define {
  define() noexcept
    : def_(0), value_()
  {}
  virtual ~define() {}

  virtual sword applyDefToStmt(DMLSqlStmt  &stmt) {
    unsigned int pos = stmt.getDefPos();
    sword ret = OCIDefineByPos(stmt.getOCIStmt(), &def_, stmt.getConnection()->getError(), pos, &value_, sizeof(T), Type, &isNull_, 0, 0, OCI_DEFAULT);
    return ret;
  }

  virtual std::string  to_string() const {
    if (isNull()) {
      return std::string();
    }
    return std::to_string(value_);
  }

  virtual bool isNull() const {
    return isNull_ == -1;
  }

  OCIDefine  *def_;
  T value_;
  sb2 isNull_;
};

template<class T, int StringTypes, std::size_t Length, typename std::enable_if<std::is_same<T, char>::value>::type* = nullptr>
struct literal_define : public base_define {
  literal_define() noexcept
    : base_define(), def_(0), value_{0}
  {}
  virtual ~literal_define() {}

  virtual sword applyDefToStmt(DMLSqlStmt  &stmt) {
    unsigned int pos = stmt.getDefPos();
    sword ret = OCIDefineByPos(stmt.getOCIStmt(), &def_, stmt.getConnection()->getError(), pos, value_.data(), Length + 1, StringTypes, &isNull_, 0, 0, OCI_DEFAULT);
    return ret;
  }

  //can be clipped
  std::string to_string() const {
    if (isNull()) {
      return std::string();
    }
    return std::string(value_.begin(), value_.end());
  }

  virtual bool isNull() const {
    return isNull_ == -1;
  }

  OCIDefine  *def_;
  std::array<T, Length + 1> value_;
  sb2 isNull_;
};


struct number_define : public base_define {
  number_define()
    :base_define(), def_(0), errorHandler_(0)
  {}
  //http://docs.oracle.com/cd/B19306_01/server.102/b14200/sql_elements004.htm

  virtual ~number_define() {}
  virtual sword applyDefToStmt(DMLSqlStmt  &stmt) {
    unsigned int pos = stmt.getDefPos();
    errorHandler_ = stmt.getConnection()->getError();
    //the main point is here: THERE MUST BE USED SQLT_VNU
    sword ret = OCIDefineByPos(stmt.getOCIStmt(), &def_, errorHandler_, pos, &num_, sizeof(OCINumber), SQLT_VNU, &isNull_, 0, 0, OCI_DEFAULT);
    return ret;
  }

  std::string to_string() const {
    if (isNull()) {
      return std::string();
    }

    oratext buff[128];
    ub4 size = 128;
    sword ret = OCINumberToText(errorHandler_, &num_, (const oratext  *)"TM9", 3, 0, 0, &size, buff);
    return std::string(reinterpret_cast<const char*>(buff));
  }

  virtual bool isNull() const {
    return isNull_ == -1;
  }

private:
  OCIDefine  *def_;
  OCINumber  num_;
  OCIError  *errorHandler_;
  sb2 isNull_;
};