#pragma once

#include <unordered_map>

#include <locale>
#include <codecvt>

#include "expr_print_visitor.h"

#include "tuples_hash.hpp"

class const_expr_helper
{
public:
  virtual std::shared_ptr<expr_node> op(const std::shared_ptr<constant_base>   &l, const std::shared_ptr<constant_base>  &r) const = 0;
  virtual ~const_expr_helper() {}
};

template<class T, class U, bin_op_node::BINOP_TYPE oper>
class const_expr_impl : public const_expr_helper {
public:
  virtual std::shared_ptr<expr_node> op(const std::shared_ptr<constant_base>   &l, const std::shared_ptr<constant_base>  &r) const override {
    return std::shared_ptr<expr_node>();
  }
  virtual ~const_expr_impl() {}
};

template<class T, class U>
class const_expr_impl<T, U, bin_op_node::PLUS> : public const_expr_helper {
public:
  std::shared_ptr<expr_node> op(const std::shared_ptr<constant_base>   &l, const std::shared_ptr<constant_base>  &r) const override {
    T t;
    U u;
    return std::shared_ptr<expr_node>(new constant<decltype(t + u)>(std::dynamic_pointer_cast<constant<T>>(l)->value() + std::dynamic_pointer_cast<constant<U>>(r)->value()));
  }
  virtual ~const_expr_impl() {}
};

template<class T, class U>
class const_expr_impl<T, U, bin_op_node::MINUS>: public const_expr_helper {
public:
  std::shared_ptr<expr_node> op(const std::shared_ptr<constant_base>   &l, const std::shared_ptr<constant_base>  &r) const override {
    T t;
    U u;
    return std::shared_ptr<expr_node>(new constant<decltype(t - u)>(std::dynamic_pointer_cast<constant<T>>(l)->value() - std::dynamic_pointer_cast<constant<U>>(r)->value()));
  }
  virtual ~const_expr_impl() {}
};

template<class T, class U>
class const_expr_impl<T, U, bin_op_node::MUL> : public const_expr_helper {
public:
  std::shared_ptr<expr_node> op(const std::shared_ptr<constant_base>   &l, const std::shared_ptr<constant_base>  &r) const override {
    T t;
    U u;
    return std::shared_ptr<expr_node>(new constant<decltype(t*u)>(std::dynamic_pointer_cast<constant<T>>(l)->value()*std::dynamic_pointer_cast<constant<U>>(r)->value()));
  }
  virtual ~const_expr_impl() {}
};

template<class T, class U>
class const_expr_impl<T, U, bin_op_node::DIV> : public const_expr_helper {
public:
  std::shared_ptr<expr_node> const_expr_impl<T, U, bin_op_node::DIV>::op(const std::shared_ptr<constant_base>   &l, const std::shared_ptr<constant_base>  &r) const override {
    std::shared_ptr<constant<U>> right = std::dynamic_pointer_cast<constant<U>>(r);
    if (right->value() == U()) {
      throw std::runtime_error("division by 0");
    }
    std::shared_ptr<constant<T>> left = std::dynamic_pointer_cast<constant<T>>(l);
    T t;
    U u;
    return std::shared_ptr<expr_node>(new constant<decltype(t / u)>(left->value() / right->value()));
  }
  virtual ~const_expr_impl() {}
};

template<class T, class U>
class const_expr_impl<T, U, bin_op_node::POWER> : public const_expr_helper {
public:
  std::shared_ptr<expr_node> const_expr_impl<T, U, bin_op_node::POWER>::op(const std::shared_ptr<constant_base>   &l, const std::shared_ptr<constant_base>  &r) const override {
    //?????
    /*if (std::is_floating_point<U>::value) {
      throw std::logic_error("float power is not supported");
    }*/
    return std::shared_ptr<expr_node>(new constant<T>(std::pow<T,U>(std::dynamic_pointer_cast<constant<T>>(l)->value(), std::dynamic_pointer_cast<constant<U>>(r)->value())));
  }
  virtual ~const_expr_impl() {}
};


class const_op_builder {
public:
  using tuple_t = std::tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>;
  using map_t = std::unordered_map<tuple_t, std::shared_ptr<const_expr_helper>, tuple_hash<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>>;

