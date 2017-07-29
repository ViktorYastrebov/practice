#include <iostream>
#include <algorithm>
//  CREATE TABLE simple_datatypes (
//    col_nchar NCHAR(128), col_int INT, col_dec DECIMAL(10,2),
//    col_dec2 DEC(30,5), col_dec3 DECIMAL(4,2), col_float float(24),
//    col_date DATE);
// INSERT INTO mssql_test.dbo.simple_datatypes VALUES('some string value', 1, 123.56, 123456789.0123, 12.34, 3.14, '2016-12-01');
//
//
//  INSERT INTO mssql_test.dbo.simple_datatypes 
//	VALUES('another string value', 2, 234.56, 234567890.123,23.45, 2.71, '2016-12-02'),
//	('another string value 2', 3, 345.67, 345678901.234,23.45, 1.618, '2016-12-03'),
//	('another string value 3', 4, 345.67, 456789012.345,34.56, 6.626, '2016-12-04'),
//	('another string value 4', 5, 456.78, 567890123.456,56.78, 6.81, '2016-12-05'),
//	('another string value 5', 6, 789.01, 678901234.567,67.89, 0.00001, '2016-12-06'),

//https://support.microsoft.com/en-us/kb/222831 Decimal example

#include "common.h"

void example_char(const std::string  &dns, const std::string  &user, const std::string  &pwd) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(dns, user, pwd);
    {
      std::string createCharTest("CREATE TABLE simple_char (col_char CHAR(32), col_wchar NCHAR(32))");
      DDLStatement createTable{ con, createCharTest };
      int ret = createTable.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << createTable.getErrorMessage() << std::endl;
      }
      createTable.commit();
    }

    {
      std::string insertStr("INSERT INTO mssql_test.dbo.simple_char VALUES(?,?)");
      DMLStatement insertStmt{ con,insertStr };

      //The point with this strings was: USE SQL_NTS as last argument
      std::shared_ptr<base_bind>  bindChar = std::make_shared<bind_param<std::string, SQL_CHAR, SQL_C_CHAR>>("Some string value");
      std::shared_ptr<base_bind>  bindUchar = std::make_shared<bind_param<std::wstring, SQL_WCHAR, SQL_C_WCHAR>>(L"Unicode String");
      insertStmt.setBind(bindChar);
      insertStmt.setBind(bindUchar);
      int ret = insertStmt.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << insertStmt.getErrorMessage() << std::endl;
      }
      insertStmt.commit();
    }
  } catch (const std::exception  &ex) {
    std::cout << "exceptions occurs: " << ex.what() << std::endl;
  }
}

void example_decimal(const std::string  &dns, const std::string  &user, const std::string  &pwd) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(dns, user, pwd);
    {
      std::string createCharTest("CREATE TABLE simple_decimal (col_dec dec(10,2), col_dec2 dec(30,5), col_dec3 dec(4,2))");
      DDLStatement createTable{ con, createCharTest };
      int ret = createTable.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << createTable.getErrorMessage() << std::endl;
      }
      createTable.commit();
    }

    {
      std::string insertStr("INSERT INTO mssql_test.dbo.simple_decimal VALUES(?,?,?)");
      DMLStatement insertStmt{ con,insertStr };

      std::shared_ptr<base_bind>  bind1 = std::make_shared<bind_param<SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC >>(numberFromString("123.45"));
      std::shared_ptr<base_bind>  bind2 = std::make_shared<bind_param< SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC >>(numberFromString("234567890.123"));
      std::shared_ptr<base_bind>  bind3 = std::make_shared<bind_param< SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("12.34"));

      if (!SQL_SUCCEEDED(insertStmt.setBind(bind1))) {
        std::cout << insertStmt.getErrorMessage() << std::endl;
      }
      if (!SQL_SUCCEEDED(insertStmt.setBind(bind2))) {
        std::cout << insertStmt.getErrorMessage() << std::endl;
      }
      if (!SQL_SUCCEEDED(insertStmt.setBind(bind3))) {
        std::cout << insertStmt.getErrorMessage() << std::endl;
      }

      int ret = insertStmt.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << insertStmt.getErrorMessage() << std::endl;
      }
      insertStmt.commit();
    }
  }
  catch (const std::exception  &ex) {
    std::cout << "exceptions occurs: " << ex.what() << std::endl;
  }
}


