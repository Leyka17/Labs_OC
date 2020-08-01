#pragma once
#include <string>
#include <zconf.h>
#include "zmq.hpp"



bool send_mess(zmq::socket_t& socket, const std::string& message_str);

std::string recieve_mess(zmq::socket_t& socket);

std::string get_port_name(int port);

int bindSocket(zmq::socket_t& socket,int port_=0);

void createNode(int parent_id,int id, int port);