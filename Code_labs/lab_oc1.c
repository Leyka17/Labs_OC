#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


int main(int argc, const char**argv[])
{
    if(argv[1])
    {
    pid_t parent = fork();
    if(parent < 0)
    {
        write(STDIN_FILENO,"Error, sorry,problem with fork\n",30);
        return -1;
    }
    if(parent==0)
    {
        int file = open(argv[1],O_WRONLY|O_TRUNC| O_CREAT ,S_IRWXU);
        if(file==-1)
        {
            write(STDIN_FILENO,"Error, not open file\n",30);
            return -1; 
        }
        else
        {
        dup2(file,1);
        execl("/bin/sh","/bin/sh",NULL);

        exit(1);
        }
        
    }
    if(parent>0)
    {
        wait(NULL);
       
    }
    }
    else{
        write(STDIN_FILENO,"Error, sorry,no file name\n",30);
    }
    return 0;
}