  const_op_builder()
  {
    map_ = {
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(unsigned int)), bin_op_node::PLUS),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, unsigned int, bin_op_node::PLUS>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(double)), bin_op_node::PLUS),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, double, bin_op_node::PLUS>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(unsigned int)), bin_op_node::PLUS),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, unsigned int, bin_op_node::PLUS>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(double)), bin_op_node::PLUS),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, double, bin_op_node::PLUS>)),

      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(unsigned int)), bin_op_node::MINUS),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, unsigned int, bin_op_node::MINUS>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(double)), bin_op_node::MINUS),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, double, bin_op_node::MINUS>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(unsigned int)), bin_op_node::MINUS),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, unsigned int, bin_op_node::MINUS>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(double)), bin_op_node::MINUS),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, double, bin_op_node::MINUS>)),

      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(unsigned int)), bin_op_node::MUL),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, unsigned int, bin_op_node::MUL>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(double)), bin_op_node::MUL),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, double, bin_op_node::MUL>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(unsigned int)), bin_op_node::MUL),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, unsigned int, bin_op_node::MUL>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(double)), bin_op_node::MUL),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, double, bin_op_node::MUL>)),

      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(unsigned int)), bin_op_node::DIV),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, unsigned int, bin_op_node::DIV>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(double)), bin_op_node::DIV),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, double, bin_op_node::DIV>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(unsigned int)), bin_op_node::DIV),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, unsigned int, bin_op_node::DIV>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(double)), bin_op_node::DIV),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, double, bin_op_node::DIV>)),

      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(unsigned int)), bin_op_node::POWER),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, unsigned int, bin_op_node::POWER>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(double)), bin_op_node::POWER),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, double, bin_op_node::POWER>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(unsigned int)), std::type_index(typeid(unsigned int)), bin_op_node::POWER),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<unsigned int, unsigned int, bin_op_node::POWER>)),
      std::make_pair(std::make_tuple<std::type_index, std::type_index, bin_op_node::BINOP_TYPE>(
        std::type_index(typeid(double)), std::type_index(typeid(double)), bin_op_node::POWER),
        std::shared_ptr<const_expr_helper>(new const_expr_impl<double, double, bin_op_node::POWER>))
    };
  }

  std::shared_ptr<const_expr_helper> getBy(const tuple_t  &idx) {
    return map_.find(idx)->second;
  }

private:
  map_t map_;
};

//http://codereview.stackexchange.com/questions/72214/simple-mathematical-operations-add-sub-mul-div-in-c11-template
//http://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set

//TO DO: tuple hash mapping. I'm not sure that type[i] ^ type[i+1] is good here.
//may be have to make the compiletime mapping. The id will be returned by contant instantination


//I've got it. WE just can cast to the lerger lexigraphical type. For example.
//compare: double & float -> cast float->double and compare. That is it
//template<class T, class U>
//struct lexigraphical_type {};
//struct<float, double> lexigraphical_type { typedef result_t double; }
//struct<double, float> lexigraphical_type { typedef result_t double; }

template<class T>
struct equal
{
  bool operator()(const std::shared_ptr<expr_node>  &cnst, const std::shared_ptr<expr_node>  &val) {
    return std::static_pointer_cast<constant<T>>(cnst)->value() == std::static_pointer_cast<constant<T>>(val)->value();
  }
};

template<bin_op_node::BINOP_TYPE op, unsigned int value = 0>
class reducer {
public:
  using map_t = std::unordered_map< std::tuple<std::type_index, std::type_index>, std::function<bool(const std::shared_ptr<expr_node>&, const std::shared_ptr<expr_node>  &)>,
    tuple_hash<std::type_index, std::type_index> >;

  reducer()
  {
    map_ = {
      std::make_pair(std::make_tuple<std::type_index, std::type_index>(typeid(unsigned int), typeid(unsigned int)), equal<unsigned int>()),
      std::make_pair(std::make_tuple<std::type_index, std::type_index>(typeid(double), typeid(double)), equal<double>())
    };
  }

