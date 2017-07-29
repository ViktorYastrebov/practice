#include <algorithm>
//#include <string>
#include <locale>
#include <iterator>

#include "server.hpp"

//add the base polimorthic type for instantination at runtime somehow.
//it can be: PolyType/SingleType. See Syb. The main idea: dynamicaly create objects and process them from JSON

const std::string Session::CONTENT_LEGNTH = "Content-Length";

Session::Session(const std::initializer_list<std::string>  &list, unsigned int port, const std::string  &address)
  :service_(), res_(std::make_shared<restbed::Resource>()), settings_(std::make_shared<restbed::Settings>())
{
  res_->set_paths(list);
  res_->set_method_handler("GET", std::bind(&Session::get_handler, this, std::placeholders::_1));
  res_->set_method_handler("POST", std::bind(&Session::post_handler, this, std::placeholders::_1));
  res_->set_method_handler("PUT", std::bind(&Session::put_handler, this, std::placeholders::_1));

  settings_->set_port(port);
  settings_->set_bind_address(address);
}

Session::~Session() {
  service_.stop();
}

void Session::start() {
  service_.publish(res_);
  service_.start(settings_);
}

void Session::stop() {
  if (service_.is_up()) {
    service_.stop();
  }
}

void Session::get_handler(const std::shared_ptr<restbed::Session>  &session) {
  auto request = session->get_request();

  auto shouldClose = request->get_query_parameter("close");
  if (shouldClose == "1") {
    std::string closeSignal("Session has been closed");
    session->close(restbed::OK, closeSignal, { { CONTENT_LEGNTH, std::to_string(closeSignal.length()) } });
    stop();
    return;
  }

  std::string responseBody;
  auto params = request->get_query_parameters();
  if (!params.empty()) {
    auto iter = params.begin();
    responseBody += iter->first + std::string(": ") + iter->second + std::string(" + modified");
    responseBody.insert(0, 1, '{');
    responseBody.push_back('}');
    ++iter;

    for(iter; iter != params.end(); ++iter) {
    //for (iter : params) {
      responseBody.push_back(',');
      responseBody += iter->first + std::string(": ") + iter->second + std::string("modified");
      responseBody.insert(0, 1, '{');
      responseBody.push_back('}');
    }
  }
  session->yield(restbed::OK, responseBody, { { CONTENT_LEGNTH, std::to_string(responseBody.length()) } }, nullptr);
}

void Session::post_handler(const std::shared_ptr<restbed::Session>  &session) {
  auto req = session->get_request();
  std::string len = req->get_header(CONTENT_LEGNTH);
  std::size_t content_length = std::stoull(len);
  session->fetch(content_length,
    [req](const std::shared_ptr<restbed::Session> session, const restbed::Bytes  &body)
  {
    restbed::Byte modData[] = { " post_data: " };
    restbed::Bytes responce(&modData[0], &modData[sizeof(modData) / sizeof(restbed::Byte)]);
    std::copy(body.begin(), body.end(), std::back_inserter(responce));

    session->yield(restbed::OK, responce, { { CONTENT_LEGNTH, std::to_string(responce.size())} });
  });
}

void Session::put_handler(const std::shared_ptr<restbed::Session>  &session) {
  std::string putResponse("put has been processed");
  session->yield(restbed::OK, putResponse, { { CONTENT_LEGNTH, std::to_string(putResponse.length()) } });
}