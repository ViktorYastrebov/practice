#pragma once

#include <memory>
//TODO: switch to std::experimental::filesystem::path
#include <string>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>

#include <llvm/IR/Constant.h>
#include <llvm/IR/GlobalVariable.h>


#include <llvm/IR/BasicBlock.h>

#include "llvm/ADT/APFloat.h"

#include <llvm/IR/DerivedTypes.h>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

//TODO: move it to separate class
#include "../parser_objects/common_incl.hpp"
#include "ASTProcess.hpp"


class CodeGen {
public:
  CodeGen();
  CodeGen(const std::shared_ptr<base_ast_node>  &ast);
  ~CodeGen();

  using vars_map_t = std::map<std::string, llvm::AllocaInst*>;

  void setAST(const std::shared_ptr<base_ast_node>  &ast);

  const llvm::Module*  getModule() const;
  llvm::Module*  getModule();

  const llvm::LLVMContext  &getCtx() const;
  llvm::LLVMContext  &getCtx();

  void BuildByteCode();
  void dumpModule(const std::string  &path);

  const std::list<llvm::Value*>  &getValues() const;
  std::list<llvm::Value*>  &getValues();

  const ASTProcess  &getASTProcessor() const;
  ASTProcess  &getASTProcessor();

  const llvm::IRBuilder<>  *getCurrentBuilder() const;
  llvm::IRBuilder<>  *getCurrentBuilder();

  const vars_map_t  &getVars() const;
  vars_map_t  &getVars();

private:
  std::shared_ptr<base_ast_node>  ast_;
  llvm::LLVMContext ctx_;
  std::unique_ptr<llvm::Module>  module_;
  std::list<llvm::Value*> vals_;
  ASTProcess astProcess_;
  llvm::IRBuilder<> *builder_;

  vars_map_t vars_map_;

private:
//It's about the main()
//http ://llvm.org/releases/2.6/docs/tutorial/JITTutorial1.html.
  llvm::Function  *createFunction(const std::string  &name,
      const std::vector <llvm::Type*>  &types, llvm::Function::LinkageTypes lnkType);
};