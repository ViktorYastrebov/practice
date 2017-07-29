#include <iostream>

#include "common.hpp"
#include "transaction.hpp"
#include "test_suits.h"

void example_direct_usage(const std::string  &user, const std::string  &pwd, const std::string  &serverName) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(user, pwd, serverName);
    {
      std::string createTable("CREATE TABLE oracle_test(col_int INT, col_char CHAR(32), col_float FLOAT(24))");
      DDLSqlStmt createStmt(con, createTable);
      sword ret = createStmt.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }
      std::cout << "create statement execute status = " << ret << std::endl;
    }

    {
      std::string sql("INSERT INTO oracle_test VALUES(:1,:2,:3)");
      DMLSqlStmt stmt(con, sql);
      for (int i = 0; i < 10; ++i) {

        std::shared_ptr<base_bind> binds[] = {
          std::make_shared<bind<int, SQLT_INT>>(i),
          std::make_shared<bind<std::string, SQLT_STR>>(std::string("some char string") + std::to_string(i)),
          std::make_shared<bind<float, SQLT_FLT>>(static_cast<float>(i + 10))
        };

        std::for_each(binds, binds + 3, [&](const std::shared_ptr<base_bind>  &bind) { stmt.setBind(bind);  });

        sword ret = stmt.execute();
        if (ret != OCI_SUCCESS) {
          std::cout << con->getErrorMessage(ret) << std::endl;
        }
        stmt.resetBindPos();
      }
      stmt.commit();
    }

    {
      std::string sql("SELECT col_int, col_char, col_float FROM oracle_test");
      DMLSqlStmt selectAll(con, sql);

      std::shared_ptr<base_define> defs[] = {
        std::make_shared<define<int, SQLT_INT>>(),
        std::make_shared<literal_define<char, SQLT_CHR, 32>>(),
        std::make_shared<define<double, SQLT_FLT>>()
      };

      std::for_each(defs, defs + 3, [&](const std::shared_ptr<base_define>  &def) { selectAll.setDef(def); });

      sword ret = selectAll.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }

      do {

        int ivalue = std::static_pointer_cast<define<int, SQLT_INT>>(defs[0])->value_;
        std::string  svalue = std::static_pointer_cast<literal_define<char, SQLT_CHR, 32>>(defs[1])->to_string();
        double fvalue = std::static_pointer_cast<define<double, SQLT_FLT>>(defs[2])->value_;

        std::cout << "col_int = " << ivalue << ", col_char = " << svalue << ", col_float = " << fvalue << std::endl;
      } while (selectAll.fetch() == OCI_SUCCESS);
      selectAll.commit();
    }

    {
      DDLSqlStmt dropTable(con, "DROP TABLE oracle_test");
      sword ret = dropTable.execute();
      ret = dropTable.commit();
    }

  }
  catch (const std::exception  &ex) {
    std::cout << "exception occurs: " << ex.what() << std::endl;
  }
}

void example_func(const std::string  &user, const std::string  &pwd, const std::string  &serverName) {

  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(user, pwd, serverName);
    {
      using create_table_map = std::map<std::string, std::list<std::tuple<std::string, std::string>>>;
      create_table_map tables = {
        { "oracle_test1",
        { std::make_tuple<std::string, std::string>("col1", "int"), std::make_tuple < std::string, std::string>("col2","float(24)") }
        }
        ,
        { "oracle_test2",
        { std::make_tuple<std::string, std::string>("col1","char(32)"), std::make_tuple<std::string, std::string>("col2", "int") }
        },
        { "oracle_test3",
        { std::make_tuple<std::string, std::string>("col1","float(24)"), std::make_tuple<std::string, std::string>("col2", "float(24)") }
        }
      };

      using insert_tables_map = std::map<std::string, std::list<std::shared_ptr<base_bind>>>;
      insert_tables_map inserts =
      {
        { "oracle_test1",
        { std::make_shared<bind<int, SQLT_INT>>(1), std::make_shared<bind<double, SQLT_FLT>>(3.14) }
        },
        { "oracle_test2",
        { std::make_shared<bind<std::string, SQLT_STR>>(std::string("val1")), std::make_shared<bind<int, SQLT_INT>>(100) }
        },

        { "oracle_test3",
        { std::make_shared<bind<double, SQLT_FLT>>(3.3), std::make_shared<bind<double, SQLT_FLT>>(200.00) }
        }
      };

      for (auto &table : tables) {
        auto createStmt = currying(buildCreateStmt, con)(table.first, table.second);
        sword ret = createStmt.execute();
        createStmt.commit();

        insert_tables_map::iterator it = inserts.find(table.first);
        if (it != inserts.end()) {
          auto insertStmt = currying(buildInsertStmt, con)(it->first, it->second);
          sword ret = insertStmt.execute();
          insertStmt.commit();
        }
      }

      using select_map = std::map<std::string, std::list<std::tuple<std::string, std::shared_ptr<base_define>>>>;
      select_map  selects = {
        { std::string("oracle_test1"),
        {
          std::make_tuple<std::string, std::shared_ptr<base_define>>("col1", std::make_shared<define<int,SQLT_INT>>()),
          std::make_tuple<std::string, std::shared_ptr<base_define>>("col2", std::make_shared<define<double, SQLT_FLT>>())
        }
        },

        { std::string("oracle_test2"),
        {
          std::make_tuple<std::string, std::shared_ptr<base_define>>("col1", std::make_shared<literal_define<char, SQLT_CHR, 32>>()),
          std::make_tuple("col2", std::make_shared<define<int,SQLT_INT>>())
        }
        },
        { std::string("oracle_test3"),
        {
          std::make_tuple<std::string, std::shared_ptr<base_define>>("col1", std::make_shared<define<double, SQLT_FLT>>()),
          std::make_tuple<std::string, std::shared_ptr<base_define>>("col2", std::make_shared<define<double, SQLT_FLT>>())
        }
        }
      };

      for (auto &sel : selects) {
        auto selectStmt = currying(buildSelectStmt, con)(sel.second, sel.first);
        sword ret = selectStmt.execute();
        do {
          std::cout << "table name: " << sel.first << std::endl;
          for (auto &res : sel.second) {
            std::cout << std::get<1>(res)->to_string() << " ,";
          }
          std::cout << std::endl;
        } while (selectStmt.fetch() == OCI_SUCCESS);
      }

      for (auto &dropIt : tables) {
        auto dropStmt = currying(buildDropTable, con)(dropIt.first);
        dropStmt.execute();
        dropStmt.commit();
      }
    }
  }
  catch (const std::exception  &ex) {
    std::cout << "exception occurs: " << ex.what() << std::endl;
  }
}

