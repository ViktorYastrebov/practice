
#include <vector>
#include <numeric>
//#include <algorithm>

#include <iostream>

#include "tbb/blocked_range.h"
#include "tbb/parallel_reduce.h"
#include "tbb/parallel_for.h"
//#include "tbb/parallel_scan.h"

/*template<class T>
auto sum(const T  &l, const T& r)  -> decltype(l + y) {
  return l + r;
}*/

void init(const tbb::blocked_range<int*>  &range) {
  for (auto &value : range) {
    value = 1;
  }
}

int sum(const tbb::blocked_range<int*>  &range, int init) {
  for (auto &value : range) {
    init += value;
  }
  return init;
}

int join(const int  &l, const int &r) {
  return l + r;
}

auto main() -> int {
  

//===============================================================
  {
    //std::vector<int> data(100'000);
    const int datalen = 4'000'000;

    //std::vector<int> data(datalen);
    int *data = new int[datalen];

    //auto x_range = tbb::blocked_range<int*>(&(*data.begin()), &(*data.end()));
    auto x_range = tbb::blocked_range<int*>(data, data + datalen);
    tbb::parallel_for(x_range, &init);

    int result = tbb::parallel_reduce(x_range, 0, &sum, &join);

    std::cout << "result = " << result << std::endl;
  }
//=================================================================
  {
    const int datalen = 4'000'000;
    std::vector<int> data(datalen, 1);

    int value = tbb::parallel_reduce(tbb::blocked_range<std::vector<int>::const_iterator>(data.begin(), data.end()), 0,
      [](const tbb::blocked_range<std::vector<int>::const_iterator>  &it, int init) {
      return std::accumulate(it.begin(), it.end(), init);
    },
      [](const int  &l, const int  &r) {
      return l + r;
    }
    );
  }

  return 0;
}