  std::shared_ptr<expr_node>  operator()(const std::shared_ptr<expr_node>  &l, std::shared_ptr<expr_node>  &r) {
    if (l->isConstant()) {
      std::shared_ptr<constant_base> cnst = std::static_pointer_cast<constant_base>(l);

      std::shared_ptr<constant<double>> oneDouble(new constant<double>(1.0));
      std::shared_ptr<constant<unsigned int>> oneUInt(new constant<unsigned int>(1));

      std::shared_ptr<constant<double>> zeroDouble(new constant<double>(0.0));
      std::shared_ptr<constant<unsigned int>> zeroUInt(new constant<unsigned int>(0));

      map_t::const_iterator it = map_.find(std::make_tuple(cnst->type(), oneDouble->type()));
      map_t::const_iterator it2 = map_.find(std::make_tuple(cnst->type(), oneUInt->type()));
      bool isEqualOne = false;
      if (it != map_.end()) {
        isEqualOne |= it->second(cnst, oneDouble);
      }
      if (it2 != map_.end()) {
        isEqualOne |= it2->second(cnst, oneUInt);
      }
      if (isEqualOne) {
        return r;
      }

      it = map_.find(std::make_tuple(cnst->type(), zeroDouble->type()));
      it2 = map_.find(std::make_tuple(cnst->type(), zeroUInt->type()));
      bool isEqualZero = false;
      if (it != map_.end()) {
        isEqualZero |= it->second(cnst, zeroDouble);
      }
      if (it2 != map_.end()) {
        isEqualZero |= it2->second(cnst, zeroUInt);
      }
      if (isEqualZero) {
        //for X/0 exception should be thrown
        return std::shared_ptr<constant<unsigned int>>(new constant<unsigned int>(value));
      }
    }

    if (r->isConstant()) {
      std::shared_ptr<constant_base> cnst = std::static_pointer_cast<constant_base>(r);

      std::shared_ptr<constant<double>> oneDouble(new constant<double>(1.0));
      std::shared_ptr<constant<unsigned int>> oneUInt(new constant<unsigned int>(1));

      std::shared_ptr<constant<double>> zeroDouble(new constant<double>(0.0));
      std::shared_ptr<constant<unsigned int>> zeroUInt(new constant<unsigned int>(0));

      map_t::const_iterator it = map_.find(std::make_tuple(cnst->type(), oneDouble->type()));
      map_t::const_iterator it2 = map_.find(std::make_tuple(cnst->type(), oneUInt->type()));
      bool isEqualOne = false;
      if (it != map_.end()) { 
        isEqualOne |= it->second(cnst, oneDouble);
      }
      if (it2 != map_.end()) {
        isEqualOne |= it2->second(cnst, oneUInt);
      }
      if (isEqualOne) {
        return l;
      }

      it = map_.find(std::make_tuple(cnst->type(), zeroDouble->type()));
      it2 = map_.find(std::make_tuple(cnst->type(), zeroUInt->type()));
      bool isEqualZero = false;
      if (it != map_.end()) {
        isEqualZero |= it->second(cnst, zeroDouble);
      } 
      if (it2 != map_.end()) {
        isEqualZero |= it2->second(cnst, zeroUInt);
      }

      if (isEqualZero) {
        return std::shared_ptr<constant<unsigned int>>(new constant<unsigned int>(value));
      }
    }
    return std::shared_ptr<expr_node>();
  }
private:
  map_t map_;

};

struct reduced_map {

  using map_t = std::unordered_map<bin_op_node::BINOP_TYPE, std::function< std::shared_ptr<expr_node>(const std::shared_ptr<expr_node>&, std::shared_ptr<expr_node>&)> >;

  reduced_map() {
    op_map_ = {
      std::make_pair(bin_op_node::BINOP_TYPE::MUL, reducer<bin_op_node::BINOP_TYPE::MUL>()),
      std::make_pair(bin_op_node::BINOP_TYPE::DIV, reducer<bin_op_node::BINOP_TYPE::DIV>()),
      std::make_pair(bin_op_node::BINOP_TYPE::POWER, reducer<bin_op_node::BINOP_TYPE::POWER, 1>())
    };
  }

  std::shared_ptr<expr_node> reduceByOp(const bin_op_node  &binOp) const {
    map_t::const_iterator it = op_map_.find(binOp.get_op());
    if (it != op_map_.end()) {
      return it->second(binOp.get_left(), binOp.get_right());
    }
    return std::shared_ptr<expr_node>();
  }

private:
  map_t op_map_;
};


#if TYPE_CHECKING
struct base_type_map {
  virtual bool isEqual(const std::shared_ptr<expr_node>  &var) const = 0;
  virtual ~base_type_map() {}
};

template<class T, T value>
struct type_map_impl {
  bool isEqual(const std::shared_ptr<expr_node>  &var) {
    std::shared_ptr<constant<T>> cnst = std::dynamic_pointer_cast<constant<T>>(var);
    return var->value() == value;
  }
  virtual ~type_map_impl() {}
};

