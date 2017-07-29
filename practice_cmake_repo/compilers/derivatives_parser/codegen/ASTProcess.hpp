#pragma once

#include <map>
#include <functional>

#include "../parser_objects/common_incl.hpp"

class CodeGen;

//TODO: can be implemented as Visitor too.
class ASTProcess {
public:
  using func_map_t = std::map < base_ast_node::AST_NODE_TYPE, std::function<bool(CodeGen  &, const std::shared_ptr<base_ast_node>  &node)>>;
  ASTProcess(CodeGen  &gen);
  ~ASTProcess();
  bool generate(const std::shared_ptr<base_ast_node>  &node);
private:
  func_map_t func_map_;
  CodeGen & gen_;
};