//https://social.technet.microsoft.com/Forums/Windowsserver/en-US/5ee258b8-0970-4036-b49f-155417366b70/sqlcnumeric-data-incorrect-after-insert?forum=sqldataaccess
// have to check

void example_decimal2(const std::string  &dns, const std::string  &user, const std::string  &pwd) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(dns, user, pwd);
    {
      std::string createCharTest("CREATE TABLE simple_decimal2 (col_dec3 dec(4,2))");
      DDLStatement createTable{ con, createCharTest };
      int ret = createTable.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << createTable.getErrorMessage() << std::endl;
      }
      createTable.commit();
    }

    {
      std::string insertStr("INSERT INTO mssql_test.dbo.simple_decimal2 VALUES(?)");
      DMLStatement insertStmt{ con,insertStr };

      std::shared_ptr<base_bind>  bind = std::make_shared<bind_param< SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("12.34"));

      if (!SQL_SUCCEEDED(insertStmt.setBind(bind))) {
        std::cout << insertStmt.getErrorMessage() << std::endl;
      }
      int ret = insertStmt.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << insertStmt.getErrorMessage() << std::endl;
      }
      insertStmt.commit();
    }
  }
  catch (const std::exception  &ex) {
    std::cout << "exceptions occurs: " << ex.what() << std::endl;
  }
}

void example_decimal3(const std::string &dns, const std::string  &user, const std::string  &pwd) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(dns, user, pwd);
    {
      std::string createCharTest("CREATE TABLE simple_decimal3 (col_dec dec(4,2), col_dec2 dec(10,2))");
      DDLStatement createTable{ con, createCharTest };
      int ret = createTable.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << createTable.getErrorMessage() << std::endl;
      }
      createTable.commit();
    }

    {
      std::string insertStr("INSERT INTO mssql_test.dbo.simple_decimal3 VALUES(?,?)");
      DMLStatement insertStmt{ con,insertStr };

      std::shared_ptr<base_bind>  bind = std::make_shared<bind_param< SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("12.34"));
      std::shared_ptr<base_bind>  bind2 = std::make_shared<bind_param<SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC >>(numberFromString("12345678.91"));

      if (!SQL_SUCCEEDED(insertStmt.setBind(bind))) {
        std::cout << insertStmt.getErrorMessage() << std::endl;
      }

      if (!SQL_SUCCEEDED(insertStmt.setBind(bind2))) {
        std::cout << insertStmt.getErrorMessage() << std::endl;
      }
      int ret = insertStmt.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << insertStmt.getErrorMessage() << std::endl;
      }
      insertStmt.commit();
    }
  }
  catch (const std::exception  &ex) {
    std::cout << "exceptions occurs: " << ex.what() << std::endl;
  }
}

void example_def_float(const std::string &dns, const std::string  &user, const std::string  &pwd) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(dns, user, pwd);
    {
      DMLStatement selectFloat{ con, "SELECT col_float  FROM mssql_test.dbo.simple_datatypes" };
      std::shared_ptr<base_define> def = std::make_shared<define<float, SQL_REAL>>();
      int ret  = selectFloat.setDef(def);

      ret = selectFloat.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << selectFloat.getErrorMessage() << std::endl;
      }

      while (SQL_SUCCEEDED(selectFloat.fetch())) {
        //ret = selectFloat.getData();
        //if (!SQL_SUCCEEDED(ret)) {
        //  std::cout << selectStmt.getErrorMessage() << std::endl;
        //}
        std::cout << "value :" << def->to_string() << std::endl;
      }
    }
  }
  catch (const std::exception  &ex) {
    std::cout << "exceptions occurs: " << ex.what() << std::endl;
  }
}

void example_def_float_mix(const std::string &dns, const std::string  &user, const std::string  &pwd) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(dns, user, pwd);
    {
      DMLStatement selectFloat{ con, "SELECT col_dec, col_float  FROM mssql_test.dbo.simple_datatypes" };


      std::shared_ptr<base_define> def_dec = std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>();
      std::shared_ptr<base_define> def_flt = std::make_shared<define<float, SQL_REAL>>();

      int ret = selectFloat.setDef({ def_dec, def_flt });
      //int ret = selectFloat.setDef(def_dec);
      //ret = selectFloat.setDef(def_flt);

      //https://mssqlwiki.com/2012/10/16/sql-server-exception_access_violation-and-sql-server-assertion/

      ret = selectFloat.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << selectFloat.getErrorMessage() << std::endl;
      }
      while (SQL_SUCCEEDED(selectFloat.fetch())) {
        ret = selectFloat.getData();
        if (!SQL_SUCCEEDED(ret)) {
          std::cout << selectFloat.getErrorMessage() << std::endl;
        }
        std::cout << "float :" << def_flt->to_string() << ", dec : " << def_dec->to_string() << std::endl;
      }
    }
  } catch (const std::exception  &ex) {
    std::cout << "exceptions occurs: " << ex.what() << std::endl;
  } catch (...) {
    std::cout << "something terrible has happened" << std::endl;
  }

}