struct type_map_builder {
private:
  type_map_builder()
  {
    map_ = {
      std::make_pair(std::type_index(typeid(unsigned int)), std::shared_ptr <base_type_map>(new type_map_impl<unsigned int, 0>())),
      std::make_pair(std::type_index(typeid(unsigned int)), std::shared_ptr <base_type_map>(new type_map_impl<unsigned int, 1>())),
      std::make_pair(std::type_index(typeid(double)), std::shared_ptr <base_type_map>(new type_map_impl<double, 0>())),
      std::make_pair(std::type_index(typeid(double)), std::shared_ptr <base_type_map>(new type_map_impl<double, 1>()))
    };
  }
public:
  using map_t = std::unordered_map<std::tuple<std::type_index, std::shared_ptr<base_type_map>>;
  static type_map_builder  &instance() {
    static type_map_builder builder;
    return builder;
  }
  map_t map_;
};
//TODO can be more generalized
class expr_reduce_base {
public:
  virtual std::shared_ptr<expr_node> reduce(const std::shared_ptr<expr_node>  &left, const std::shared_ptr<expr_node>  &right, bool isLeftConst, bool isRightConst) const = 0;
  virtual ~expr_reduce_base() {}
};

template<bin_op_node::BINOP_TYPE oper>
class expr_reduce_impl : public expr_reduce_base
{
public:
  virtual std::shared_ptr<expr_node> reduce(const std::shared_ptr<expr_node>  &left, const std::shared_ptr<expr_node>  &right, bool isLeftConst, bool isRightConst) const override {
    if (isLeftConst && !isRightConst) {
      std::shared_ptr<constant_base> cnst = std::dynamic_pointer_cast<constant_base>(left);
      type_map_builder::map_t::const_iterator it = type_map_builder::instance().map_.find(cnst->type());

      if(it->second->isEqual(cnst)) {
        return std::shared_ptr<constant<unsigned int>>(new constant<unsigned int>(0));
      }
      //it->second->set(1);
      if (it->second->isEqual(cnst)) {
        return right;
      }
    }

    if (!isLeftConst  && isRightConst) {
      std::shared_ptr<constant_base> cnst = std::dynamic_pointer_cast<constant>(right);
      type_map_builder::map_t::const_iterator it = type_map_builder::instance().map_.find(cnst->type());
      if (it->second->isEqual(cnst)) {
        return std::shared_ptr<constant<unsigned int>>(new constant<unsigned int>(0));
      }
      it->second->set
      if (cnst->value() == T(1)) {
        return left;
      }
    }
    return std::shared_ptr<expr_node>();
  }
  virtual ~expr_reduce_impl() {}
};


template<class T>
class expr_reduce_impl<T, bin_op_node::POWER> : public expr_reduce_base
{
public:
  virtual std::shared_ptr<expr_node> reduce(const std::shared_ptr<expr_node>  &left, const std::shared_ptr<expr_node>  &right, bool isLeftConst, bool isRightConst) const override {
    if (isLeftConst && !isRightConst) {
      std::shared_ptr<constant<T>> cnst = std::dynamic_pointer_cast<constant<T>>(left);
      if (cnst->value() == T()) {
        //diff is here
        return std::shared_ptr<constant<unsigned int>>(new constant<unsigned int>(1));
      }
      if (cnst->value() == T(1)) {
        return right;
      }
    }
    //can be done by 2 depth recursion. But need to stop it by some flag;
    //return reduce<U, T, oper>(r, l, isRightConst, isLeftConst);
    if (!isLeftConst  && isRightConst) {
      std::shared_ptr<constant<T>> cnst = std::dynamic_pointer_cast<constant<T>>(right);
      if (cnst->value() == T()) {
        //diff is here
        return std::shared_ptr<constant<unsigned int>>(new constant<unsigned int>(1));
      }
      if (cnst->value() == T(1)) {
        return left;
      }
    }
    return std::shared_ptr<expr_node>();
  }
  virtual ~expr_reduce_impl() {}
};


