#include <iostream>
#include <string>
#include <algorithm>

#include <memory>

//#include "currying.hpp"


#include <cassert>
#include <tuple>
#include "capture.hpp"

template < typename T >
auto foo(T&& t)
{
  return[t = capture<T>(t)]() -> decltype(auto)
  {
    auto&& x = t.get();
    return std::forward<decltype(x)>(x);
    // or simply, return t.get();
  };
}

template < std::size_t... I, typename... T >
auto bar_impl(std::index_sequence<I...>, T&&... t)
{
  static_assert(std::is_same<std::index_sequence<I...>, std::index_sequence_for<T...>>::value, "");
  return[t = std::make_tuple(capture<T>(t)...)]()
  {
    return std::forward_as_tuple(std::get<I>(t).get()...);
  };
}
template < typename... T >
auto bar(T&&... t)
{
  return bar_impl(std::index_sequence_for<T...>{}, std::forward<T>(t)...);
}
//=================================

/*
void print3(std::string a, std::string b, std::string c)
{
  std::cout << a << b << c;
}

template<class T>
T sum(const T  &a, const T&b, const T &c) {
  return a + b + c;
}

#include <initializer_list>
#include <list>
#include <numeric>

struct define {};
struct bind {};

struct test_forward
{
  test_forward(int &&a, std::string &&b, double &&c)
    : val1_(std::move(a)), val2_(std::move(b)), val3_(std::move(c))
  {}

  int val1_;
  std::string val2_;
  double val3_;
};



auto to_string(const std::list<std::tuple<std::string, bind>>  &selectList, const std::string  &from, const std::list<std::tuple<std::string, define>>  &where_clause) {
  //std::string to_string(const std::list<std::tuple<std::string, bind>>  &selectList, const std::string  &from, const std::list<std::tuple<std::string, define>>  &where_clause) {
  std::string ret("SELECT ");
  std::for_each(selectList.begin(), selectList.end(),
    [&](const std::tuple<std::string, bind>  &tuple)
  {
    ret += std::get<0>(tuple) + " ";

  }
  );
  ret += "FROM " + from + " WHERE ";
  int idx = 1;
  std::for_each(where_clause.begin(), where_clause.end(),
    [&](const std::tuple<std::string, define>  &tuple)
  {
    ret += std::get<0>(tuple) + " :" + std::to_string(idx++);
  });
  return ret;
}

struct SelectBuilder {
  template<typename Function, typename ... Args>
  auto create(Function fun, Args ... args) {
    return [=](auto ... rest) {
      return (*this.*fun)(args ..., rest ...);
    };
  }

  std::string to_string(
    const std::list<std::tuple<std::string, bind>>  &selectList,
    const std::string  &from,
    const std::list<std::tuple<std::string, define>>  &where_clause)
  {
    std::string ret("SELECT ");
    std::for_each(selectList.begin(), selectList.end(),
      [&](const std::tuple<std::string, bind>  &tuple)
    {
      ret += std::get<0>(tuple) + " ";

    }
    );
    ret += "FROM " + from + "WHERE ";
    int idx = 1;
    std::for_each(where_clause.begin(), where_clause.end(),
      [&](const std::tuple<std::string, define>  &tuple)
    {
      ret += std::get<0>(tuple) + " :" + std::to_string(idx++);
    });
    return ret;
  }
  std::list<define> selected_list_;
  std::list<bind> where_clause_;
};
*/

void get_pointer(int &&a) {
  const int *ptr = &a;
  std::cout << ptr << std::endl;
}

class test {
public:
  test()
    :value(0) {
    std::cout << "ctor " << value << std::endl;
  }
  test(int a)
    :value(a)  {
    std::cout << "ctor " << value <<std::endl;
  }
  ~test() {
    std::cout << "dtor " << value <<std::endl;
  }
  test(const test & test) {
    std::cout << "copy " << value << std::endl;
  }
  test & operator = (const test  &a) {
    value = a.value;
    std::cout << "assign " << value << std::endl;
    return *this;
  }

