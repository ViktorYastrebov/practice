#include <algorithm>
#include <vector>

#include <iostream>

auto main() -> int {
  std::vector<float> v = { 1.0f, 2.0f, 0.0f, 10.0f, 5.0f,3.14f };
  auto resultIter = std::max_element(v.begin(), v.end());
  std::ptrdiff_t idx = std::distance(v.begin(), resultIter);
  std::cout << "max element is: " << idx << std::endl;
  return 0;
}
