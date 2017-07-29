#include <stdexcept>

#include "codegen.hpp"

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Bitcode/ReaderWriter.h>

#include <llvm/Pass.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>

CodeGen::CodeGen()
  :ast_(),
   ctx_(), 
   module_(new llvm::Module("new module", ctx_)),
   //builder_(ctx_),
   astProcess_(*this),
   builder_(nullptr)
{}

CodeGen::CodeGen(const std::shared_ptr<base_ast_node>  &ast)
  :ast_(ast),
   ctx_(),
   module_(new llvm::Module("new module", ctx_)),
   //builder_(ctx_),
   astProcess_(*this),
   builder_(nullptr)
{}

CodeGen::~CodeGen()
{}

void CodeGen::setAST(const std::shared_ptr<base_ast_node>  &ast) {
  ast_ = ast;
}

void CodeGen::BuildByteCode() {

  if (!ast_) {
    throw std::runtime_error("AST node is empty. Nothing to generate");
  }

  std::vector<llvm::Type *> types = {
    llvm::Type::getInt32Ty(ctx_)
  };
  llvm::Function  *mainFunc = createFunction("main", types, llvm::GlobalValue::LinkageTypes::InternalLinkage);
  llvm::BasicBlock  *mainBB = llvm::BasicBlock::Create(ctx_, "mainBB", mainFunc);
  llvm::IRBuilder<> builder(mainBB);
  builder_ = &builder;
  astProcess_.generate(ast_);

  llvm::Value  *retVal = llvm::ConstantInt::get(types[0], 0, true);
  builder_->CreateRet(retVal);


  //https://gist.github.com/alloy/d86b007b1b14607a112f

  llvm::legacy::FunctionPassManager funcPass(module_.get());
  
  /*llvm::EngineBuilder factory(std::move(module_.get()));
  factory.setEngineKind(llvm::EngineKind::JIT);
  factory.setTargetOptions(Opts);
  factory.setMCJITMemoryManager(std::move(MemMgr));
  llvm::ExecutionEngine *executionEngine = factory.create();*/

  //funcPass.add(llvm::createPrintBasicBlockPass());
  funcPass.add(llvm::createVerifierPass());
  funcPass.add(llvm::createFloat2IntPass());

  bool ret = funcPass.doInitialization();

  //funcPass.doInitialization();

  //llvm::PassManager< PM;

  //funcPass.
  //llvm::PassManager<> Pass();
  //llvm::PassManager
}

void CodeGen::dumpModule(const std::string  &path) {

  //llvm::
  std::error_code err;
  llvm::raw_fd_ostream fd(path, err, llvm::sys::fs::OpenFlags::F_None);
  if (!err) {
    llvm::WriteBitcodeToFile(module_.get(), fd);
    fd.flush();
  }
}

const llvm::Module*  CodeGen::getModule() const {
  return module_.get();
}

llvm::Module*  CodeGen::getModule() {
  return module_.get();
}

const llvm::LLVMContext  &CodeGen::getCtx() const {
  return ctx_;
}

llvm::LLVMContext  &CodeGen::getCtx() {
  return ctx_;
}

const std::list<llvm::Value*>  &CodeGen::getValues() const {
  return vals_;
}

std::list<llvm::Value*>  &CodeGen::getValues() {
  return vals_;
}

const ASTProcess  &CodeGen::getASTProcessor() const {
  return astProcess_;
}

ASTProcess  &CodeGen::getASTProcessor() {
  return astProcess_;
}

const llvm::IRBuilder<>  *CodeGen::getCurrentBuilder() const {
  return builder_;
}

llvm::IRBuilder<>  *CodeGen::getCurrentBuilder() {
  return builder_;
}

const CodeGen::vars_map_t  &CodeGen::getVars() const {
  return vars_map_;
}

CodeGen::vars_map_t  &CodeGen::getVars() {
  return vars_map_;
}


llvm::Function  *CodeGen::createFunction(const std::string  &name, const std::vector <llvm::Type*>  &types, llvm::Function::LinkageTypes lnkType) {
  llvm::FunctionType  *funTy = nullptr;
  if (types.size() > 1) {
    funTy = llvm::FunctionType::get(types[0], types[1], false);
  } else {
    funTy = llvm::FunctionType::get(types[0], false);
  }

  llvm::Function  *func = llvm::Function::Create(funTy, lnkType, name, module_.get());
  func->setCallingConv(llvm::CallingConv::C);
  return func;
}