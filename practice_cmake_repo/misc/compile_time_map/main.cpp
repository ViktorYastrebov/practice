#include "generic_map.hpp"
#include <typeinfo>

#include <iostream>

//  http://coliru.stacked-crooked.com/a/99ccb93ec853cc54
//  http://baptiste-wicht.com/posts/2015/07/simulate-static_if-with-c11c14.html
//  Traveral compiletime list
//  http://playfulprogramming.blogspot.com/2011/09/compile-time-quick-sort-using-c.html
//  Try to look this one: https://github.com/sbabbi/tinympl
//  Quite cool: https://marcoarena.wordpress.com/tag/template-metaprogramming/
//  http://codereview.stackexchange.com/questions/44546/very-basic-tuple-implementation
//  http://stackoverflow.com/questions/7661643/how-to-detect-the-first-and-the-last-argument-in-the-variadic-templates

auto main() -> int {


#ifdef ENUM_IN_ENTRY

  enum BuildInTypes {
    INT,
    DOUBLE,
    FLOAT,
    CHAR,
    UNSGINED_CHAR,
    SHORT
  };

  typedef map<
    entry<BuildInTypes, BuildInTypes::INT, int>,
    entry<BuildInTypes, BuildInTypes::DOUBLE, double>,
    entry<BuildInTypes, BuildInTypes::FLOAT, float>,
    entry<BuildInTypes, BuildInTypes::CHAR, char>
  > map_t;

  typedef forward_type<char, double, int>::type first_Type;
  forward_type<map_t>::type::head_type::value_type obj;
  std::cout << typeid(obj).name() << std::endl;

  at<map_t, BuildInTypes::INT>::result_type typeObj1;
  std::cout << "type1Obj : " << typeid(typeObj1).name() << std::endl;
  
  at<map_t, BuildInTypes::DOUBLE>::result_type typeObj2;
  std::cout << "type1Obj : " << typeid(typeObj2).name() << std::endl;

  at<map_t, BuildInTypes::FLOAT>::result_type typeObj3;
  std::cout << "type1Obj : " << typeid(typeObj3).name() << std::endl;

  at<map_t, BuildInTypes::CHAR>::result_type typeObj4;
  std::cout << "type1Obj : " << typeid(typeObj4).name() << std::endl;

#endif

#ifdef EPLICIT_ENUM_TYPE
  typedef map<
    entry<BuildInTypes::INT, int>,
    entry<BuildInTypes::DOUBLE, double>,
    entry<BuildInTypes::FLOAT, float>,
    entry<BuildInTypes::CHAR, char>
  > map_t;

  at<map_t, BuildInTypes::INT>::result_type typeObj1;
  std::cout << "type1Obj : " << typeid(typeObj1).name() << std::endl;

    at<map_t, BuildInTypes::DOUBLE>::result_type typeObj2;
  std::cout << "type1Obj : " << typeid(typeObj2).name() << std::endl;
  
  at<map_t, BuildInTypes::FLOAT>::result_type typeObj3;
  std::cout << "type1Obj : " << typeid(typeObj3).name() << std::endl;

  at<map_t, BuildInTypes::CHAR>::result_type typeObj4;
  std::cout << "type1Obj : " << typeid(typeObj4).name() << std::endl;

#endif
  return 0;
}