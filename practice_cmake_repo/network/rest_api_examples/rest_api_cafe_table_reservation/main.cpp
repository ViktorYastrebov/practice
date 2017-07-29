/*
* Example illustrating binding service to specific address.
*
* Server Usage:
*    ./distribution/example/bind_service_address
*
* Client Usage:
*    curl -w'\n' -v -XGET 'http://127.0.0.1:1984/resource'
*/

#include <memory>
#include <cstdlib>
#include <restbed>

#include <iostream>

#include "server.hpp"


auto main(int argc, char* argv[]) -> int
{
  Session session({ {"rest_api"} }, 1984, "109.86.210.10");
  session.start();
  return 0;
}