  test(test &&t)  {
    std::cout << "prev value =" << value;
    value = std::move(t.value);
    std::cout << ", cur value = "<< value << std::endl;
  }

  void print() const {
    std::cout << "value =" << value << std::endl;
  }
private:
  int value;
};

test f(const test  &t) {
  test nt(2);
  return std::move(nt);
}

std::string another_test() {
  return std::move(std::string("something"));
}

#include <vector>
void vect_ref(std::vector<int>  &&r) {}

template<class T>
void  vect_ref2(T&&r) {
  std::cout << typeid(r).name() << std::endl;
}

int main() {
  test t(1);
  test &&t2 = f(t);
  t2.print();
  get_pointer(10);

  std::string  &&ur = another_test();
  
  {
    std::vector<int> v;
    std::vector<int>  &refV = v;
    //vect_ref(v); error
    //vect_ref(refV);
    vect_ref2(refV);
  }

/*
  std::list<std::tuple<std::string, bind>> selList = { std::make_tuple(std::string("col_id"), bind()) };
  std::list<std::tuple<std::string, define>> where = { std::make_tuple(std::string("col_int <> "), define()) };
  std::string from("ORACLE_TEST");

  SelectBuilder builder;
  std::function<std::string(const std::list<std::tuple<std::string, bind>>  &selectList,
    const std::string  &from,
    const std::list<std::tuple<std::string, define>>  &where_clause)> func = std::bind(&SelectBuilder::to_string, &builder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
  func(selList, from, where);

  std::function<std::string(const std::list<std::tuple<std::string, bind>>  &selectList,
    const std::string  &from)> func2 = std::bind(&SelectBuilder::to_string, &builder, std::placeholders::_1, std::placeholders::_2, where);
  func2(selList, from);

  std::function<std::string(const std::list<std::tuple<std::string, bind>>  &selectList)> func3 = std::bind(&SelectBuilder::to_string, &builder, std::placeholders::_1, from, where);
  func3(selList);

  std::function<std::string()> func4 = std::bind(&SelectBuilder::to_string, &builder, selList, from, where);
  func4();

  auto binSum = [](auto a, auto b) { return a + b; };
  auto addnext = currying(binSum, 1);
  auto res = addnext(4);
  std::cout << res << std::endl;

  auto lsum = [](auto a, auto b) { return a + b; };
  auto f = [=](auto func, auto a) {
    return [=](auto b) { return func(a, b);  };
  };
  std::cout << f(lsum, 1)(2) << std::endl;
  
  currying_std::curry(print3)("Hello ")("functional ")("world!");
  std::cout << currying_std::curry(sum<int>)(1)(2)(3) << std::endl;

  //auto unique_func = make_unique_my<test_forward>(10, std::string("hello"), 3.0);
*/

  /*
  static_assert(std::is_same<decltype(foo(0)()), int&&>::value, "");
  assert(foo(0)() == 0);

  auto i = 0;
  static_assert(std::is_same<decltype(foo(i)()), int&>::value, "");
  assert(&foo(i)() == &i);

  const auto j = 0;
  static_assert(std::is_same<decltype(foo(j)()), const int&>::value, "");
  assert(&foo(j)() == &j);

  const auto&& k = 0;
  static_assert(std::is_same<decltype(foo(std::move(k))()), const int&&>::value, "");
  assert(foo(std::move(k))() == k);
  */

  //auto t = bar(capture<int>(0), capture<int>(i), capture<const int>(j), capture(std::move(k)))();
/*
  static_assert(std::is_same<decltype(t), std::tuple<int&&, int&, const int&, const int&&>>::value, "");
  assert(std::get<0>(t) == 0);
  assert(&std::get<1>(t) == &i);
  assert(&std::get<2>(t) == &j);
  assert(std::get<3>(t) == k && &std::get<3>(t) != &k);
*/

  return 0;
}
