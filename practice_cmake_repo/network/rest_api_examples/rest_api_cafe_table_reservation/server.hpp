#pragma once 


#include <map>
#include <memory>
#include <restbed>


class Session {
public:

  explicit Session(const std::initializer_list<std::string>  &list, unsigned int port, const std::string  &address);
  virtual ~Session();

  void start();
  void stop();

private:
  void get_handler(const std::shared_ptr<restbed::Session>  &session);
  void post_handler(const std::shared_ptr<restbed::Session>  &session);
  void put_handler(const std::shared_ptr<restbed::Session>  &session);
private:
  restbed::Service service_;
  //std::map<std::string, std::shared_ptr<restbed::Resource>> resource_map_;
  std::shared_ptr<restbed::Resource> res_;
  std::shared_ptr<restbed::Settings> settings_;

  //static constexpr const char content_length[] = "Content-Length";
  static const std::string CONTENT_LEGNTH;
};