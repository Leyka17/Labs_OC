#include"worker.h"


Worker::Worker(const char*file_name,const char*open_parametr,const char* size_mapping)
{
    if(file_name&&strlen(file_name)>0)
        file_name_=file_name;
    else
    {
        throw MyExeption("Empty name");
    }
    if(!open_parametr||strlen(open_parametr)<0)
        throw MyExeption("open paramert is empty");
    if(ToInt(size_mapping)>0)
       {
            size_t tmp=ToInt(size_mapping);
            size_t PAGE_SIZE=4096;
            if(tmp%4096==0)
                size_mapping_=tmp;
            else
                size_mapping_=(tmp/PAGE_SIZE+1)*PAGE_SIZE;
            std::cout<<"Your size for mapping will transform to nearest page_size " <<size_mapping_<<std::endl;
       }
    else
    {
        throw MyExeption("size of mapping is not positive");
    }
    struct stat file_info;
    if(strcmp(open_parametr,"read")==0)
    {
        fd_=open(file_name, O_RDONLY,S_IRUSR);
        open_parametr_=PROT_READ;
        if(fd_==-1)
            throw MyExeption("Dont open file");
    }
    else if(strcmp(open_parametr,"write")==0)
    {
        fd_=open(file_name, O_RDWR,S_IRUSR);
        open_parametr_=PROT_READ|PROT_WRITE;
        if(fd_==-1)
            throw MyExeption("Dont open file");
    }
    else
        throw MyExeption("Not such parametr");
    
    fstat(fd_, &file_info);
    file_size_=file_info.st_size;
    if(file_size_==0)
        throw MyExeption("Empty file");
    
}

Worker::~Worker()
{
    close(fd_);
}

int Worker::ToInt(const char *str)const
{
    if(str[0]=='-'|| strlen(str)==0)
        return -1;
    int translate=str[0]-'0';
    size_t len=strlen(str);
    for(size_t i=1;i<len;++i)
    {
       translate=translate*10+(str[i]-'0');
    }
    return translate;
}

void Worker::Copy (char*text_to_copy,char**place_for_copy,size_t start,size_t pos_copy,size_t len)
{
    int j=pos_copy;
    char a=text_to_copy[start];
    for(size_t i=start;i<len+start;++i)
        {
            char a=text_to_copy[i];
            (*place_for_copy)[j]=text_to_copy[i];
            ++j;
        }
}

std::vector<size_t> Worker::Skip_symbol(std::string patern)const
{
    std::vector<size_t> result(256,patern.size());
    for(size_t i=0;i<patern.size()-1;++i)
    {
            result[patern[i]]=patern.size()-i-1;
        
    }
    return result;
}

std::vector<size_t> Worker::Boyer_Mur(char*text,std::string patern,size_t size)
{
    std::vector<size_t> shift;
    shift=Skip_symbol(patern);
    std::vector<size_t> result;
    size_t skip=0;
    while(size-skip>=patern.size())
    {
        size_t i=patern.size()-1;
        while(text[i+skip]==patern[i])
        {
            if(i==0)
            {
                result.push_back(skip);
                break;
            }
            --i;
        }
        skip=skip+shift[text[skip+patern.size()-1]];
    }
    if(result.empty())
        result.push_back(-1);
    return result;
}


void Worker:: FindForReplace(char**text,std::string patern,std::string new_replace,size_t size)
{
    std::vector<size_t> shift;
    shift=Skip_symbol(patern);
    size_t skip=0;
    while(size-skip>=patern.size())
    {
        size_t i=patern.size()-1;
        while((*text)[i+skip]==patern[i])
        {
            
            if(i==0)
            {
               size_t j=0;
               for(size_t k=skip;k<skip+new_replace.size();++k)
                {
                    (*text)[k]=new_replace[j];
                    ++j;
                }
                break;
            }
            --i;
        }
        skip=skip+shift[(*text)[skip+patern.size()-1]];
    }
}

std::string Worker::EnterCounter(char*text,std::string patern,size_t *line_counter,size_t size)
{
    std::vector<size_t> find_pos=Worker::Boyer_Mur(text,patern,size);
    std::vector<size_t> result;
    std::string output;
    for(size_t i=0;i<size+1;++i)
    {
        
        if(text[i]=='\n'||text[i]=='\0')
        { 
            while((find_pos.size()>0)&&(find_pos[0]<i)&&find_pos[0]!=-1)
            {
                output+="line "+std::to_string(*line_counter)+'\n';
                find_pos.erase(find_pos.begin());
            }
            if(text[i]=='\n')
                ++(*line_counter);  
        }
    }
    return output;
}



