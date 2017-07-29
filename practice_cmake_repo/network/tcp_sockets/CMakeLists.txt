cmake_minimum_required(VERSION 3.5)

project(tcp_sockets)

file(CLIENTS_SOURCES main_client.cpp)
# https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/
# https://vcansimplify.wordpress.com/2013/03/14/c-socket-tutorial-echo-server/
# https://github.com/mafintosh/echo-servers.c.git
# http://www.tutorialspoint.com/unix_sockets/socket_server_example.htm
# http://www.theinsanetechie.in/2014/01/tcp-in-c-one-server-and-multiple-clients.html

file(SERVERS_SOURCES main_server.cpp)
add_executable(tcp_server ${SERVERS_SOURCES})

