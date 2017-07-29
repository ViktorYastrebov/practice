//#include <type_traits>
#include <string>
#include <iostream>

#include <unordered_map>
#include <memory>

#include "funcions.hpp"
//#include "structs.hpp"
#include "hash_template.hpp"
#include "container.hpp"


//http://stackoverflow.com/questions/5099429/how-to-use-stdtuple-types-with-boostmpl-algorithms
//http://stackoverflow.com/questions/12653407/runtime-value-to-type-mapping

#include <boost/mpl/map.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/for_each.hpp>

template<class T, int type_id>
struct type_map_builder {
  typedef T type;
  static const int typeId = type_id;
  static constexpr int getTypeId() {
    return typeId;
  }

  T create() {
    return T();
  }
};

//problem here. create should be instantinated as specific type or cast it to something general
//if try to using type_instance std::unordered_map it needs general type.
//the point is: runtime switching can't be done at compile time!!! even if try to force cast

//otherwise - have to implement all basic types as classes with all operations.

//anothere way of solving this problem: specify data types and functoins which needs to be performed.
// SYB
//http://stackoverflow.com/questions/26075969/compile-time-map-and-inverse-map-values

struct base_type {
  virtual void getAsBytes(unsigned char const  *data, std::size_t size) = 0;
  virtual ~base_type() {};
};

template<class T>
struct type_intance : public base_type {
  typedef T type;
  virtual void getAsBytes(unsigned char const  *data, std::size_t size) {
    T def_value = T();
    std::memcpy((void*)data, static_cast<const void*>(&def_value), size);
  }
};

enum TYPES_ID {
  CHAR_TYPE = 1,
  UCHAR_TYPE,
  SHORT_TYPE,
  USHORT_TYPE,
  INT_TYPE,
  UNSIGNED_INT
};

template<class T>
std::unique_ptr<base_type> make_type() {
  return std::unique_ptr<base_type>(new type_intance<T>());
}

void type_mapping_issue() {
  typedef std::unordered_map<TYPES_ID, std::unique_ptr<base_type>> types_mapping;
  std::unordered_map<TYPES_ID, std::unique_ptr<base_type>> type_map;
  type_map.insert(std::make_pair(TYPES_ID::CHAR_TYPE, make_type<char>()));
  type_map.insert(std::make_pair(TYPES_ID::UCHAR_TYPE, make_type<unsigned char>()));
  type_map.insert(std::make_pair(TYPES_ID::SHORT_TYPE, make_type<short>()));
  type_map.insert(std::make_pair(TYPES_ID::USHORT_TYPE, make_type<unsigned short>()));


  const std::size_t short_rt_legnth = sizeof(short);
  unsigned char short_rt[sizeof(short)];
  TYPES_ID rt_check = TYPES_ID::SHORT_TYPE;
  types_mapping::const_iterator it = type_map.find(rt_check);
  if (it != type_map.end()) {
    it->second->getAsBytes(short_rt, short_rt_legnth);
    std::cout << short_rt << std::endl;
  }

  //type_map_builder<int, 1> intType;
  //boost::mpl::vector<type_map_builder<int, 1>, type_map

  //std::cout << "type_id = " << intType.getTypeId() << "default ctor of type  = " << intType.create() << std::endl;
  //std::unordered_map<TYPES_ID, std::unique_ptr<base_type>>::const_iterator it =  type_map.find(TYPES_ID::SHORT_TYPE);
  //std::cout << "type_id = " << it->first << "value = " << (*it->second);
}


//intantinate the class by list types;


struct compound {};
struct generic_type
{
  template<class T>
  int operator ()(T value) {
    std::cout << typeid(value).name() << std::endl;
    return 0;
  }
};

struct initializer {
  typedef boost::mpl::list<int, char, double, compound> type_list;
  static  int test;
};

int initializer::test = [] { boost::mpl::for_each<type_list>(generic_type()); return 0; }();


union test {
  double a;
  int c;
};


/*
#include <typeindex>
#include <vector>
#include <iostream>

std::vector<std::type_index> vec;

template<typename T>
void Fill() {
vec.push_back(typeid(T));
}

template <typename T1, typename T2, typename... Tn>
void Fill() {
Fill<T1>();
Fill<T2, Tn...>();
}

int main()
{
Fill<int,double>();
std::cout << vec.size() << std::endl;
}
*/



class A {
public:
  virtual ~A() {}
  virtual void print() const = 0;
};

template<class T>
class B : public A {
public:
  B()
    :mem_(T())
  {}

  virtual ~B() {}
  virtual void print() const {
    std::cout << "def:" << mem_ << std::endl;
  }
private:
  T mem_;
};


template<class T, class U>
auto my_div(const T  &t, const U  &u) -> decltype(t + u) {
  return t / u;
}

//mapping diff types by id

