#include <tuple>
#include <cstring>
#include <functional>
#include <type_traits>
#include <typeindex>

//http://stackoverflow.com/questions/23429512/how-to-fill-a-vector-with-typeids-from-variadic-template-arguments
#ifdef ORIGIN_HASH_IMPL

size_t hash_combiner(size_t left, size_t right) //replacable
{
  return left^right;
}

template<int index, class...types>
struct hash_impl {
  size_t operator()(size_t a, const std::tuple<types...>& t) const {
    typedef typename std::tuple_element<index, std::tuple<types...>>::type nexttype;
    hash_impl<index - 1, types...> next;
    size_t b = std::hash<nexttype>()(std::get<index>(t));
    return next(hash_combiner(a, b), t);
  }
};
template<class...types>
struct hash_impl<0, types...> {
  size_t operator()(size_t a, const std::tuple<types...>& t) const {
    typedef typename std::tuple_element<0, std::tuple<types...>>::type nexttype;
    size_t b = std::hash<nexttype>()(std::get<0>(t));
    return hash_combiner(a, b);
  }
};

namespace std {
  template<class...types>
  struct hash_tuple<std::tuple<types...>> {
    size_t operator()(const std::tuple<types...>& t) {
      const size_t begin = std::tuple_size<std::tuple<types...>>::value - 1;
      return hash_impl<begin, types...>()(1, t); //1 should be some larger value
    }
  };
}
#else

size_t hash_combiner(size_t left, size_t right) {
  return left^right;
}

template<class T>
size_t type_switched(const T  &v) {
  return std::hash<T>()(v);
}

template<>
size_t type_switched<std::type_index>(const std::type_index  &v) {
  return v.hash_code();
}

template<int index, class ... types>
struct hash_impl {
  size_t operator()(size_t a, const std::tuple<types ...>  &t) {
    typedef typename std::tuple_element<index, std::tuple<types...>>::type next_type;
    size_t b = type_switched<next_type>(std::get<index>(t));
    hash_impl<index - 1, types...> next;
    return next(hash_combiner(a, b), t);
  }
};
template<class...types>
struct hash_impl<0, types...> {
  size_t operator()(size_t a, const std::tuple<types...>& t) const {
    typedef typename std::tuple_element<0, std::tuple<types...>>::type next_type;
    size_t b = type_switched<next_type>(std::get<0>(t));
    return hash_combiner(a, b);
  }
};

template<class...types>
struct tuple_hash {
  size_t operator()(const std::tuple<types...>& t) const {
    const size_t begin = std::tuple_size<std::tuple<types...>>::value - 1;
    return hash_impl<begin, types...>()(1, t); //1 should be some larger value
  }
};

#endif