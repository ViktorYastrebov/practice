#pragma once



#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

const int max_length = 1024;

void session(tcp::socket sock);
void server(boost::asio::io_service& io_service, unsigned short port);
void start_blocking_server();