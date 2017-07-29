#include "dml_stmt.hpp"

struct base_bind {
  virtual ~base_bind() = 0 {}
  virtual int applyToStmt(DMLStatement  &stmt) = 0;
};

//Compilation issue with name bind due to winsock.h bind function!!!

template<class T, short Type, short SqlType, bool isNull = false>
struct bind_param : public base_bind {
  bind_param()
    :value_()
  {}
  bind_param(const T  &value)
    :value_(value)
  {}
  virtual ~bind_param() {}
  virtual int applyToStmt(DMLStatement  &stmt) {
    //may be have to throw an exception to mark the specialized functio must be called
    //there are so many shades of ODBC driver specifications
    
    int pos = stmt.getBindPos();
    SQLRETURN ret = SQL_SUCCESS;
    if (isNull) {
      SQLLEN out_len = SQL_NULL_DATA;
      ret = SQLBindParameter(stmt.getStmt(), pos, SQL_PARAM_INPUT, Type, SqlType, 0, 0, (SQLPOINTER)&value_, sizeof(value_), &out_len);
    } else {
      ret = SQLBindParameter(stmt.getStmt(), pos, SQL_PARAM_INPUT, Type, SqlType, 0, 0, (SQLPOINTER)&value_, sizeof(value_), nullptr);
    }
    return ret;
  }
private:
  T value_;
};

//ODBC 64 https://msdn.microsoft.com/en-us/library/ms716287(v=vs.85).aspx
int bind_param<std::string, SQL_CHAR, SQL_C_CHAR>::applyToStmt(DMLStatement  &stmt) {
  //FOR NULL-TERMINATED STRING DO NOT SET SQL_NTS!!!
  //SQLLEN out_len = SQL_NTS;
  SQLULEN string_length = value_.length();
  int pos = stmt.getBindPos();
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), stmt.getBindPos(), SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, string_length, 0, (SQLPOINTER)value_.c_str(), string_length, nullptr);
  return ret;
}

int bind_param<std::string, SQL_CHAR, SQL_C_CHAR, true>::applyToStmt(DMLStatement  &stmt) {
  SQLLEN out_len = SQL_NULL_DATA;
  //SQLULEN string_length = value_.length();
  int pos = stmt.getBindPos();
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), stmt.getBindPos(), SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, SQL_DESC_LENGTH, 0, (SQLPOINTER)0,0, &out_len);
  return ret;
}

int bind_param<std::wstring, SQL_WCHAR, SQL_C_WCHAR>::applyToStmt(DMLStatement  &stmt) {
  //FOR NULL-TERMINATED STRING DO NOT SET SQL_NTS!!!
  //SQLLEN out_len = SQL_NTS;
  SQLULEN string_length = value_.length();
  int pos = stmt.getBindPos();
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), stmt.getBindPos(), SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, string_length, 0, (SQLPOINTER)value_.c_str(),
    string_length*sizeof(std::wstring::value_type), nullptr);
  return ret;
}

int bind_param<std::wstring, SQL_WCHAR, SQL_C_WCHAR, true>::applyToStmt(DMLStatement  &stmt) {
  SQLLEN out_len = SQL_NULL_DATA;
  int pos = stmt.getBindPos();
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), stmt.getBindPos(), SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, SQL_DESC_LENGTH, 0, (SQLPOINTER)0, 0, &out_len);
  return ret;
}

//https://support.microsoft.com/en-us/kb/181254
int bind_param<SQL_NUMERIC_STRUCT, SQL_C_NUMERIC, SQL_NUMERIC>::applyToStmt(DMLStatement  &stmt) {
  SQLULEN size = sizeof(value_);
  int pos = stmt.getBindPos();
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), pos, SQL_PARAM_INPUT, SQL_C_NUMERIC, SQL_NUMERIC, value_.precision, value_.scale, (SQLPOINTER)&value_, size, nullptr);
  if(!(SQL_SUCCEEDED(ret))) {
    return ret;
  }

  SQLHDESC hdesc = nullptr;
  ret = SQLGetStmtAttr(stmt.getStmt(), SQL_ATTR_APP_PARAM_DESC, &hdesc, 0, NULL);
  if (!(SQL_SUCCEEDED(ret))) {
    return ret;
  }
  ret = SQLSetDescField(hdesc, pos, SQL_DESC_TYPE, (SQLPOINTER)SQL_C_NUMERIC, 0);
  if (!(SQL_SUCCEEDED(ret))) {
    return ret;
  }
  ret = SQLSetDescField(hdesc, pos, SQL_DESC_PRECISION, (SQLPOINTER)value_.precision, 0);
  if (!(SQL_SUCCEEDED(ret))) {
    return ret;
  }
  ret= SQLSetDescField(hdesc, pos, SQL_DESC_SCALE, (SQLPOINTER)value_.scale, 0);
  if (!(SQL_SUCCEEDED(ret))) {
    return ret;
  }
  ret = SQLSetDescField(hdesc, pos, SQL_DESC_DATA_PTR, (SQLPOINTER)&value_, 0);

  return ret;
}

int bind_param<SQL_NUMERIC_STRUCT, SQL_C_NUMERIC, SQL_NUMERIC, true>::applyToStmt(DMLStatement  &stmt) {
  SQLLEN out_len = SQL_NULL_DATA;
  int pos = stmt.getBindPos();
  const int DEFAULT_PRECISION = 18;
  const int DEFAULT_SCALE = 12;
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), pos, SQL_PARAM_INPUT, SQL_C_NUMERIC, SQL_NUMERIC, DEFAULT_PRECISION, DEFAULT_SCALE, (SQLPOINTER)&value_.val, sizeof(value_.val), &out_len);
  return ret;
}

//https://msdn.microsoft.com/en-us/library/ff878122.aspx
int bind_param<SQL_DATE_STRUCT, SQL_C_DATE, SQL_DATE>::applyToStmt(DMLStatement  &stmt) {
  SQLLEN out_len = 0;
  SQLULEN size = sizeof(SQL_DATE_STRUCT);
  int pos = stmt.getBindPos();
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), pos, SQL_PARAM_INPUT, SQL_C_DATE, SQL_DATE, SQL_DATE_LEN, 0, (SQLPOINTER)&value_, size, &out_len);
  return ret;
}

int bind_param<SQL_DATE_STRUCT, SQL_C_DATE, SQL_DATE, true>::applyToStmt(DMLStatement  &stmt) {
  SQLLEN out_len = SQL_NULL_DATA;
  int pos = stmt.getBindPos();
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), pos, SQL_PARAM_INPUT, SQL_C_DATE, SQL_C_DATE, SQL_DATE_LEN, 0, (SQLPOINTER)0, 0, &out_len);
  return ret;
}

/*
//use SQL_NULL_DATA for setting NULLs
template<class T, short Type, short SqlType, bool isNull, typename std::enable_if<std::is_same <T, std::string>::value || std::is_same<T, std::wstring>::value>::type = 0>
int bind_param<T, Type, SqlType, isNull>::applyToStmt(DMLStatement  &stmt) {
  SQLLEN out_len = 0;

  if (isNull) {
    out_len = SQL_NULL_DATA;
  }

  SQLULEN string_length = value_.length();
  int pos = stmt.getBindPost();
  SQLRETURN ret = SQLBindParameter(stmt.getStmt(), pos, SQL_PARAM_INPUT, Type, SqlType, SQL_DESC_LENGTH, 0, (SQLPOINTER)value_.c_str(), value_.length(), &out_len);
  return ret;
}*/