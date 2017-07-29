#include "ASTProcess.hpp"
#include "codegen.hpp"

#define GEN_FUNC(name) static bool name(CodeGen  &gen, const std::shared_ptr<base_ast_node>   &node)

//static bool GenerateContant

/*static llvm::Type  *getLexicographicalType(const llvm::Value  *left, const llvm::Value  *right) {
  if (left->getType()->isDoubleTy() && right->getType()->isIntegerTy()) {
    return left->getType();
  }
  if (left->getType()->isIntegerTy() && right->getType()->isDoubleTy()) {
    return right->getType();
  }
  return nullptr;
}*/

/*
static std::pair < llvm::Value  *, llvm::Type*> getLexicographicalType(llvm::Value *left, llvm::Value  *right) {
  if (left->getType()->isDoubleTy() && right->getType()->isIntegerTy()) {
    return std::make_pair(right,left->getType());
  }
  if (left->getType()->isIntegerTy() && right->getType()->isDoubleTy()) {
    return std::make_pair(left, right->getType());
  }
  return{ nullptr, nullptr };
}*/

GEN_FUNC(GenerateConstInt) {
  bool ret = true;
  std::shared_ptr<constant<int>>  cnst = std::static_pointer_cast<constant<int>>(node);
  //uint64_t val = cnst->value();
  //llvm::Value  *valCnst = llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(gen.getCtx()), val, true);
  llvm::Value  *valCnst = llvm::ConstantFP::get(llvm::Type::getDoubleTy(gen.getCtx()), static_cast<double>(cnst->value()));
  ret = (valCnst != nullptr);
  gen.getValues().push_back(valCnst);
  return ret;
}

GEN_FUNC(GenerateConstDouble) {
  bool ret = true;
  std::shared_ptr<constant<double>> cnst = std::static_pointer_cast<constant<double>>(node);
  llvm::Value *val = llvm::ConstantFP::get(llvm::Type::getDoubleTy(gen.getCtx()), cnst->value());
  ret = (val != nullptr);
  gen.getValues().push_back(val);
  return ret;
}

GEN_FUNC(GenerateConstantBase) {
  std::shared_ptr<constant_base>  cnst = std::static_pointer_cast<constant_base>(node);
  std::string type_name = cnst->type().name();
  if (type_name == "unsigned int") {
    return GenerateConstInt(gen, cnst);
  }
  if (cnst->type().name() == "double") {
    return GenerateConstDouble(gen, node);
  }
  return false;
}

GEN_FUNC(GenerateBracketedExpr) {
  std::shared_ptr<bracket_expr> brecket_expr = std::static_pointer_cast<bracket_expr>(node);
  return gen.getASTProcessor().generate(brecket_expr->expr());
}

GEN_FUNC(GenerateBinOp) {
  bool ret = true;
  std::shared_ptr<bin_op_node>  bin = std::static_pointer_cast<bin_op_node>(node);
  bool leftProcced = gen.getASTProcessor().generate(bin->get_left());
  if (!leftProcced) {
    return false;
  }
  bool rightProceed = gen.getASTProcessor().generate(bin->get_right());

  std::list<llvm::Value*> &vals = gen.getValues();
  if (!rightProceed || vals.size() < 2) {
    return false;
  }

  llvm::Value  *right = vals.back();
  vals.pop_back();
  llvm::Value  *left = vals.back();
  vals.pop_back();

  llvm::Value  *res = nullptr;

  //"Cannot create binary operator with two operands of differing type!");
  //have to make the manual cast
  /*if (left->getType()->getTypeID() != right->getType()->getTypeID()) {
    if (left->getType()->isDoubleTy() && right->getType()->isIntegerTy()) {
      right = gen.getCurrentBuilder()->CreateCast(llvm::Instruction::CastOps::UIToFP, right, left->getType(), "castOp");
    }
    if (left->getType()->isIntegerTy() && right->getType()->isDoubleTy()) {
      left = gen.getCurrentBuilder()->CreateCast(llvm::Instruction::CastOps::UIToFP, left, right->getType(), "castOp");
    }
  }*/

  switch (bin->get_op()) {
  case bin_op_node::PLUS:
  {
    //"Tried to create an integer operation on a non-integer type!");
    res =  gen.getCurrentBuilder()->CreateFAdd(left, right, "plusOp");
  }break;
  case bin_op_node::MINUS:
  {
    res = gen.getCurrentBuilder()->CreateSub(left, right, "subOp");
  }break;
  case bin_op_node::MUL:
  {
    //res = gen.getCurrentBuilder()->CreateMul(left, right, "mulOp");
    res = gen.getCurrentBuilder()->CreateFMul(left, right, "mulOp");
  }break;
  case bin_op_node::DIV:
  {
    res = gen.getCurrentBuilder()->CreateSDiv(left, right, "divOp");
  }break;
  case bin_op_node::POWER:
  {
    //It looks like there is no default Power operation.
    //if it's constant value It can be calculated here and unroll the loop. otherwise it can be 
    return false;
  }break;
  case bin_op_node::NO_OP:
  default:
    return false;
  };

  ret = res != nullptr;
  vals.push_back(res);

  return ret;
}

GEN_FUNC(GenerateUnaryOp) {
  std::shared_ptr<unary_op>  unOp = std::static_pointer_cast<unary_op>(node);
  return true;
}

GEN_FUNC(GenerateVar) {
  std::shared_ptr<identifier> ident = std::static_pointer_cast<identifier>(node);
  std::string name = ident->toString();

  const CodeGen::vars_map_t &vars =  gen.getVars();
  CodeGen::vars_map_t::const_iterator it = vars.find(name);
  if (it != vars.end()) {
    llvm::Value*  var = gen.getCurrentBuilder()->CreateLoad(it->second);
    gen.getValues().push_back(var);
  } else {
    llvm::AllocaInst  *inst = gen.getCurrentBuilder()->CreateAlloca(llvm::Type::getDoubleTy(gen.getCtx()), nullptr, name);
    CodeGen::vars_map_t &varsInst = gen.getVars();
    varsInst.insert(std::make_pair(name, inst));
    gen.getValues().push_back(gen.getCurrentBuilder()->CreateLoad(inst));
  }
  return true;
}

ASTProcess::ASTProcess(CodeGen  &gen)
  :gen_(gen)
{
  func_map_ = {
    { base_ast_node::BIN_OP,   std::bind(GenerateBinOp, std::placeholders::_1, std::placeholders::_2)   },
    { base_ast_node::UN_OP,    std::bind(GenerateUnaryOp, std::placeholders::_1, std::placeholders::_2) },
    { base_ast_node::CONSTANT, std::bind(GenerateConstantBase, std::placeholders::_1, std::placeholders::_2) },
    { base_ast_node::IDENT,    std::bind(GenerateVar, std::placeholders::_1, std::placeholders::_2) },
    { base_ast_node::BRACKETED_EXPR, std::bind(GenerateBracketedExpr, std::placeholders::_1, std::placeholders::_2) }
  };
}

ASTProcess::~ASTProcess()
{}

//TODO: the reason. Can be optimized with hash table. It gives O(1) if the function has been used previously
bool ASTProcess::generate(const std::shared_ptr<base_ast_node>  &node) {
  func_map_t::const_iterator it = func_map_.find(node->id());
  if (it != func_map_.end()) {
    return it->second(gen_, node);
  }
  return false;
}