void type_map_by_id() {
#if 0
  using map_t = std::unordered_map<std::tuple<bool, int, double>, int, hash_tuple2<bool, int, double>>;
  map_t map;
  map.insert(std::make_pair(std::make_tuple<bool, int, double>(true, 1, 3.0), 50));
  map.insert(std::make_pair(std::make_tuple<bool, int, double>(false, 2, 5.0), 100));


  bool val1 = true;
  int val2 = 0;
  double val3 = 0.0;

  std::cin >> std::boolalpha >> val1;
  std::cout << "val1 : " << val1 << std::endl;
  std::cin >> val2;
  std::cout << "val2 : " << val2 << std::endl;
  std::cin >> val3;
  std::cout << "val3 : " << val3 << std::endl;

  map_t::const_iterator it = map.find(std::make_tuple(val1, val2, val3));
  if (it != map.end()) {
    std::cout << it->second << std::endl;
  }
#endif

  using tuple_idx_type_t = std::tuple<std::type_index, std::type_index>;
  using map2_t = std::unordered_map<tuple_idx_type_t, std::shared_ptr<A>, hash_tuple2<std::type_index, std::type_index>>;
  //map2_t map2;

  //map2.insert(std::make_pair(std::make_tuple(std::type_index(typeid(double)), std::type_index(typeid(unsigned int))), 1));
  //map2_t map2;
  map2_t map2 = {
    std::make_pair(std::make_tuple<std::type_index, std::type_index>(std::type_index(typeid(double)), std::type_index(typeid(unsigned int))), std::shared_ptr<A>(new B<int>)),
    std::make_pair(std::make_tuple<std::type_index, std::type_index>(std::type_index(typeid(unsigned int)), std::type_index(typeid(unsigned int))), std::shared_ptr<A>(new B<float>)),
    std::make_pair(std::make_tuple<std::type_index, std::type_index>(std::type_index(typeid(unsigned int)), std::type_index(typeid(double))), std::shared_ptr<A>(new B<double>)),
    std::make_pair(std::make_tuple<std::type_index, std::type_index>(std::type_index(typeid(double)), std::type_index(typeid(double))), std::shared_ptr<A>(new B<unsigned int>))
  };

  map2_t::const_iterator it = map2.find(std::make_tuple(std::type_index(typeid(unsigned int)), std::type_index(typeid(double))));
  if (it != map2.end()) {
    it->second->print();
  }

  std::cout << my_div(10, 2.3) << std::endl;
}



//map function templates 
struct simple_base_type {};

template<typename T> 
struct simple_type_wrapper : simple_base_type {
  simple_type_wrapper()
    :value(T())
  {}

  simple_type_wrapper(const T  &v)
    :value(v)
  {}

  T value;
};


template<class T>
struct equal {
  bool operator()(const std::shared_ptr<simple_base_type>  &l, const std::shared_ptr<simple_base_type>  &r) const {
    return std::static_pointer_cast<simple_type_wrapper<T>>(l)->value == std::static_pointer_cast<simple_type_wrapper<T>>(r)->value;
  }
};

struct mapped_type {
  using map_t = std::unordered_map<int, std::function<bool(const std::shared_ptr<simple_base_type>&, const std::shared_ptr<simple_base_type>&)>>;
  mapped_type() {
    //std::function<bool(const std::shared_ptr<base_type>&, const std::shared_ptr<base_type>&)> f = ;
    map_ = {
      std::make_pair(1, equal<double>()),
      std::make_pair(2, equal<int>())
    };
  }

  bool Do(int type, const std::shared_ptr<simple_base_type>  &l, const std::shared_ptr<simple_base_type>  &r) {
    map_t::const_iterator it = map_.find(type);
    if (it != map_.end()) {
      return it->second(l, r);
    }
  }

  map_t map_;
};


void func_mapping() {
  mapped_type tm;
  std::shared_ptr<simple_base_type> l(new simple_type_wrapper<double>(1));
  std::shared_ptr<simple_base_type> r(new simple_type_wrapper<double>(2));
  std::cout << std::boolalpha << tm.Do(1, l, r) << std::endl;
  std::cout << std::boolalpha << tm.Do(1, r, r) << std::endl;
}

void poly_container_test() {
  poly_objects<int, std::vector> cont = { 1, 2, 3 };
  poly_objects<int, std::list> cont2 = { 1,2,3,4 };
  poly_objects<std::shared_ptr<int>> con3;
  poly_objects_ptrs<int> con_ptrs;


  poly_object_wrapper<int, std::list> con_ptr2;
  con_ptr2.data_.push_back(std::make_shared<int>(1));

  /*
  poly_object_wrapper<int, std::list, ByValue> by_value;
  by_value.data_.push_back(1);
  */

  cont.execute();
  cont2.execute();
}

auto main(int argc, char*argv[]) -> int {
  //test t;
  //t.a;

  //func_mapping();

  //test_base();

  poly_container_test();
  return 0;
}