struct expr_reduce_builder {
  /*using tuple_t = std::tuple<std::type_index, bin_op_node::BINOP_TYPE>;
  using map_t = std::unordered_map<tuple_t, std::shared_ptr<expr_reduce_base>, tuple_hash<std::type_index, bin_op_node::BINOP_TYPE>>;
  expr_reduce_builder()
  {
    reduce_map = {
      std::make_pair(std::make_tuple<std::type_index, bin_op_node::BINOP_TYPE>(typeid(unsigned int), bin_op_node::MUL),
        std::shared_ptr<expr_reduce_base>(new expr_reduce_impl<unsigned int, bin_op_node::MUL>())),
      std::make_pair(std::make_tuple<std::type_index, bin_op_node::BINOP_TYPE>(typeid(double), bin_op_node::MUL),
        std::shared_ptr<expr_reduce_base>(new expr_reduce_impl<double, bin_op_node::MUL>())),

      std::make_pair(std::make_tuple<std::type_index, bin_op_node::BINOP_TYPE>(typeid(unsigned int), bin_op_node::DIV),
        std::shared_ptr<expr_reduce_base>(new expr_reduce_impl<unsigned int, bin_op_node::DIV>())),
      std::make_pair(std::make_tuple<std::type_index, bin_op_node::BINOP_TYPE>(typeid(double), bin_op_node::DIV),
        std::shared_ptr<expr_reduce_base>(new expr_reduce_impl<double, bin_op_node::DIV>())),

      std::make_pair(std::make_tuple<std::type_index, bin_op_node::BINOP_TYPE>(typeid(unsigned int), bin_op_node::POWER),
        std::shared_ptr<expr_reduce_base>(new expr_reduce_impl<unsigned int, bin_op_node::POWER>())),
      std::make_pair(std::make_tuple<std::type_index, bin_op_node::BINOP_TYPE>(typeid(double), bin_op_node::POWER),
        std::shared_ptr<expr_reduce_base>(new expr_reduce_impl<double, bin_op_node::POWER>()))
    };
  }
  map_t reduce_map;*/
};
#endif



enum state {
  general = 0,
  process_expr,
  process_power_expr
};


template<state type>
class derivative_builder :
  public Loki::BaseVisitor,
  public Loki::Visitor<base_ast_node>,
  public Loki::Visitor<expr_node>,
  public Loki::Visitor<bracket_expr>,
  public Loki::Visitor<constant<unsigned int>>,
  public Loki::Visitor<constant<double>>,
  public Loki::Visitor<identifier>,
  public Loki::Visitor<constant_variable>,
  public Loki::Visitor<unary_op>,
  public Loki::Visitor<bin_op_node>,
  public Loki::Visitor<trigonometric_expr<TRIG_FUNC::COS>>,
  public Loki::Visitor<trigonometric_expr<TRIG_FUNC::SIN>>,
  public Loki::Visitor<trigonometric_expr<TRIG_FUNC::TAN>>
{
public:
  using result_t = std::stack<std::shared_ptr<base_ast_node>>;
  derivative_builder();
  derivative_builder(const std::shared_ptr<base_ast_node>  &ast);

  virtual ~derivative_builder();

  void process();

  void Visit(base_ast_node  &base);
  void Visit(expr_node  &expr);
  void Visit(bracket_expr &br_expr);
  void Visit(constant<unsigned int>  &cnst);
  void Visit(constant<double>  &cnst);
  void Visit(identifier  &ident);
  void Visit(constant_variable  &var);
  void Visit(unary_op  &un);
  void Visit(bin_op_node  &bin);
  void Visit(trigonometric_expr<TRIG_FUNC::COS>  &cos);
  void Visit(trigonometric_expr<TRIG_FUNC::SIN>  &sin);
  void Visit(trigonometric_expr<TRIG_FUNC::TAN>  &tan);

  std::shared_ptr<base_ast_node>  get_deriv_expr() const;
private:
  std::shared_ptr<base_ast_node>  ast_;
  result_t  deriv_expr_;
};

template<state type>
derivative_builder<type>::derivative_builder()
{}

template<state type>
derivative_builder<type>::derivative_builder(const std::shared_ptr<base_ast_node>  &ast)
  :ast_(ast)
{}

template<state type>
derivative_builder<type>::~derivative_builder()
{}


template<state type>
void derivative_builder<type>::process() {
  this->Visit(*ast_);
}

template<state type>
void derivative_builder<type>::Visit(base_ast_node  &base) {
  base.Accept(*this);
}

template<state type>
void derivative_builder<type>::Visit(expr_node  &expr) {
  expr.Accept(*this);
}

template<state type>
void derivative_builder<type>::Visit(bracket_expr &br_expr) {
  br_expr.expr()->Accept(*this);

  std::shared_ptr<base_ast_node>  top = deriv_expr_.top();
  deriv_expr_.pop();

  if (top->id() == base_ast_node::CONSTANT) {
    derivative_builder<state::process_expr> cnstBuilder;
    cnstBuilder.Visit(*top);
    deriv_expr_.push(cnstBuilder.get_deriv_expr());
  } else {
    std::shared_ptr<base_ast_node> val(new bracket_expr(std::static_pointer_cast<expr_node>(top)));
    deriv_expr_.push(val);
  }
}


