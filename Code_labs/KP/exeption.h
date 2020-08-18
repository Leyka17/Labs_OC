#pragma once
#include<string>


class MyExeption
{
public:
    MyExeption(const std::string& msg) : msg_(msg) {}
    ~MyExeption(){}
    std::string getMessage() const {
        return(msg_);}

private:
    std::string msg_;
};