//Be carefull with it! It  works!
#include <string>
#include <zconf.h>
#include "zmq.hpp"
#include<iostream>
#include<string>
#include<set>
#include<sstream>
#include<vector>
#include<map>
#include <sys/types.h>
#include<chrono>
#include <signal.h>

#include"topology.h"
#include"function_server.h"
struct Node
{
int id_;
int port_;
};



void Menu()
{
    std::cout<<"Menu for server"<<std::endl<<
               "Comnds:"<<std::endl<<
               "1)сreate parent_node id_node"<<std::endl<<
               "2)remove id_node"<<std::endl<<
               "4)exec id_node text_string parent_string"<<std::endl<<
               "5)hearbit time"<<std::endl<<
               "6)exit"<<std::endl;


}



int main()
{
    Topology ServerTopology;
    zmq::context_t context(1);//1- count sokets
    std::map<int, zmq::socket_t> sokets_dict;
    std::map<int, int> pid_dict;
    std::map<int, int> port_dict;

    /*
    zmq::socket_t soket(context,ZMQ_REQ);
    soket.setsockopt(ZMQ_SNDTIMEO, 2000);//устанавливает парематр для сокета на отправку в течении 2000 милисек сокет будет пытаться отправлять сообщение если не получилось то ошибка
    int port = bindSocket(soket);//подключение к порту
    */
    std::string comand;
    bool work=true;
    pid_t pid;
    Menu();
    pid_t child=0;
    int linger=0;
    while(work)
    {
        std::cout<<"Write comand"<<std::endl;
        std::string result;
        int id_node,parent_id;
        
        std::cin>>comand;
        if(comand=="create")
        {
            std::cout<<"parent_id"<<std::endl<<"id_node"<<std::endl;
            std::cin>>parent_id>>id_node;
            if(ServerTopology.Check_id(id_node))
               {std::cout<<"Id is exist"<<std::endl;}
            else
            {
                if(parent_id==-1)
                {
                    sokets_dict.emplace(id_node,zmq::socket_t(context,ZMQ_REQ));
                    sokets_dict.at(id_node).setsockopt(ZMQ_SNDTIMEO, 2000);
                    sokets_dict.at(id_node).setsockopt(ZMQ_RCVTIMEO, 2000);
                    //sokets_dict.at(id_node).setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
                    int port = bindSocket(sokets_dict.at(id_node));
                    port_dict.emplace(id_node,port);
                    pid=fork();
                    if(pid<0)
                    {std::cout<<"Error with fork"<<std::endl;
                    exit(1);}
                    if(pid==0)
                    {
                        createNode(parent_id,id_node, port);
                    }
                    else
                    {
                        pid_dict.emplace(id_node,pid);
                        send_mess(sokets_dict.at((id_node)),"pid");
                        result = recieve_mess(sokets_dict.at((id_node)));
                    }

                }
                else
                {
                    if(ServerTopology.Check_parent(parent_id))
                    {
                        std::vector<int>path=ServerTopology.Get_path(parent_id);
                        if (path.empty())
                        {
                            std::cout << "I can not make path "<<std::endl;
                            continue;
				        }
                        bool has_child=ServerTopology.Has_child(parent_id);
                        int next_id= path.front();
				        path.erase(path.begin());
                        std::ostringstream msg_stream;
				        msg_stream << "create " << path.size();
				        for (auto i : path) 
                        {		
					        msg_stream << " " << i;
				        }
                        msg_stream << " " << id_node<<" "<<has_child;

                        bool send=send_mess(sokets_dict.at(next_id), msg_stream.str());
                        result = recieve_mess(sokets_dict.at(next_id));
                    }
                    else
                    {   std::cout<<"Not find parent"<<std::endl;
                        continue;
                    }
                }
                std::string res=result.substr(0,5);
                if (res == "I can") 
                {
                    ServerTopology.Insert(parent_id,id_node);
                }
                std::cout<<result<<std::endl;
            }
           
            

        }
        else if(comand=="remove")
        {
            int node_id;
            std::cout<<"write node id to delate"<<std::endl;
            std::cin>>node_id;
            if(!ServerTopology.Check_id(node_id))
            {
                std::cout<<"Not that id"<<std::endl;
            }
            else
            {
                if(pid_dict.count(node_id)!=0)
                {
    
                    send_mess(sokets_dict.at(node_id), "kill");
                    result = recieve_mess(sokets_dict.at(node_id));
                    kill(pid_dict.at(node_id), SIGTERM);
                    kill(pid_dict.at(node_id), SIGKILL); 
                    pid_dict.erase(node_id);
                    sokets_dict.at(node_id).unbind(get_port_name(port_dict[node_id]));
                    sokets_dict.erase(node_id);
                    port_dict.erase(node_id);
                }
                else
                {
                    
                
                    std::vector<int>path=ServerTopology.Get_path(node_id);
                    if (path.empty())
                    {
                        std::cout << "I can not make path "<<std::endl;
                        continue;
                    }
                    int next_id= path.front();
                    path.erase(path.begin());
                    std::ostringstream msg_stream;
                    msg_stream << "remove " << path.size();
                    for (auto i : path) 
                    {		
                        msg_stream << " " << i;
                    }
                    bool send=send_mess(sokets_dict.at(next_id), msg_stream.str());
                    result = recieve_mess(sokets_dict.at(next_id));   
                }
                
                if (result == "Ok") 
                {
                    ServerTopology.Remove(node_id);
                }
                std::cout<<result<<std::endl;

            }
        }
        else if(comand=="exec")
        {
            int node_id;
            std::string s,patern;
            std::cout<<"write node id of worker"<<std::endl;
            std::cin>>node_id;
            std::cout<<"write string"<<std::endl;
            std::cin>>s;
            std::cout<<"write string patern "<<std::endl;
            std::cin>>patern;
            if(!ServerTopology.Check_id(node_id))
            {
                std::cout<<"Not that id"<<std::endl;
            }
            else
            {
                    std::vector<int>path=ServerTopology.Get_path(node_id);
                    if (path.empty())
                    {
                        std::cout << "I can not make path "<<std::endl;
                        continue;
                    }
                    int next_id= path.front();
                    path.erase(path.begin());
                    std::ostringstream msg_stream;
                    msg_stream << "exec "<< path.size();
                    for (auto i : path) 
                    {		
                        msg_stream << " " << i;
                    }
                    msg_stream<<" "<<s<<" "<<patern;
                    bool send=send_mess(sokets_dict.at(next_id), msg_stream.str());
                    result = recieve_mess(sokets_dict.at(next_id));   
                }

                std::cout<<result<<std::endl;
            }
        else if(comand=="heartbeat")
        {
            int heartbeatTime=0;
            std::ostringstream msg_stream;
            std::chrono::time_point<std::chrono::system_clock> start;
            std::string result;
            std::cout<<"Time"<<std::endl;
            std::cin>>heartbeatTime;
            msg_stream<<"heartbeat ";
            bool work=true;
            bool broken=false;
            start=std::chrono::system_clock::now();

            while(work)
            {

                std::chrono::time_point<std::chrono::system_clock> time_now;
                time_now=std::chrono::system_clock::now();
                int delta=std::chrono::duration_cast<std::chrono::milliseconds>(time_now- start).count();
                if(delta<heartbeatTime)
                    continue;
                for(auto&socket: sokets_dict)
                {
                    bool ok=send_mess(socket.second,msg_stream.str());
                    //std::cout<<"send_heartbeat "<<ok<<std::endl;
                    result=recieve_mess(socket.second);
                    //std::cout<<result<<std::endl;
                    if(result.substr(0,7)=="Hearbeat")
                    {   
                        size_t end_output=result.find("Done");

                        std::cout<<result.substr(0,end_output)<<std::endl;
                        broken=true;
                    }
                }
                if(!broken)
                    std::cout<<"Ok"<<std::endl;
                std::string to_continue;
                std::cout<<"Do you want continue heartbeat"<<std::endl<<"y/n"<<std::endl;
                std::cin>>to_continue;
                if(to_continue=="n")
                    work=false;
                else if(to_continue=="y")
                {
                    broken=false;
                    start=std::chrono::system_clock::now();
                }
                else
                    std::cout<<"wrong command";

            }
        }
        else if(comand=="exit")
        {
            for(auto&socket: sokets_dict)
            {
                send_mess(socket.second,"kill");
                result = recieve_mess(socket.second);
                kill(pid_dict.at(socket.first), SIGTERM);
                kill(pid_dict.at(socket.first), SIGKILL); 
            }
            work=false;
            std::cout<<"Thanks for choosing us!"<<std::endl;
        }
        else
        {
            std::cout<<"Not such comand"<<std::endl;
        }
        

    }



    return 0;
}