template<>
void derivative_builder<state::general>::Visit(constant<unsigned int>  &cnst) {
  std::shared_ptr<base_ast_node> val(new constant<unsigned int>(0));
  deriv_expr_.push(val);
}
template<>
void derivative_builder<state::process_expr>::Visit(constant<unsigned int>  &cnst) {
  std::shared_ptr<base_ast_node> val(new constant<unsigned int>(cnst));
  deriv_expr_.push(val);
}
template<>
void derivative_builder<state::process_power_expr>::Visit(constant<unsigned int>  &cnst) {
  std::shared_ptr<base_ast_node> val(new constant<unsigned int>(cnst.value() - 1));
  deriv_expr_.push(val);
}


template<>
void derivative_builder<state::general>::Visit(constant<double>  &cnst) {
  std::shared_ptr<base_ast_node> val(new constant<double>(0.0));
  deriv_expr_.push(val);
}
template<>
void derivative_builder<state::process_expr>::Visit(constant<double>  &cnst) {
  std::shared_ptr<base_ast_node> val(new constant<double>(cnst));
  deriv_expr_.push(val);
}
template<>
void derivative_builder<state::process_power_expr>::Visit(constant<double>  &cnst) {
  std::shared_ptr<base_ast_node> val(new constant<double>(cnst.value() - 1));
  deriv_expr_.push(val);
}

template<>
void derivative_builder<state::general>::Visit(identifier  &ident) {
  std::shared_ptr<base_ast_node> val(new constant<unsigned int>(1));
  deriv_expr_.push(val);
}
template<>
void derivative_builder<state::process_expr>::Visit(identifier  &ident) {
  std::shared_ptr<base_ast_node> val(new identifier(ident));
  deriv_expr_.push(val);
}
template<>
void derivative_builder<state::process_power_expr>::Visit(identifier  &ident) {
  //may be have to identify that we need log the full expression
  std::shared_ptr<base_ast_node> val(new identifier(ident));
  deriv_expr_.push(val);
}

template<>
void derivative_builder<state::general>::Visit(constant_variable  &var) {
  std::shared_ptr<base_ast_node> val(new constant<unsigned int>(0));
  deriv_expr_.push(val);
}
template<>
void derivative_builder<state::process_expr>::Visit(constant_variable  &var) {
  std::shared_ptr<base_ast_node>  val(new constant_variable(var));
  deriv_expr_.push(val);
}
template<>
void derivative_builder<state::process_power_expr>::Visit(constant_variable  &var) {
  std::shared_ptr<expr_node> cpy(new constant_variable(var));
  std::shared_ptr<expr_node> one(new constant<unsigned int>(1));
  std::shared_ptr<base_ast_node> minus1(new bin_op_node(cpy, bin_op_node::MINUS, one));
  deriv_expr_.push(minus1);
}

//TODO: need example for correction of implementation
template<state type>
void derivative_builder<type>::Visit(unary_op  &un) {
  un.expr()->Accept(*this);
}


