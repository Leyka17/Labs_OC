#include"topology.h"
#include<tuple>


Topology::Topology()=default;
Topology::~Topology(){}

void Topology::Insert(const int& parent,const int& id)
{
     
    if((Check_parent(parent))&&(!Check_id(id)))
    {
        
        idInTree.insert(id);
        if(parent==-1)
        {
            std::vector<int>tmp;
            tmp.push_back(id);
            tree.push_back(tmp);
        }
        else
        {
            std::pair<size_t,size_t>index_parent=Find(parent);
            //std::cout<<std::get<0>(index_parent)<<" "<<std::get<1>(index_parent)<<" "<<id<<std::endl;
            tree[std::get<0>(index_parent)].insert(tree[std::get<0>(index_parent)].begin()+std::get<1>(index_parent)+1,id);
        }
        
    }

}


bool Topology::Check_parent(const int& parent) const
{
    if((parent>=-1)&&parent!=0)
    {
        if (parent>0)
        {
            return idInTree.count(parent);
        }
        return true; 
    }
    std::cout<<"Negative id"<<std::endl;
    return false;
    
}

bool Topology::Check_id(const int& id)const
{
    if(id>0)
    {
        return idInTree.count(id);
    }
    std::cout<<"id shoul be positive"<<std::endl;
    return false;
}   

std::pair<size_t,size_t> Topology::Find(const int& id)
{
    for(size_t i=0;i<tree.size();++i)
    {
        for(size_t j=0;j<tree[i].size();++j)
        {
           
            if(tree[i][j]==id)
            {
               std::pair<size_t,size_t>tmp{i,j};
                //std::cout<<i<<" "<<j<<" "<<id<<std::endl;
               return tmp;
            }
        }
    }

}

void Topology::Remove(const int& id)
{
    if(Check_id(id))
    {
        std::pair<size_t,size_t>tmp=Find(id);
        
        size_t vec=std::get<0>(tmp),elem=std::get<1>(tmp);
        //std::cout<<vec<<" "<<elem<<" "<<id<<std::endl;
        for(size_t i=elem;i<tree[vec].size();++i)
        {
            idInTree.erase(tree[vec][i]);
        }
        if(elem>0)
        {
            tree[vec].erase(tree[vec].begin()+elem,tree[vec].end());
            return;
        }
    
        tree.erase(tree.begin()+vec);
    }
    return;
}


void Topology::Print()const 
{
    for(size_t i=0;i<tree.size();++i)
    {
        for(size_t j=0;j<tree[i].size();++j)
            std::cout<<tree[i][j]<<" ";
        std::cout<<std::endl;
    }
}

std::vector<int> Topology::Get_path(const int& parent_id)
{
    std::tuple<size_t,size_t>tmp=Find(parent_id);
    std::vector<int>res;
    for(size_t j=0;j<=std::get<1>(tmp);++j)
    {
            
        res.push_back(tree[std::get<0>(tmp)][j]);
    }
    return res;
}

bool Topology::Has_child(const int& parent_id)
{
    std::tuple<size_t,size_t>tmp=Find(parent_id);
    if((std::get<1>(tmp)+1)==tree[std::get<0>(tmp)].size())
        return false;
    return true;

}


std::string z_function(std::string s,std::string patern)
{
	size_t lenPatern=patern.size();
    s=patern+'$'+s;
    size_t len=s.size();
	std::vector<size_t> z (len);
    std::string result;
    size_t left=0,right=0;
	for (size_t i=1; i<len; ++i) {
		if (i <= right)
			z[i] = std::min(right-i+1, z[i-left]);
		while (i+z[i] < len && s[z[i]] == s[i+z[i]])
			++z[i];
		if (i+z[i]-1 > right)
			left = i,  right = i+z[i]-1;
        if(z[i]==lenPatern)
            result=result+std::to_string(i-lenPatern-1)+';';
	}
    if(!result[0])
    {
        result=result+'-'+std::to_string(1);
       
    }
	return result;
}
