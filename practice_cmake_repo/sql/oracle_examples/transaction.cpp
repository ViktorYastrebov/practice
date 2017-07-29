#include <stdexcept>
#include <ctime>

#include <sys/utime.h>

#include "transaction.hpp"

//Transaction::Transaction(SqlStatement  &stmt)
Transaction::Transaction(const std::shared_ptr<Connection>  &con)
  //:stmt_(stmt)
{
  
  /*
  OCIHandleAlloc((dvoid *)envhp, (dvoid **)&txnhp1, OCI_HTYPE_TRANS, 0, 0);
  OCIAttrSet((dvoid *)svchp, OCI_HTYPE_SVCCTX, (dvoid *)txnhp1, 0,
    OCI_ATTR_TRANS, errhp);

  // start a transaction with global transaction id = [1000, 123, 1]
  gxid.formatID = 1000; // format id = 1000
  gxid.gtrid_length = 3; // gtrid = 123
  gxid.data[0] = 1;
  gxid.data[1] = 2;
  gxid.data[2] = 3;
  gxid.bqual_length = 1; // bqual = 1/
  gxid.data[3] = 1;

  OCIAttrSet((dvoid *)txnhp1, OCI_HTYPE_TRANS, (dvoid *)&gxid, sizeof(XID),
    OCI_ATTR_XID, errhp);

  // start global transaction 1 with 60 second time to live when detached
  OCITransStart(svchp, errhp, 60, OCI_TRANS_NEW);
  */
  sword ret = OCIHandleAlloc(con->getEnvironment(), (void**)&trans_, OCI_HTYPE_TRANS, 0, 0);
  if (ret != OCI_SUCCESS) {
    throw std::runtime_error("can't allocate the transacation handler");
  }
  genTransName();
  ret = OCIAttrSet(con->getContext(), OCI_HTYPE_SVCCTX, (void*)trans_, 0, OCI_ATTR_TRANS, con->getError());
  if (ret != OCI_SUCCESS) {
    OCIHandleFree(trans_, OCI_HTYPE_TRANS);
    throw std::runtime_error(con->getErrorMessage(ret));
  }
  ret = OCIAttrSet(trans_, OCI_HTYPE_TRANS, (text*)trans_name_.c_str(), trans_name_.length(), OCI_ATTR_TRANS_NAME, con->getError());
  if (ret != OCI_SUCCESS) {
    OCIHandleFree(trans_, OCI_HTYPE_TRANS);
    throw std::runtime_error(con->getErrorMessage(ret));
  }
  ret = OCITransStart(con->getContext(), con->getError(), 60, OCI_TRANS_NEW);
  if (ret != OCI_SUCCESS) {
    OCIHandleFree(trans_, OCI_HTYPE_TRANS);
    throw std::runtime_error(con->getErrorMessage(ret));
  }
}

Transaction::~Transaction() {
  OCIHandleFree(trans_, OCI_HTYPE_TRANS);
}

bool Transaction::execute() {
  sword ret = stmt_->execute();
  return ret == OCI_SUCCESS;
}

void Transaction::commit() {
  OCITransCommit(stmt_->getConnection()->getContext(), stmt_->getConnection()->getError(), (ub4)0);
}

void Transaction::rollback() {
  OCITransRollback(stmt_->getConnection()->getContext(), stmt_->getConnection()->getError(), (ub4)0);
}

const std::shared_ptr<SqlStatement>  &Transaction::stmt() const {
  return stmt_;
}

std::shared_ptr<SqlStatement>  &Transaction::stmt() {
  return stmt_;
}

//TODO: it's not the perfect way. Need to get miliseconds at least
void Transaction::genTransName() {
  trans_name_ = "Transaction:";
  std::time_t t = std::time(nullptr);
  char buff[128];
  std::strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
  trans_name_.append(buff);
}

const std::string  &Transaction::getName() const {
  return trans_name_;
}

std::string  &Transaction::getName() {
  return trans_name_;
}

//void commit();
//void rollback();
