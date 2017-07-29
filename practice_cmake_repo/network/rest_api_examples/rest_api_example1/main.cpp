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

using namespace std;
using namespace restbed;

Service service;

void response_handler(const std::shared_ptr<Session>  session) {
  std::cout << "yield has finished" << std::endl;
}

void get_method_handler(const shared_ptr< Session > session)
{
  //session->close(OK, "Hello, World!", { { "Content-Length", "13" } });

  auto request = session->get_request();
  std::string header = request->get_query_parameter("close");
  if (header == std::string("1")) {
    std::string closeSignal("Session has been closed");
    session->close(restbed::OK, closeSignal, { {"Content-Length", std::to_string(closeSignal.length()) } });
    service.stop();
  }
  /*
  auto response = make_shared<Response>();
  std::string responeBody("{name : menuItem, { price : 123}}");
  response->set_header("response", "text");
  response->set_body(responeBody);
  session->yield(*response, response_handler);
  */
  std::string body("{name: menuItem, { price: 123 }}");
  session->yield(restbed::OK, body, { {"Content-Length", std::to_string(body.length()) } }, nullptr);
}

void bind_port() {
  auto resource = make_shared< Resource >();
  resource->set_path("/resource");
  resource->set_method_handler("GET", get_method_handler);

  auto settings = make_shared< Settings >();
  settings->set_port(1984);
  //settings->set_bind_address("127.0.0.1");
  settings->set_bind_address("109.86.210.10");
  //settings->set_default_header("Connection", "close");
  settings->set_status_message(restbed::OK, "Alive");

  service.publish(resource);
  service.start(settings);
}
//109.86.212.246 Vladimir
int main(const int, const char**)
{
  bind_port();
  return EXIT_SUCCESS;
}