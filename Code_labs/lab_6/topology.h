#pragma once
#include <string>
#include<iostream>
#include<string>
#include<set>
#include<vector>



class Topology
{
private:
std::vector<std::vector<int>>tree;
std::set<int>idInTree;
public:
Topology();
~Topology();
void Insert(const int& parent,const int& id);
bool Check_parent(const int& parent) const;
bool Check_id(const int& id)const;
std::pair<size_t,size_t> Find(const int& id);
void Remove(const int& id);
void Print()const;
std::vector<int> Get_path(const int& parent_id);
bool Has_child(const int& parent_id);
};

std::string z_function(std::string s,std::string patern);