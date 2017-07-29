#pragma once

#include <type_traits>
#include <utility>

template < typename T >
class capture_wrapper
{
  static_assert(!std::is_rvalue_reference<T>::value, "");
  std::remove_const_t<T> mutable val_;
public:
  constexpr explicit capture_wrapper(T&& v)
    noexcept(std::is_nothrow_move_constructible<std::remove_const_t<T>>::value)
    :val_(std::move(v)) {}
  constexpr T&& get() const noexcept { return std::move(val_); }
};

template < typename T >
class capture_wrapper<T&>
{
  T& ref_;
public:
  constexpr explicit capture_wrapper(T& r) noexcept : ref_(r) {}
  constexpr T& get() const noexcept { return ref_; }
};

template < typename T >
constexpr typename std::enable_if<std::is_lvalue_reference<T>::value, capture_wrapper<T>>::type
capture(std::remove_reference_t<T>& t) noexcept
{
  return capture_wrapper<T>(t);
}

template < typename T >
constexpr typename std::enable_if<
  std::is_rvalue_reference<T&&>::value,  capture_wrapper<std::remove_reference_t<T>>>::type
capture(std::remove_reference_t<T>&& t)
noexcept(std::is_nothrow_constructible<capture_wrapper<std::remove_reference_t<T>>, T&&>::value)
{
  return capture_wrapper<std::remove_reference_t<T>>(std::move(t));
}

template < typename T >
constexpr typename std::enable_if<std::is_rvalue_reference<T&&>::value, capture_wrapper<std::remove_reference_t<T>>>::type
capture(std::remove_reference_t<T>& t)
noexcept(std::is_nothrow_constructible<capture_wrapper<std::remove_reference_t<T>>, T&&>::value)
{
  return capture_wrapper<std::remove_reference_t<T>>(std::move(t));
}