//http://www.easysoft.com/developer/languages/c/examples/index.html examples. May be helpful

void example_direct_usage(const std::string &dns, const std::string  &user, const std::string  &pwd) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(dns, user, pwd);
    
    //create
    {
      std::string createTableStr("CREATE TABLE simple_datatypes " \
              "(col_nchar NCHAR(128)," \
              "col_int INT,"  \
              "col_dec DECIMAL(10,2)," \
              "col_dec2 DEC(30,5),"\
              "col_dec3 DECIMAL(4,2)," \
              "col_float float(24)," \
              "col_date DATE)");
      DDLStatement createTable{ con, createTableStr };
      int ret = createTable.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << createTable.getErrorMessage() << std::endl;
      }
      createTable.commit();
    }

    //insert
    {
      std::string insertStr("INSERT INTO mssql_test.dbo.simple_datatypes VALUES(?,?,?,?,?,?,?)");
      DMLStatement insertStmt{ con,insertStr };

      std::shared_ptr<base_bind> binds[] = {
        std::make_shared<bind_param<std::wstring, SQL_WCHAR, SQL_C_WCHAR>>(L"Some string value"),
        std::make_shared<bind_param<int, SQL_INTEGER, SQL_C_LONG>>(1), //do not use SQL_C_SLONG. It must be SQL_C_LONG But I do not see the resason.
        std::make_shared<bind_param<SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("123.45")),
        std::make_shared<bind_param<SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("234567890.123")),
        std::make_shared<bind_param<SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("23.45")),
        std::make_shared<bind_param<float, SQL_REAL, SQL_C_FLOAT>>(3.14f), //the same thing. there must be used SQL_REAL
        std::make_shared<bind_param<SQL_DATE_STRUCT,SQL_DATE,SQL_C_DATE>>(dateFromString("2016-12-01"))
      };

      std::for_each(binds, binds + 7, [&](const std::shared_ptr<base_bind>  &bind)
        {
          int ret = insertStmt.setBind(bind);
          if (!SQL_SUCCEEDED(ret)) {
            std::cout << "error in bind, N=" << insertStmt.getBindPos() - 1 << ", error message: " << insertStmt.getErrorMessage() << std::endl;
          }
        }
      );
      int ret = insertStmt.execute();
      if (ret != SQL_SUCCESS) {
        std::cout << "error :" << insertStmt.getErrorMessage() << std::endl;
      }
      ret = insertStmt.commit();
    }

    //select
    {
      DMLStatement selectStmt{ con, "SELECT col_dec, col_dec2, col_dec3 FROM mssql_test.dbo.simple_datatypes" };
      std::shared_ptr<base_define> defs[] = {
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>(),
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>(),
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>()
      };

      std::for_each(defs, defs + 3, [&](const std::shared_ptr<base_define>  &def)
      {
        int ret = selectStmt.setDef(def);
        if (!SQL_SUCCEEDED(ret)) {
          std::cout << selectStmt.getErrorMessage() << std::endl;
        }
      });
      int ret = selectStmt.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << selectStmt.getErrorMessage() << std::endl;
      }

      while (SQL_SUCCEEDED(selectStmt.fetch())) {
        ret = selectStmt.getData();
        if (!SQL_SUCCEEDED(ret)) {
          std::cout << selectStmt.getErrorMessage() << std::endl;
        }
        for (auto &def : defs) {
          std::cout << "value :" << def->to_string() << " ";
        }
        std::cout << std::endl;
      }
    }

    //full select
    {
      DMLStatement selectStmt{ con, "SELECT * FROM mssql_test.dbo.simple_datatypes" };
      //DMLStatement selectStmt{ con, "SELECT col_int, col_dec, col_dec2, col_dec3, col_float  FROM mssql_test.dbo.simple_datatypes" };
      std::list<std::shared_ptr<base_define>> defs_list = {
        std::make_shared<define<wchar_t, SQL_WCHAR, 128>>(),
        std::make_shared<define<int, SQL_INTEGER>>(),
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>(),
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>(),
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>(),
        std::make_shared<define<float, SQL_REAL>>(),
        std::make_shared<define<SQL_DATE_STRUCT, SQL_DATE>>()
      };

      //const int total_defs = 5;
      /*std::shared_ptr<base_define> defs[total_defs] = {
        //std::make_shared<define<wchar_t, SQL_WCHAR, 128>>(),
        std::make_shared<define<int, SQL_INTEGER>>(),
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>(),
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>(),
        std::make_shared<define<SQL_NUMERIC_STRUCT, SQL_NUMERIC>>(),
        std::make_shared<define<float, SQL_REAL>>()
        //std::make_shared<define<SQL_DATE_STRUCT, SQL_DATE>>()
      };*/


      /*std::for_each(defs, defs + total_defs, [&](const std::shared_ptr<base_define>  &def) {
        selectStmt.setDef(def);
      });*/
      selectStmt.setDef(defs_list);

      int ret = selectStmt.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << selectStmt.getErrorMessage() << std::endl;
      }

      while (SQL_SUCCEEDED(selectStmt.fetch())) {
        ret = selectStmt.getData();
        if (!SQL_SUCCEEDED(ret)) {
          std::cout << selectStmt.getErrorMessage() << std::endl;
        }
        for (auto &def : defs_list) {
          std::cout << "value :" << def->to_string() << " ";
        }
        std::cout << std::endl;
      }
    }

  } catch (const std::exception  &ex) {
    std::cout << "exceptions occurs: " << ex.what() << std::endl;
  }
}