void numbers_define_example(const std::string  &user, const std::string  &pwd, const std::string  &serverName) {
  //http://docs.oracle.com/cd/B10501_01/appdev.920/a96584/toc.htm
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(user, pwd, serverName);
    {
      std::string createTable("CREATE TABLE oracle_numbers (num1 NUMBER(38), num2 NUMBER(4,5), num3 NUMBER(8,3), num4 NUMBER(10,8))");
      DDLSqlStmt createStmt(con, createTable);
      sword ret = createStmt.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }
      createStmt.commit();
      std::cout << "create statement execute status = " << ret << std::endl;
    }

    {
      std::string insertStr("INSERT INTO oracle_numbers VALUES(999012999012999012999012, .01234, 12345.678, 12.34567890)");
      DMLSqlStmt insertStmt(con, insertStr);
      sword ret = insertStmt.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }
      std::cout << "insert statement execute status = " << ret << std::endl;
      insertStmt.commit();
    }

    {
      std::string selectStr("SELECT num1, num2, num3, num4 FROM oracle_numbers");
      DMLSqlStmt selectStmt(con, selectStr);
      std::shared_ptr<base_define>  numDefs[] = {
        std::make_shared<number_define>(),
        std::make_shared<number_define>(),
        std::make_shared<number_define>(),
        std::make_shared<number_define>()
      };
      std::for_each(numDefs, numDefs + 4,
        [&](const std::shared_ptr<base_define>  &def)
      { selectStmt.setDef(def); }
      );

      sword ret = selectStmt.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }

      int idx = 1;
      std::for_each(numDefs, numDefs + 4, [&](const std::shared_ptr<base_define>  &def)
      {
        std::cout << "num" << idx << " : " << def->to_string() << std::endl;
        ++idx;
      });
      selectStmt.commit();
    }

    {
      DDLSqlStmt dropTable(con, "DROP TABLE oracle_numbers");
      sword ret = dropTable.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }
      std::cout << "drop table execute status = " << ret << std::endl;
      dropTable.commit();
    }

  }
  catch (const std::exception  &ex) {
    std::cout << "exception occurs: " << ex.what() << std::endl;
  }
}

