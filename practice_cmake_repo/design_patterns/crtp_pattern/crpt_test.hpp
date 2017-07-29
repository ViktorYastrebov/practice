#pragma once

template<typename T, template<typename U = T> class implementation>
class interface_casts {
public:
  virtual ~interface_casts()
  {}

  T process() {
    impl().process<type>();
  }

private:
  implementation<T> &impl() {
    return *static_cast<implementation<T>*>(this);
  }
};

template<typename T>
class plus_process
  : public interface_casts<T, plus_process>
{
public:
  plus_process()
    : t1_(), t2_()
  {}

  plus_process(const T  &t1, const T &t2)
    :t1_(t1), t2_(t2)
  {}

  ~plus_process() {}

  T process() {
    return t1_ + t2_;
  }
private:
  T t1_;
  T t2_;
};