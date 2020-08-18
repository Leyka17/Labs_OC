#pragma once
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<iostream>
#include<sys/mman.h>
#include<vector>
#include<algorithm>
#include <semaphore.h>
#include<string.h>
#include"exeption.h"

class Worker
{
     private:
     std::string file_name_;
     int open_parametr_;
     int size_mapping_;
     int fd_;
     off_t   file_size_; 

     int ToInt(const char*str)const;
     std::vector<size_t> Boyer_Mur(char*text,std::string patern,size_t size);
     void FindForReplace(char**text,std::string patern,std::string new_replace);
     std::vector<size_t> Skip_symbol(std::string patern)const;
     void Copy (char*text_to_copy,char**place_for_copy,size_t start,size_t pos_copy,size_t len);
     std::string EnterCounter(char*text,std::string patern,size_t *line_counter,size_t size);
     void ReplaceInStr(char**text,size_t position,std::string new_replace);

     public: 
     Worker(const char*file_name,const char *open_parametr,const char *size_mapping);
     ~Worker();
     void Menu();
     
    
     std::string Find(std::string patern);
     std::string Replace(std::string patern,std::string new_replace);
     void Print(size_t str_numer);




};