void example_char_and_decimals(const std::string &dns, const std::string  &user, const std::string  &pwd) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(dns, user, pwd);

    {
      std::string createTableStr("CREATE TABLE simple_datatypes_mix " \
        "(col_char CHAR(128)," \
        "col_nchar NCHAR(128)," \
        "col_dec DECIMAL(10,2)," \
        "col_dec2 DEC(30,5),"\
        "col_dec3 DECIMAL(4,2))");

      DDLStatement createTable{ con, createTableStr };
      int ret = createTable.execute();
      if (!SQL_SUCCEEDED(ret)) {
        std::cout << "error :" << createTable.getErrorMessage() << std::endl;
      }
      createTable.commit();
    }

    {
      std::string insertStr("INSERT INTO mssql_test.dbo.simple_datatypes_mix VALUES(?,?,?,?,?)");
      DMLStatement insertStmt{ con,insertStr };

      std::shared_ptr<base_bind> binds[] = {
        std::make_shared<bind_param<std::string, SQL_CHAR, SQL_C_CHAR>>("Some string value"),
        std::make_shared<bind_param<std::wstring, SQL_WCHAR, SQL_C_WCHAR>>(L"Unicode string value"),
        std::make_shared<bind_param<SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("123.45")),
        std::make_shared<bind_param<SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("234567890.123")),
        std::make_shared<bind_param<SQL_NUMERIC_STRUCT, SQL_NUMERIC, SQL_C_NUMERIC>>(numberFromString("23.45")),
      };

      std::for_each(binds, binds + 5, [&](const std::shared_ptr<base_bind>  &bind)
      {
        int ret = insertStmt.setBind(bind);
        if (!SQL_SUCCEEDED(ret)) {
          std::cout << "error in bind, N=" << insertStmt.getBindPos() - 1 << ", error message: " << insertStmt.getErrorMessage() << std::endl;
        }
      }
      );
      int ret = insertStmt.execute();
      if (ret != SQL_SUCCESS) {
        std::cout << "error :" << insertStmt.getErrorMessage() << std::endl;
      }
      ret = insertStmt.commit();
    }

    {
    }
  }
  catch (const std::exception  &ex) {
    std::cout << "exceptions occurs: " << ex.what() << std::endl;
  }
}

#include <map>

auto main() -> int {
  std::string dsn("mssql2016");
  std::string user("sa");
  std::string pwd("123456");

  example_direct_usage(dsn, user, pwd);
  //example_def_float(dsn, user, pwd);
  //example_def_float_mix(dsn, user, pwd);

  //example_char(dsn, user, pwd);
  //example_decimal(dsn, user, pwd);
  //example_decimal2(dsn, user, pwd);
  //example_decimal3(dsn, user, pwd);

  //example_char_and_decimals(dsn, user, pwd);

  return 0;
}