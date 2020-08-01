#include"function_server.h"
#include<iostream>



bool send_mess(zmq::socket_t& socket, const std::string& message_str) 
{
    zmq::message_t message(message_str.size());
    memcpy(message.data(), message_str.c_str(), message_str.size());//copping
    return socket.send(message);
}

std::string recieve_mess(zmq::socket_t& socket) {
    zmq::message_t message;
    bool ok;
    try {
        ok = socket.recv(&message);
    } catch (...) {
        ok = false;
    }
    std::string recv_message(static_cast<char*>(message.data()), message.size());
    if (recv_message.empty())
    {
        return "Error: Node is not available";
    }
    return recv_message;
}

void createNode(int parent_id,int id, int port)
{
    char* arg1 = strdup((std::to_string(id)).c_str());
    char*arg2=strdup((std::to_string(parent_id)).c_str());
    char* arg3 = strdup((std::to_string(port)).c_str());
    
    char* args[] = {"./my_slave", arg1, arg2, arg3,NULL};
    execv("./my_slave", args);
}

std::string get_port_name(int port) {
    return "tcp://127.0.0.1:" + std::to_string(port);
}

int bindSocket(zmq::socket_t& socket,int port_) {
    int port = 30000;
    while (true) 
    {
        try 
        {
            if(port==port_)
            {
                ++port;
            socket.bind(get_port_name(port));
            break;
            }
            else
            {
                socket.bind(get_port_name(port));
                break;
            }
            
        }
        catch(...) 
            {
            ++port;
            }
    }
    return port;
}


