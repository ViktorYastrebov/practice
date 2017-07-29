#pragma once

#include <cstddef> //std::nullptr_t

#define ENUM_IN_ENTRY

#ifdef ENUM_IN_ENTRY
template<class EnumType, EnumType id, class T>
struct entry {
  static constexpr EnumType key_value = id;
  using value_type = T;
  using enum_type = EnumType;
};

template<class ...Types>
struct map;

template<class Head, class ...Tail>
struct map<Head, Tail...> {
  using head_type = Head;
  using tail_list = map<Tail...>;
};

template<class Last>
struct map<Last> {
  using head_type = Last;
  using tail_list = map<>;
};

template <class T1, class ...T>
struct forward_type
{
  using type = T1;
};

template<class EnumType, EnumType id, bool equal, class ...Types>
struct at_impl_;

//case: traversal type list(Head:Tail)
template<class EnumType, EnumType id, class Head, class ...Types>
struct  at_impl_<EnumType,id, false, map<Head,Types...>>
{
  static constexpr bool eq_next = (typename forward_type<Types...>::type::key_value == id);
  using result_type = typename at_impl_<EnumType, id, eq_next, map<Types...>>::result_type;
};

//case: type is found
template<class EnumType, EnumType id, class Head, class ...Types>
struct  at_impl_<EnumType, id, true, map<Head, Types...>>
{
  using result_type = typename Head::value_type;
};

//case: list is empty
template<class EnumType, EnumType id, bool equal>
struct at_impl_<EnumType, id, equal, map<EnumType>>
{
  using result_type = nullptr_t;
};

template<class TypeMap, typename TypeMap::head_type::enum_type id>
struct at {
  static constexpr bool first_eq = (typename TypeMap::head_type::key_value == id);
  using result_type = typename at_impl_<typename TypeMap::head_type::enum_type, id, first_eq, TypeMap>::result_type;
};

#endif

#ifdef EPLICIT_ENUM_TYPE
enum BuildInTypes {
  INT,
  DOUBLE,
  FLOAT,
  CHAR,
  UNSGINED_CHAR,
  SHORT
};

template<BuildInTypes id, class T>
struct entry {
  static constexpr BuildInTypes key_value = id;
  using value_type = T;
};

template<class ...Types>
struct map;

template<class Head, class ...Tail>
struct map<Head, Tail...> {
  using head_type = Head;
  using tail_list = map<Tail...>;
};

template<class Last>
struct map<Last> {
    using head_type = Last;
  using tail_list = map<>;
};

template <class T1, class ...T>
struct forward_type
{
  using type = T1;
};

template<BuildInTypes id, bool equal, class ...Types>
struct at_impl_;

//case: traversal type list(Head:Tail)
template<BuildInTypes id, class Head, class ...Types>
struct  at_impl_<id, false, map<Head, Types...>>
{
  static constexpr bool eq_next = (typename forward_type<Types...>::type::key_value == id);
  using result_type = typename at_impl_<id, eq_next, map<Types...>>::result_type;
};

//case: type is found
template<BuildInTypes id, class Head, class ...Types>
struct  at_impl_<id, true, map<Head, Types...>>
{
  using result_type = typename Head::value_type;
};

//case: list is empty
template<BuildInTypes id, bool equal>
struct at_impl_<id, equal, map<>>
{
  using result_type = nullptr_t;
};

template<class TypeMap, BuildInTypes id>
struct at {
  static constexpr bool first_eq = (typename TypeMap::head_type::key_value == id);
  using result_type = typename at_impl_<id, first_eq, TypeMap>::result_type;
};

#endif