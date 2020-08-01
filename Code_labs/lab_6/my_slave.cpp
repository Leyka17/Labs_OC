#include <string>
#include <zconf.h>
#include "zmq.hpp"
#include<iostream>
#include<string>
#include<set>
#include<sstream>
#include<map>
#include<vector>
#include <csignal>

#include"topology.h"
#include"function_server.h"

int main(int argc, char** argv)
{


    int id_node_now = std::stoi(argv[1]);
    int parent_id = std::stoi(argv[2]);
    int parent_port = std::stoi(argv[3]);

    zmq::context_t context(3);
    zmq::socket_t parent_soket(context, ZMQ_REP);
    parent_soket.setsockopt(ZMQ_SNDTIMEO, 2000);
    zmq::socket_t child_soket(context, ZMQ_REQ);
    child_soket.setsockopt(ZMQ_SNDTIMEO, 2000);
    child_soket.setsockopt(ZMQ_RCVTIMEO,2000);
    
    
    std::map<int, int> pid_dict;
    std::map<int, int> port_dict;
    std::map<int,int>parent_child;


    parent_soket.connect(get_port_name(parent_port));
    

    int child_pid=0;
    bool work=true;
  
    while(work)
    {
        std::string request;
        request= recieve_mess(parent_soket);
        std::istringstream comand_stream(request);//format string from thread
        std::string comand;
        comand_stream >> comand;
       
        if(comand=="pid")
        {
            //std::cout<<"pid_id "<<id_node_now<<std::endl;
            std::string parent_string = "I can:" + std::to_string(getpid());
            send_mess(parent_soket, parent_string);
        }
        else if(comand=="create")
        {
            int new_id, size;
            bool has_child;
            pid_t pid;
			comand_stream >> size;

			std::vector<int> path(size);
            for (int i=0;i<size;++i)
				comand_stream >> path[i];
            comand_stream>>new_id;
            comand_stream>>has_child;
            if((size==0)&&(has_child==false))
            {
                
                int port = bindSocket(child_soket);
                port_dict[new_id]=port;
                
                pid=fork();
                if(pid<0)
                {
                    send_mess(parent_soket,"Sorry not fork");
                    continue;
                }
                if(pid==0)
                {
                    createNode(id_node_now,new_id,port);
                }
                else
                {
                   
					pid_dict[new_id] = pid;
                    parent_child[id_node_now]=new_id;
					send_mess(child_soket, "pid");
					send_mess(parent_soket, recieve_mess(child_soket));
                }
                
            }
            
            if((size==0)&&(has_child==true))
            {
                //std::cout<<id_node_now<<std::endl;
                int old_child=parent_child.find(id_node_now)->second; 
                int old_port=port_dict.find(old_child)->second;
               
            
                
                child_soket.unbind(get_port_name(old_port));
                int new_port = bindSocket(child_soket,old_port);

                port_dict[new_id]=new_port;
                parent_child[id_node_now]=new_id;

                //for(auto& item : port_dict)
                //    std::cout << item.first << " : " << item.second << std::endl;

                pid=fork();
                if(pid<0)
                {
                    send_mess(parent_soket,"Sorry not fork");
                    continue;
                }
                if(pid==0)
                {

                    createNode(id_node_now,new_id,new_port);
                }
                else
                {
                
                    std::ostringstream msg_stream;
                    msg_stream<<"reconect"<<" "<<old_port<<" "<<old_child;
                    send_mess(child_soket,msg_stream.str());
                    recieve_mess(child_soket);

                    pid_dict[new_id] = pid;
                    parent_child[id_node_now]=new_id;
                   
                    //std::cout<<id_node_now<<std::endl;
                    bool send_pid=send_mess(child_soket, "pid");
                    //std::cout<<"send_pid "<<send_pid<<std::endl;
                   
                   
                    send_mess(parent_soket,recieve_mess(child_soket));   
                    
                }
            
            }
            if(size>0)
            {
                //std::cout<<"//"<<id_node_now<<std::endl;
                
                /*for(int i=0;i<size;++i)
                {
                   std::cout<<"path "<<path[i]<<" ";
                }
                std::cout<<std::endl;*/

                int next_id=path.front();
                path.erase(path.begin());
                std::ostringstream msg_stream;
                
				msg_stream << "create " << path.size();
				for(int i : path) {
					msg_stream << " " << i;
				}
				msg_stream << " " << new_id<<" "<<has_child;
                bool send=send_mess(child_soket,msg_stream.str());
                send_mess(parent_soket,recieve_mess(child_soket));
            }
        }
        
    
        else if(comand=="reconect")
        {
            int child_id,child_port;
            comand_stream>>child_port;
            comand_stream>>child_id;
        
            
            //std::cout<<"hello "<<child_port<<std::endl;
            child_soket.bind(get_port_name(child_port));
            port_dict[child_id]=child_port;
            parent_child[id_node_now]=child_id;
            
            
            bool send=send_mess(child_soket,"pid");
            std::string result=recieve_mess(child_soket);
            result=result.substr(6,result.size());
            pid_dict[child_id]=std::stoi(result);
            
            send_mess(parent_soket,"done");
        }
        else if(comand=="remove")
        {
            int size,remove_id;
            comand_stream>>size;
            --size;
            std::vector<int>path(size);
            for(int i=0;i<size;++i)
            {
                comand_stream>>path[i];
            }
            comand_stream>>remove_id;
            if((size)==0)
            {
                //std::cout<<"**"<<id_node_now<<std::endl;
                send_mess(child_soket,"kill");
                recieve_mess(child_soket);
                //std::cout<<"i am here\n";
                kill(pid_dict.at(remove_id),SIGTERM);
                kill(pid_dict.at(remove_id),SIGKILL);
     
                pid_dict.erase(remove_id);
                //std::cout<<"pid del\n";
                child_soket.unbind(get_port_name(port_dict.find(remove_id)->second));
                //std::cout<<"bind\n";
                port_dict.erase(remove_id);
                //::cout<<"port_del\n";
                parent_child.erase(id_node_now);
                //std::cout<<"chid-del\n";
                send_mess(parent_soket,"Ok");
            }
            else
            {
                //std::cout<<"//"<<id_node_now<<std::endl;
                path.erase(path.begin());
                path.push_back(remove_id);
                std::ostringstream msg_stream;
                msg_stream<<"remove "<<path.size();
                for(int i : path) {
					msg_stream << " " << i;
				}
                send_mess(child_soket,msg_stream.str());
                send_mess(parent_soket,recieve_mess(child_soket));
            } 

        }
        else if(comand=="kill")
        {
            //std::cout<<id_node_now<<std::endl;
            if(parent_child.count(id_node_now)!=0)
            {
                int child_id=parent_child[id_node_now];
                //std::cout<<"child id "<<child_id<<std::endl;
                send_mess(child_soket,"kill");
                recieve_mess(child_soket);
                //std::cout<<"i am here first\n ";
                child_soket.unbind(get_port_name(port_dict.find(child_id)->second));
                kill(pid_dict.at(child_id),SIGTERM);
                kill(pid_dict.at(child_id),SIGKILL);
            }
            //std::cout<<"hello\n";
            bool send=send_mess(parent_soket,"Ok");
            //std::cout<<send<<std::endl;
        }
        else if(comand=="exec")
        {
            int size;
            std::string s,patern,z_fun,result;
            comand_stream>>size;
            std::vector<int>path(size);
            for(size_t i=0;i<size;++i)
            {
                comand_stream>>path[i];
            } 
            comand_stream>>s;
            comand_stream>>patern;
            if(size==0)
            {
              z_fun=z_function(s,patern);
              result=result+"Ok:"+std::to_string(id_node_now)+":"+z_fun;
              //std::cout<<result<<std::endl;
              send_mess(parent_soket,result);
            }
            else
            {
                int next_id=path.front();
                path.erase(path.begin());
                std::ostringstream msg_stream;
                
				msg_stream << "exec " << path.size();
				for(int i : path) {
					msg_stream << " " << i;
				}
				msg_stream << " " << s<<" "<<patern;
                bool send=send_mess(child_soket,msg_stream.str());
                send_mess(parent_soket,recieve_mess(child_soket));
            }
            
        }
        else if(comand=="heartbeat")
        {
            int heartbeatTime;
            std::ostringstream msg_stream;
            std::cout<<id_node_now;
            if(parent_child.find(id_node_now)==parent_child.end())
            {
        
                send_mess(parent_soket,"Done");
            }
            else
            {
                msg_stream<<"heartbeat ";
                send_mess(child_soket,msg_stream.str());
                zmq::message_t result;
                bool ok=child_soket.recv(&result);
                if(!ok)
                {
                    std::ostringstream msg_stream;
                    std::string recv_message(static_cast<char*>(result.data()), result.size());
                    msg_stream<<"Hearbit:node not aviable now:"<<parent_child[id_node_now]<<"\n"<<recv_message;
                    send_mess(parent_soket,msg_stream.str());
                }
                else
                {
                    std::ostringstream msg_stream;
                    std::string recv_message(static_cast<char*>(result.data()), result.size());
                    msg_stream<<recv_message<<" "<<"Done "<<id_node_now;
                    send_mess(parent_soket,msg_stream.str());
                }  
            } 

        }

    }
    



    return 0;
}