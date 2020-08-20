#include "worker.h"
#include<string.h>
#include"exeption.h"





int main(int argc, const char*argv[])
{
    bool loop=true;
   
    try
    {
        Worker *worker = new Worker(argv[1],argv[2],argv[3]);
        std::string comand;
        std::string patern;
        std::string new_replace;
        size_t str_numer;
        worker->Menu();
        while(loop)
        {
            std::cout<<"Write comand"<<std::endl;
            std::cin>>comand;
            if(comand=="find")
            {
                
                std::cout<<"write patern"<<std::endl;
                std::cin>>patern;
                std::cout<< worker->Find(patern);
            }
            else if(comand=="replace")
            {
                std::cout<<"write what replace"<<std::endl;
                std::cin>>patern;
                std::cout<<"write new str for replace"<<std::endl;
                std::cin>>new_replace;
                std::cout<<worker->Replace(patern,new_replace)<<std::endl;
            }
            else if(comand=="print")
            {
                std::cout<<"write number of str to print"<<std::endl;
                std::cin>>str_numer;
                worker->Print(str_numer);

            }
            else if(comand=="exit")
            {
                loop=false;
                
            }
            else
            {
                std::cout<<"Not such comand"<<std::endl;
            }
            

        }
         delete worker;
    }
    catch(const MyExeption& e)
    {
        std::cout<<e.getMessage() << std::endl;
    }
   

    return 0;
}
