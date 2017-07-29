#pragma once

#include <memory>
#include <list>
#include <vector>
#include <algorithm>
#include <initializer_list>

#include <iostream>
#include <type_traits>

template<typename Item, template<typename ...> class Container = std::list>
class poly_objects {
public:

  poly_objects()
  {}

  poly_objects(std::initializer_list<Item> init)
    :conts_(init)
  {}

  using cont_t = Container<Item>;
  cont_t  &get();
  const cont_t  &get() const;

  void execute();

private:
  cont_t  conts_;
};


template<typename Item, template<typename ...> class Container>
typename poly_objects<Item, Container>::cont_t &poly_objects<Item, Container>::get() {
  return conts_;
}

template<typename Item, template<typename ...> class Container>
typename const poly_objects<Item, Container>::cont_t &poly_objects<Item , Container>::get() const {
  return conts_;
}

template<typename Item, template<typename ...> class Container>
void poly_objects<Item, Container>::execute() {
  std::for_each(conts_.begin(), conts_.end(), [&](typename const cont_t::value_type  &val)
  {
    std::cout << val << std::endl;
  });
}



template<typename Item, template<typename ...> class Container = std::list>
class poly_objects_ptrs {
public:
  poly_objects_ptrs()
  {}

  poly_objects_ptrs(std::initializer_list<std::shared_ptr<Item>>  init_list)
    :conts_(init_list)
  {}

  using cont_t = Container<std::shared_ptr<Item>>;
  cont_t  &get() { return conts_; }
  const cont_t  &get() const { return conts_; }

  void execute() {
    std::for_each(conts_.begin(), conts_.end(), [&](typename const cont_t::value_type  &val)
    {
      std::cout << val << std::endl;
    });
  }

private:
  cont_t  conts_;
};


//struct ByValue {};
template<typename Item, template<typename ...> class Container, typename Enable = void>
class poly_object_wrapper {
public:
  using cont_t = Container<std::shared_ptr<Item>>;
  cont_t data_;
};

struct ByValue {};

template<typename Item, template<typename ...> class Container, typename Enable>
class poly_object_wrapper<Item, Container, typename std::enable_if<std::is_same<ByValue, Enable>::value>::type> {
public:
  using cont_t = Container<Item>;
  cont_t data_;
};