void numbers_bind_example(const std::string  &user, const std::string  &pwd, const std::string  &serverName) {

  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(user, pwd, serverName);
    {
      std::string createTable("CREATE TABLE oracle_numbers_bind (num1 NUMBER(38), num2 NUMBER(4,5), num3 NUMBER(8,3), num4 NUMBER(10,8))");
      DDLSqlStmt createStmt(con, createTable);
      sword ret = createStmt.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }
      std::cout << "create statement execute status = " << ret << std::endl;
      createStmt.commit();
    }

    {
      std::string insertStr("INSERT INTO oracle_numbers_bind VALUES(:1,:2,:3,:4)");
      DMLSqlStmt insertStmt(con, insertStr);

      std::shared_ptr<base_bind> binds[] = {
        std::make_shared<number_bind>("999012999012999012999012"),
        std::make_shared<number_bind>(".01234"),
        std::make_shared<number_bind>("12345.678"),
        std::make_shared<number_bind>("12.34567890")
      };
      std::for_each(binds, binds + 4, [&](const std::shared_ptr<base_bind>  &bind)
      {
        insertStmt.setBind(bind);
      }
      );

      sword ret = insertStmt.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }
      std::cout << "insert statement execute status = " << ret << std::endl;
      insertStmt.commit();
    }

    {
      std::string selectStr("SELECT num1, num2, num3, num4 FROM oracle_numbers_bind");
      DMLSqlStmt selectStmt(con, selectStr);
      std::shared_ptr<base_define>  numDefs[] = {
        std::make_shared<number_define>(),
        std::make_shared<number_define>(),
        std::make_shared<number_define>(),
        std::make_shared<number_define>()
      };
      std::for_each(numDefs, numDefs + 4,
        [&](const std::shared_ptr<base_define>  &def)
      { selectStmt.setDef(def); }
      );

      sword ret = selectStmt.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }

      int idx = 1;
      std::for_each(numDefs, numDefs + 4, [&](const std::shared_ptr<base_define>  &def)
      {
        std::cout << "num" << idx << " : " << def->to_string() << std::endl;
        ++idx;
      });
      selectStmt.commit();
    }

    {
      DDLSqlStmt dropTable(con, "DROP TABLE oracle_numbers_bind");
      sword ret = dropTable.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }
      std::cout << "drop table execute status = " << ret << std::endl;
      dropTable.commit();
    }
  }
  catch (const std::exception  &ex) {
    std::cout << "exception occurs: " << ex.what() << std::endl;
  }
}

void interative_insert(const std::string  &user, const std::string  &pwd, const std::string  &serverName) {
  try {

    std::shared_ptr<Connection> con = std::make_shared<Connection>(user, pwd, serverName);
    {
      DDLSqlStmt createStmt(con, "CREATE TABLE insert_inter(col1 INT, col2 FLOAT(24))");
      sword ret = createStmt.execute();
      if (ret != OCI_SUCCESS) {
        std::cout << con->getErrorMessage(ret) << std::endl;
      }
      createStmt.commit();
    }
    {
      DMLSqlStmt insertIter(con, "INSERT INTO insert_inter VALUES(:1,:2)");
      std::shared_ptr<base_bind> binds[] = {
        std::make_shared<bind<int, SQLT_INT>>(1),
        std::make_shared<bind<double, SQLT_FLT>>(2)
      };
      std::for_each(binds, binds + 2, [&](const std::shared_ptr<base_bind> &bind) { insertIter.setBind(bind); });
      sword ret = insertIter.execute();

      const int NUM_ITERS = 10;

      std::shared_ptr<bind<int, SQLT_INT>> firstBind = std::static_pointer_cast<bind<int, SQLT_INT>>(binds[0]);
      std::shared_ptr<bind<double, SQLT_FLT>> secondBind = std::static_pointer_cast<bind<double, SQLT_FLT>>(binds[1]);
      for (int i = 1; i < NUM_ITERS; ++i) {
        firstBind->value_ += 1;
        secondBind->value_ += 2;
        ret = insertIter.execute();
      }
      insertIter.commit();
    }

  }
  catch (const std::exception  &ex) {
    std::cout << "exception occurs: " << ex.what() << std::endl;
  }
}

void transactions(const std::string  &user, const std::string  &pwd, const std::string  &serverName) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(user, pwd, serverName);
    Transaction trans1(con);
    std::shared_ptr<SqlStatement> &stmt = trans1.stmt();
    stmt.reset(new DMLSqlStmt(con, "INSERT INTO insert_inter VALUES(:1,:2)"));
    std::shared_ptr<DMLSqlStmt> insert = std::static_pointer_cast<DMLSqlStmt>(stmt);

    std::shared_ptr<base_bind> binds[] = {
      std::make_shared<bind<int, SQLT_INT>>(1),
      std::make_shared<bind<double, SQLT_FLT>>(2)
    };
    std::for_each(binds, binds + 2, [&](const std::shared_ptr<base_bind> &bind) { insert->setBind(bind); });

    if (trans1.execute()) {
      trans1.commit();
    }
    else {
      trans1.rollback();
    }

  }
  catch (const std::exception  &ex) {
    std::cout << "exception occurs :" << ex.what() << std::endl;
  }
}

//Locks: http://docs.oracle.com/javadb/10.8.3.0/devguide/cdevconcepts15366.html 
// Hm It looks there is no way to do it with single context(may be even with single Session):
// See cdemocp.c, cdemosp.c, 
// https://docs.oracle.com/cd/B10501_01/appdev.920/a96584/oci15r10.htm. OCILogon2 creates a new session.
// So There is no even the way to create it in the single session as I thought.

/*
static void transact_function(const std::shared_ptr<Connection>  &con) {

}

void threaded_transactions(const std::string  &user, const std::string  &pwd, const std::string  &serverName) {
  try {
    std::shared_ptr<Connection> con = std::make_shared<Connection>(user, pwd, serverName);
  } catch (const std::exception  &ex) {
    std::cout << "exception occurs :" << ex.what() << std::endl;
  }
}*/