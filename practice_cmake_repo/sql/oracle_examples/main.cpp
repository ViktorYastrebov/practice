#include "test_suits.h"

//http://vrogier.github.io/ocilib/

//https://docs.oracle.com/cd/B12037_01/appdev.101/b10779/oci16ms8.htm#458407
// this example demonstrate the 

auto main(int argc, char *argv[]) -> int {
  std::string user("system");
  std::string pwd("123456");
  std::string serverName("xe");

  //example_direct_usage(user, pwd, serverName);
  //example_func(user, pwd, serverName);
  //numbers_define_example(user, pwd, serverName);
  //numbers_bind_example(user, pwd, serverName);
  //interative_insert(user, pwd, serverName);
  transactions(user, pwd, serverName);
  return 0;
}
