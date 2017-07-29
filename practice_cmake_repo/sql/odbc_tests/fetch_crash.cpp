#include <iostream>

#include "fetch_crash.hpp"

void fetch_test() {
  SQLHENV  henv = SQL_NULL_HENV;
  SQLHDBC  hdbc1 = SQL_NULL_HDBC;
  SQLHSTMT hstmt1 = SQL_NULL_HSTMT;


  char szConnectionString[] = "DSN=mssql2016;UID=sa;PWD=123456;";

  //SQLSMALLINT nODBCIsolationLevel = 0;
  short int lConnectionStringLen;
  char szConnectStringOut[250];
  char szSQLStatement[250];
  SQLSMALLINT type = 0;

  char buffer[256];

  SQLINTEGER ret = 0;
  SQLINTEGER len = 0;

  ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);
  ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
  ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc1);

  ret = SQLDriverConnectA(hdbc1, NULL, (SQLCHAR*)szConnectionString, SQL_NTS, (SQLCHAR*)szConnectStringOut, sizeof(szConnectStringOut), &lConnectionStringLen, SQL_DRIVER_COMPLETE);

  ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

  //whatever you need 
  strcpy(szSQLStatement, "SELECT col_dec, col_float  FROM mssql_test.dbo.simple_datatypes");
  //strcpy(szSQLStatement, "SELECT col_float  FROM mssql_test.dbo.simple_datatypes");
  ret = SQLExecDirectA(hstmt1, (SQLCHAR *)szSQLStatement, strlen(szSQLStatement));

  {
    SQLHDESC hdesc;
    SQL_NUMERIC_STRUCT  dec;
    float flt = 0.0f;
    SQLLEN outlen = 0;
    ret = SQLBindCol(hstmt1, 1, SQL_C_NUMERIC, &dec, sizeof(dec), &outlen);

    ret = SQLBindCol(hstmt1, 2, SQL_C_FLOAT, &flt, sizeof(flt), &outlen);


    ret = SQLGetStmtAttr(hstmt1, SQL_ATTR_APP_ROW_DESC, &hdesc, SQL_IS_POINTER, &len);
    ret = SQLSetDescField(hdesc, 1, SQL_DESC_TYPE, (SQLPOINTER)SQL_C_NUMERIC, SQL_IS_INTEGER);

    //set your own PRECITION & SCALE
    const int prec = 10;
    const int scale = 2;
    ret = SQLSetDescField(hdesc, 1, SQL_DESC_PRECISION, (SQLPOINTER)prec, SQL_IS_INTEGER);
    ret = SQLSetDescField(hdesc, 1, SQL_DESC_SCALE, (SQLPOINTER)scale, SQL_IS_INTEGER);

    //set desc for the second param. is it the point of crash????
    ret = SQLSetDescField(hdesc, 2, SQL_DESC_TYPE, (SQLPOINTER)SQL_C_FLOAT, SQL_IS_INTEGER);

    memset(&dec, 0, sizeof(dec));
    ret = SQLFetch(hstmt1);

    type = SQL_ARD_TYPE;
    ret = SQLGetData(hstmt1, 1, type, &dec, 19, &outlen);
    ret = SQLGetData(hstmt1, 2, SQL_FLOAT, &flt, sizeof(flt), &outlen);
  }

  SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
  SQLDisconnect(hdbc1);
  SQLFreeHandle(SQL_HANDLE_DBC, hdbc1);
  SQLFreeHandle(SQL_HANDLE_ENV, henv);
}