template<state type>
void derivative_builder<type>::Visit(bin_op_node  &bin) {
  switch (bin.get_op()) {
    case bin_op_node::POWER:
    {
      //process exprssion already should simplify an expresion: for example x^(2+3) => 5x^4
      derivative_builder<state::process_expr> expon_expr_getter;
      expon_expr_getter.Visit(*bin.get_right());
      std::shared_ptr<base_ast_node> right_orig = expon_expr_getter.get_deriv_expr();

      derivative_builder<state::process_power_expr> expon_builder;
      expon_builder.Visit(*bin.get_right());
      std::shared_ptr<base_ast_node> right_deriv = expon_builder.get_deriv_expr();

      derivative_builder<state::process_expr> left_expr_getter;
      left_expr_getter.Visit(*bin.get_left());
      std::shared_ptr<base_ast_node> left_origin = left_expr_getter.get_deriv_expr();

      bin.get_left()->Accept(*this);
      std::shared_ptr<base_ast_node>  left_deriv = deriv_expr_.top();
      deriv_expr_.pop();
      std::shared_ptr<bin_op_node> expon_mul(new bin_op_node(std::static_pointer_cast<expr_node>(left_origin), bin_op_node::POWER, std::static_pointer_cast<expr_node>(right_deriv)));
      std::shared_ptr<bin_op_node> prefixed_expon_mul(new bin_op_node(std::static_pointer_cast<expr_node>(right_orig), bin_op_node::MUL, expon_mul));
      std::shared_ptr<base_ast_node> res(new bin_op_node(prefixed_expon_mul, bin_op_node::MUL, std::static_pointer_cast<expr_node>(left_deriv)));

      derivative_builder<state::process_expr> simplifier;
      simplifier.Visit(*res);
      deriv_expr_.push(simplifier.get_deriv_expr());
    }break;

    case bin_op_node::MUL:
    {
      //========== this is general code
      if (bin.get_left()->isConstant() && !bin.get_right()->isConstant()) {
        bin.get_right()->Accept(*this);
        std::shared_ptr<base_ast_node>  right_deriv = deriv_expr_.top();
        deriv_expr_.pop();
        //bin.get_left() must ve cloned
        std::shared_ptr<expr_node> res(new bin_op_node(bin.get_left(), bin_op_node::MUL, std::static_pointer_cast<expr_node>(right_deriv)));
        deriv_expr_.push(res);
        return;
      }

      if (bin.get_right()->isConstant() && !bin.get_left()->isConstant()) {
        bin.get_left()->Accept(*this);
        std::shared_ptr<base_ast_node> left_deriv = deriv_expr_.top();
        deriv_expr_.pop();
        std::shared_ptr<expr_node> res(new bin_op_node(std::static_pointer_cast<expr_node>(left_deriv), bin_op_node::MUL, bin.get_right()));
        deriv_expr_.push(res);
        return;
      }

      bin.get_left()->Accept(*this);
      std::shared_ptr<base_ast_node> left_deriv = deriv_expr_.top();
      deriv_expr_.pop();

      derivative_builder<state::process_expr> rexpr_builder;
      rexpr_builder.Visit(*bin.get_right());
      std::shared_ptr<base_ast_node> processed_right = rexpr_builder.get_deriv_expr();

      derivative_builder<state::process_expr> lexpr_builder;
      lexpr_builder.Visit(*bin.get_left());
      std::shared_ptr<base_ast_node> processed_left = lexpr_builder.get_deriv_expr();

      bin.get_right()->Accept(*this);
      std::shared_ptr<base_ast_node>  right_deriv = deriv_expr_.top();
      deriv_expr_.pop();

      std::shared_ptr<expr_node> mul_left(new bin_op_node(std::static_pointer_cast<expr_node>(left_deriv), bin_op_node::MUL, std::static_pointer_cast<expr_node>(processed_right)));
      std::shared_ptr<expr_node> mul_right(new bin_op_node(std::static_pointer_cast<expr_node>(processed_left), bin_op_node::MUL, std::static_pointer_cast<expr_node>(right_deriv)));
      //===========

      std::shared_ptr<expr_node> res(new bin_op_node(mul_left, bin_op_node::PLUS, mul_right));
      deriv_expr_.push(res);
    }break;

    case bin_op_node::DIV:
    {
      //========== this is general code
      bin.get_left()->Accept(*this);
      std::shared_ptr<base_ast_node> left_deriv = deriv_expr_.top();
      deriv_expr_.pop();

      derivative_builder<state::process_expr> rexpr_builder;
      rexpr_builder.Visit(*bin.get_right());
      std::shared_ptr<base_ast_node> processed_right = rexpr_builder.get_deriv_expr();

      derivative_builder<state::process_expr> lexpr_builder;
      lexpr_builder.Visit(*bin.get_left());
      std::shared_ptr<base_ast_node> processed_left = lexpr_builder.get_deriv_expr();

      bin.get_right()->Accept(*this);
      std::shared_ptr<base_ast_node> right_deriv = deriv_expr_.top();
      deriv_expr_.pop();

      std::shared_ptr<expr_node> mul_left(new bin_op_node(std::static_pointer_cast<expr_node>(left_deriv), bin_op_node::MUL, std::static_pointer_cast<expr_node>(processed_right)));
      std::shared_ptr<expr_node> mul_right(new bin_op_node(std::static_pointer_cast<expr_node>(processed_left), bin_op_node::MUL, std::static_pointer_cast<expr_node>(right_deriv)));
      //===========

      std::shared_ptr<expr_node> minus_expr(new bin_op_node(mul_left, bin_op_node::MINUS, mul_right));
      std::shared_ptr<expr_node> sqrtG(new bin_op_node(std::static_pointer_cast<expr_node>(processed_right), bin_op_node::POWER, std::make_shared<constant<unsigned int>>(2)));
      std::shared_ptr<expr_node> res(new bin_op_node(minus_expr, bin_op_node::DIV, sqrtG));
      deriv_expr_.push(res);
    }break;

    case bin_op_node::PLUS:
    case bin_op_node::MINUS:
    {
      bin.get_left()->Accept(*this);
      std::shared_ptr<expr_node> l = std::static_pointer_cast<expr_node>(deriv_expr_.top());
      deriv_expr_.pop();
      bin.get_right()->Accept(*this);
      std::shared_ptr<expr_node> r = std::static_pointer_cast<expr_node>(deriv_expr_.top());
      deriv_expr_.pop();
      std::shared_ptr<expr_node> res(new bin_op_node(l, bin.get_op(), r));
      derivative_builder<process_expr> simplifier;
      simplifier.Visit(*res);
      deriv_expr_.push(simplifier.get_deriv_expr());
    }break;
  }
}
template<>
void derivative_builder<state::process_expr>::Visit(bin_op_node  &bin) {
  //const & const
  if (bin.get_left()->isConstant() && bin.get_right()->isConstant()) {
    std::shared_ptr<constant_base>  &l = std::static_pointer_cast<constant_base>(bin.get_left());
    std::shared_ptr<constant_base>  &r = std::static_pointer_cast<constant_base>(bin.get_right());
    const_op_builder op;
    std::shared_ptr<expr_node> res =  op.getBy(std::make_tuple(l->type(), r->type(), bin.get_op()))->op(l, r);
    deriv_expr_.push(res);
    return;
  }

  if (bin.get_left()->isConstant()) {
    bin.get_right()->Accept(*this);
    std::shared_ptr<expr_node> r = std::static_pointer_cast<expr_node>(deriv_expr_.top());
    deriv_expr_.pop();

    std::shared_ptr<bin_op_node> nBin(new bin_op_node(bin.get_left(), bin.get_op(), r));
    reduced_map reduce;
    std::shared_ptr<expr_node>  reducedNode = reduce.reduceByOp(*nBin);
    if (reducedNode) {
      //may be have to clone here.
      deriv_expr_.push(reducedNode);
      return;
    }
  }

  if (bin.get_right()->isConstant()) {
    bin.get_left()->Accept(*this);
    std::shared_ptr<expr_node> l = std::static_pointer_cast<expr_node>(deriv_expr_.top());
    deriv_expr_.pop();

    std::shared_ptr<bin_op_node> nBin(new bin_op_node(l, bin.get_op(), bin.get_right()));
    reduced_map reduce;
    std::shared_ptr<expr_node>  reducedNode = reduce.reduceByOp(*nBin);
    if (reducedNode) {
      //may be have to clone here.
      deriv_expr_.push(reducedNode);
      return;
    }
  }

  //TODO: const & expr or expr & const with special cases of 0 and 1
  /*if (bin.get_left()->isConstant() || bin.get_right()->isConstant()) {
    reduced_map reduce;
    std::shared_ptr<expr_node>  reducedNode = reduce.reduceByOp(bin);
    if (reducedNode) {
      //may be have to clone here.
      deriv_expr_.push(reducedNode);
      return;
    }
  }*/

  //expr & expr = just clone.
  bin.get_left()->Accept(*this);
  std::shared_ptr<expr_node> l = std::static_pointer_cast<expr_node>(deriv_expr_.top());
  deriv_expr_.pop();
  bin.get_right()->Accept(*this);
  std::shared_ptr<expr_node> r = std::static_pointer_cast<expr_node>(deriv_expr_.top());
  deriv_expr_.pop();
  std::shared_ptr<expr_node> res(new bin_op_node(l, bin.get_op(), r));
  deriv_expr_.push(res);
}

template<state type>
void derivative_builder<type>::Visit(trigonometric_expr<TRIG_FUNC::COS>  &cos) {

}

template<state type>
void derivative_builder<type>::Visit(trigonometric_expr<TRIG_FUNC::SIN>  &sin) {

}

template<state type>
void derivative_builder<type>::Visit(trigonometric_expr<TRIG_FUNC::TAN>  &tan) {

}

template<state type>
std::shared_ptr<base_ast_node>  derivative_builder<type>::get_deriv_expr() const {
  if (deriv_expr_.empty() || deriv_expr_.size() > 1) {
    throw std::logic_error("something went wrong, stack should contain single elem");
  }
  return deriv_expr_.top();
}
