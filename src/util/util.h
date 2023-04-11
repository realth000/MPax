#ifndef MPAX_UTIL_H
#define MPAX_UTIL_H

// https://forum.qt.io/post/528081
template <class Container>
class ContainerAdapt {
 public:
  explicit ContainerAdapt(const Container& container) : c(container) {}
  typename Container::const_key_value_iterator begin() const {
    return c.keyValueBegin();
  }
  typename Container::const_key_value_iterator end() const {
    return c.keyValueEnd();
  }

 private:
  const Container& c;
};
#endif  // MPAX_UTIL_H
