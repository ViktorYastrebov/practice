#pragma once

#include <tuple>
#include <cstring>
#include <functional>
#include <typeindex>

//http://stackoverflow.com/questions/20834838/using-tuple-in-unordered-map
//http://stackoverflow.com/questions/23429512/how-to-fill-a-vector-with-typeids-from-variadic-template-arguments

size_t hash_combiner(size_t left, size_t right) {
  return left^right;
}

template<int index, class ... types>
struct hash_impl {
  size_t operator()(size_t a, const std::tuple<types ...>  &t) {
    //typedef typename std::tuple_element<index, std::tuple<types...>>::type nexttype;
    hash_impl<index - 1, types...> next;
    //size_t b = std::hash<nexttype>()(std::get<index>(t));
    size_t b = typename std::get<index>(t).hash_code();
    return next(hash_combiner(a, b), t);
  }
};
template<class...types>
struct hash_impl<0, types...> {
  size_t operator()(size_t a, const std::tuple<types...>& t) const {
    //typedef typename std::tuple_element<0, std::tuple<types...>>::type nexttype;
    //size_t b = std::hash<nexttype>()(std::get<0>(t));
    size_t b = typename std::get<0>(t).hash_code();
    return hash_combiner(a, b);
  }
};

template<class...types>
struct hash_tuple2 {
  size_t operator()(const std::tuple<types...>& t) const {
    const size_t begin = std::tuple_size<std::tuple<types...>>::value - 1;
    return hash_impl<begin, types...>()(1, t); //1 should be some larger value
  }
};

/*
struct key_hash {
size_t operator()(const std::tuple<bool, bool>  &key) {
return std::get<0>(key) ^ std::get<1>(key);
}
};
std::unordered_map<std::tuple<bool, bool>, int, key_hash> map;
*/

template<typename idx>
void print_hash(const idx  &h) {
  std::cout << typename h.hash_code() << std::endl;
}

template<typename tuple>
void print0_hash(const tuple  &h) {
  std::cout << typename std::get<0>(h).hash_code() << std::endl;
}

template<typename ... types>
void print1_hash(const std::tuple<types...>  &t) {
  std::cout << typename std::get<0>(t).hash_code() << std::endl;
}

void templates_test() {
  using tuple_idx_type_t = std::tuple<std::type_index, std::type_index>;
  tuple_idx_type_t simple_tuple = { typeid(double), typeid(int) };
  std::cout << std::get<0>(simple_tuple).hash_code() << std::endl;


  print_hash<std::type_index>(std::get<0>(simple_tuple));
  print0_hash<tuple_idx_type_t>(simple_tuple);
  print1_hash<std::type_index, std::type_index>(simple_tuple);
}