std::string Worker::Replace(std::string patern,std::string new_replace)
{
    size_t position=0;
    char*text_tmp,*text;
    size_t size_of_patern=patern.size();
    char*copy_mass=new char[patern.size()-1];
    size_t size_mapping=size_mapping_;
    if(open_parametr_==PROT_READ)
    {
        return "File only for reading";
    }
    if(new_replace.size()!=patern.size())
    {
        return "size of new word!=size of old word";
    }
    while(position<file_size_)
    {
        if(file_size_-position<size_mapping)
            size_mapping=file_size_-position;
        if(position>=size_mapping)
        {
            text_tmp=(char*)mmap(NULL,size_mapping,open_parametr_,MAP_SHARED,fd_,position);
            Copy(text_tmp,&copy_mass,0,size_of_patern-1,size_of_patern-1);
            std::vector<size_t> pos=Boyer_Mur(copy_mass,patern,(size_of_patern-1)*2);
            if(pos[0]!=-1)
            {
                size_t j=0;
                for(size_t i=size_mapping-size_of_patern+1+pos[0];i<size_mapping;++i)
                {
                    text[i]=new_replace[j];
                    ++j;
                }
                size_t start=j;
                for(size_t i=0;i<size_of_patern-start;++i)
                {
                    text_tmp[i]=new_replace[j];
                    ++j;
                } 
            }
            munmap(text,size_mapping_);
            text=text_tmp;
            FindForReplace(&text,patern,new_replace,size_mapping);
            
        }
        else
        {
            text=(char*)mmap(NULL,size_mapping,open_parametr_,MAP_SHARED,fd_,position);
            FindForReplace(&text,patern,new_replace,size_mapping);
        }
        position+=size_mapping;
        copy_mass=new char[size_of_patern-1];
        if(position<file_size_)
            Copy(text,&copy_mass,size_mapping-size_of_patern+1,0,size_of_patern-1);
        
        
    }
    delete []copy_mass;
    munmap(text,size_mapping_);
    return "ok";
}


std::string Worker::Find(std::string patern)
{
    size_t position=0, size_mapping=size_mapping_;
    size_t line_counter=0;
    char*text;
    size_t size_of_patern=patern.size();
    std::string result;
    char*copy_mass=new char[2*size_of_patern-2];
    memset (copy_mass,'\0',2*size_of_patern-2);
    while(position<file_size_)
    {
        text=(char*)mmap(NULL,size_mapping,open_parametr_,MAP_SHARED,fd_,position); 
        if(file_size_-position<size_mapping)
            size_mapping=file_size_-position;
        if(position>=size_mapping)
        {
           
            Copy(text,&copy_mass,0,size_of_patern-1,size_of_patern-1);
            std::vector<size_t> tmp_pos=Boyer_Mur(copy_mass,patern,(size_of_patern-1)*2);
            if(tmp_pos[0]!=-1)
                result+="line "+std::to_string(line_counter)+'\n';
            result+=EnterCounter(text,patern,&line_counter,size_mapping);

        }
        else
            result+=EnterCounter(text,patern,&line_counter,size_mapping);
        position+=size_mapping;
        if(position<file_size_)
        {
            Copy(text,&copy_mass,size_mapping-size_of_patern+1,0,size_of_patern-1);
        }
        munmap(text,size_mapping_);

    }
    delete [] copy_mass;
    if(result.size()==0)
        return "Not found\n";
    return result;
}


void Worker::Print(size_t str_numer)
{
    size_t line_counter=0,position=0,size_mapping=size_mapping_;
    bool find=false;
    char*text;
    while(position<file_size_)
    {
        text=(char*)mmap(NULL,size_mapping,open_parametr_,MAP_SHARED,fd_,position);
        if(file_size_-position<size_mapping)
            size_mapping=file_size_-position;
        for(size_t i=0;i<size_mapping;++i)
        {
            if(line_counter==str_numer)
            {
                std::cout<<text[i];
            }
            if(text[i]=='\n'||(position+size_mapping)==file_size_)
                ++line_counter;
            if(line_counter==str_numer+1)
            {
                find=true;
                break;
                
            }
        }
        munmap(text,size_mapping_);
        if(find)
            break;  
        position+=size_mapping;   
    }
    if(!find)
        std::cout<<"Not that line"<<std::endl;

}

void Worker::Menu()
{
        std::cout<<"Menu"<<std::endl
        <<"---Find"<<std::endl
        <<"---Replace"<<std::endl
        <<"---Print"<<std::endl
        <<"------------------------------------------------------------------------------------------"<<std::endl;
}
