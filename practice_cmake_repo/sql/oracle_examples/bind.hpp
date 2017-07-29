#pragma once

#include "dml_stmt.h"

#include <vector>
#include <algorithm>
//TODO: make compiletime mapping

struct base_bind {
  virtual sword applyBindToStmt(DMLSqlStmt  &stmt) = 0;
  virtual ~base_bind() = 0 {}
};

template<class T, int Type>
struct bind : public base_bind
{
  bind() noexcept
    :value_(), bnd_(0)
  {}

  bind(const T&  value) noexcept
    :value_(value), bnd_(0)
  {}
  virtual ~bind() {}

  virtual sword applyBindToStmt(DMLSqlStmt  &stmt) {
    unsigned int pos = stmt.getBindPos();
    ub4 size = sizeof(T);
    sword ret = OCIBindByPos(stmt.getOCIStmt(), &bnd_, stmt.getConnection()->getError(), pos, (void*)&value_, size, Type, 0, 0, 0, 0, 0, OCI_DEFAULT);
    return ret;
  }
  T value_;
  OCIBind  *bnd_;
};

template<>
sword bind<std::string, SQLT_STR>::applyBindToStmt(DMLSqlStmt  &stmt) {
  unsigned int pos = stmt.getBindPos();
  sword ret = OCIBindByPos(stmt.getOCIStmt(), &bnd_, stmt.getConnection()->getError(), pos, (void*)value_.c_str(), static_cast<sb4>(value_.length() + 1), SQLT_STR, 0, 0, 0, 0, 0, OCI_DEFAULT);
  return ret;
}

struct number_bind : public base_bind {
  number_bind()
    :bnd_(0), numstr_()
  {}

  number_bind(const std::string  &numberStr)
    :bnd_(0), numstr_(numberStr)
  {}

  virtual ~number_bind() {}

  virtual sword applyBindToStmt(DMLSqlStmt  &stmt) {
    unsigned int pos = stmt.getBindPos();
    ub4 size = sizeof(OCINumber);
    OCIError  *err = stmt.getConnection()->getError();
    sword ret = OCI_SUCCESS;

    if (!numstr_.empty()) {
      std::vector<oratext> fmt = getFormat();
       ret = OCINumberFromText(err,
        reinterpret_cast<const oratext *>(numstr_.c_str()),
        static_cast<ub4>(numstr_.length()),
        //reinterpret_cast<const oratext*>("TM9"),
        fmt.data(),
        fmt.size(),
        0, 0, &num_);
      ret = OCIBindByPos(stmt.getOCIStmt(), &bnd_, err, pos, &num_, size, SQLT_VNU, 0, 0, 0, 0, 0, OCI_DEFAULT);
    } else {
      //SET NULL indicator: http://docs.oracle.com/cd/B10501_01/appdev.920/a96584/oci15r29.htm
      sb2 indicatorPointer = -1;
      ret = OCIBindByPos(stmt.getOCIStmt(), &bnd_, err, pos, 0, 0, SQLT_VNU, &indicatorPointer, 0, 0, 0, 0, OCI_DEFAULT);
      std::string error = stmt.getConnection()->getErrorMessage(ret);
    }
    return ret;
  }

  std::vector<oratext> getFormat() const {
    std::vector<oratext>  ret;
    std::for_each(numstr_.begin(), numstr_.end(), [&](const std::string::value_type  &Ch)
    {
      //SELECT TO_NUMBER('123.05','999.99') FROM DUAL;
      //SELECT TO_NUMBER('123,456.78','999,999.99') FROM DUAL;
      if (isdigit(Ch)) {
        ret.push_back('9');
      } else {
        ret.push_back(Ch);
      }
    });
    return ret;
  }

private:
  OCIBind  *bnd_;
  OCINumber  num_;
  std::string numstr_;
};