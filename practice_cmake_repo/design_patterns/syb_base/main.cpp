#include <iostream>
#include <list>
#include <numeric>
#include <functional>

//#include "auxiliary.h"

// Helper traits
// to determine function result and parameter types
// limited to traits required by the example

template <typename Fun_>
struct Result {
  typedef typename Fun_::Result Type;
};

template <typename Fun_>
struct Param1 {
  typedef typename Fun_::Param1 Type;
};

template <typename Result_, typename P1_, typename P2_>
struct Param1<Result_(*)(P1_, P2_)> {
  typedef P1_ Type;
};

// Implementation of gfold
// base case followed by standard list example
template <typename Fun_, typename InitFun_, typename Param_>
  typename Result<InitFun_>::Type
  gfold(Fun_ fun, InitFun_ initFun, Param_& param) {
    return initFun(param);
  }

template <typename Fun_, typename InitFun_, typename T>
typename Result<InitFun_>::Type
gfold(Fun_ fun, InitFun_ initFun, std::list<T>& param) {
  return std::accumulate<
    typename std::list<T>::iterator,
    typename Result<InitFun_>::Type>(
    param.begin(), param.end(), param, fun);
}

// Implementation of gmapT
// including required helper function objects
template <typename Fun_, typename Param1_>
struct ApplyToSecond {
  typedef Param1_ Result;
  typedef Param1_ Param1;
  Fun_ fun;
  ApplyToSecond(Fun_ fun) : fun(fun) {}
  template <typename Param2_>
  Param1_ operator()(Param1_ p1, Param2_& p2) {
    fun(p2);
    return p1;
  }
};

//std::bind
template <typename Bound_>
struct Bind {
  typedef Bound_ Result;
  Bound_ bound;
  Bind(Bound_ bound) : bound(bound) {}
  template <typename Param_>
  Result operator()(Param_&) {
    return bound;
  }
};


template <typename Fun_, typename Param_>
Param_& gmapT(Fun_ fun, Param_& param) {
  return gfold(ApplyToSecond<Fun_, Param_&>(fun),
    Bind<Param_&>(param),
    param);
}

// Implementation of everywhere
// function object followed by convenience wrapper
template <typename Fun_>
struct Everywhere {
  Fun_ fun;
  Everywhere(Fun_ fun) : fun(fun) {}
  template <typename Param_>
  Param_& operator()(Param_& param) {
    return fun(gmapT(*this, param));
  }
};

template <typename Fun_, typename Param_>
Param_& everywhere(Fun_ fun, Param_& param) {
  return (Everywhere<Fun_>(fun))(param);
}


// Implementation of MkT
// function object followed by convenience wrapper
template <typename Fun_>
struct MkT {
  Fun_ fun_;
  MkT(Fun_ fun) : fun_(fun) {}
  template <typename Param_>
  Param_& operator()(Param_& param) {
    return param;
  }
  typename Param1<Fun_>::Type
    operator()(typename Param1<Fun_>::Type param) {
      return fun_(param);
    }
};

template <typename Fun_>
MkT<Fun_> mkT(Fun_ fun)
{
  return MkT<Fun_>(fun);
}

// Implementation of explicit data-types
// Class At for accessing the N:th element of a map
// The Empty base case and the Insert cons constructor
template <unsigned int N_, class Map_> class At;

struct Empty {
  template <typename Fun_, typename Init_>
  typename Result<Fun_>::Type
    gfold_(Fun_ fun, Init_& init) { return init; }
};

template <typename Data_, class Base_>
class Insert : public Base_ {
  Data_ data_;
public:
  Insert(Data_ data) : Base_(), data_(data) {}
  template <typename D1_, typename D2_>
  Insert(D1_ d1, D2_ d2)
    : Base_(d2), data_(d1) {}
  template <typename D1_, typename D2_, typename D3_>
  Insert(D1_ d1, D2_ d2, D3_ d3)
    : Base_(d2, d3), data_(d1) {}
protected:
  template <typename Fun_, typename Init_>
  typename Result<Fun_>::Type
    gfold_(Fun_ fun, Init_& init) {
      return Base_::gfold_(fun, fun(init, data_));
    }
public:
  template <typename Fun_, typename InitFun_>
  friend typename Result<Fun_>::Type
    gfold(Fun_ fun, InitFun_ initFun, Insert& param) {
      return param.gfold_(fun, initFun(param));
    }
  friend typename At<1, Insert>::Result
    At<1, Insert>::at(Insert& i);
};

template <unsigned int N_, class Data_, class Base_>
struct At <N_, Insert<Data_, Base_> >
  : At<N_ - 1, Base_>{};
template <class Data_, class Base_>
struct At <1, Insert<Data_, Base_> > {
  typedef Data_ & Result;
  static Result at(Insert<Data_, Base_> & i) {
    return i.data_;
  }
};

template <unsigned int N_, class Map_>
typename At<N_, Map_>::Result at(Map_& m) {
  return At<N_, Map_>::at(m);
}

// The C++ type hierarchy
// DeptUnit omitted for simplicity
typedef const char *Name, *Address;
typedef Insert<float, Empty> Salary;
typedef Insert<Name, Insert<Address, Empty> > Person;
typedef Insert<Person, Insert<Salary, Empty> >
Employee, Manager;
typedef Insert<Employee, Empty> PersonUnit, SubUnit;
typedef Insert<Name, Insert<Manager,
  Insert<std::list<SubUnit>, Empty> > > Dept;
typedef Insert<std::list<Dept>, Empty> Company;


// Implementation of currying in C++
template <typename Fun_>
struct Curry;
template <typename Result_, typename P1_, typename P2_>
struct Curry<Result_(*)(P1_, P2_)> {
  typedef Result_ Result;
  typedef P2_ Param1;
  Result_(*fun)(P1_, P2_);
  P1_ value;
  Curry(Result_(*fun)(P1_, P2_), P1_ value)
    : fun(fun), value(value) {}
  Result operator()(Param1 param1) const {
    return fun(value, param1);
  }
};

// Implementation of incS
// binary function followed by curried unary function
Salary& incS(float k, Salary& salary) {
  at<1>(salary) *= 1 + k;
  return salary;
}
Curry<Salary& (*)(float, Salary&)> incS(float k) {
  return Curry<Salary& (*)(float, Salary&)>(incS, k);
}
// Implementation of increase algorithm
Company& increase(float k, Company& company) {
  return everywhere(mkT(incS(k)), company);
}

// Implementation of main program
// helper function for list insertion followed by
// a main procedure constructing the "paradise"
// example hierarchy and printing Blair’s salary
// after a 20% salary increase
template <typename T>
std::list<T> append(T t, std::list<T> list) {
  list.push_back(t);
  return list;
}

int main() {

  Company company(
    append(Dept("Research", Employee(Person("Ralf", "Amsterdam"), Salary(8000)),
    append(PersonUnit(Employee(Person("Joost", "Amsterdam"), Salary(1000))),
    append(PersonUnit(Employee(Person("Marlow", "Cambridge"), Salary(2000))),
    std::list<SubUnit>()))),/*
    append(Dept("Strategy", Employee(Person("Blair", "London"), Salary(100000)),
    std::list<SubUnit>()),*/
    std::list<Dept>())/*)*/);

  increase(0.20f, company);
  std::cout << at<1>(at<2>(at<2>(at<1>(company).front()))) << std::endl;
  std::cout << at<1>(at<1>(company).front()) << std::endl;
}
