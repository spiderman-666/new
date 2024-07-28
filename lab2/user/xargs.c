#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#define N 1024
int main(int argc, char *argv[])
{
    if(argc<2){
        fprintf(2,"Input Error");
        exit(1);
    }
    char *c[N];//存放子进程的参数
    int index=0;
    for(int i=1;i<argc;i++){
        c[index++]=argv[i];
    }
    char buffer[N]={"\0"};//缓冲区存放从管道读出的数据
    while(read(0,buffer,N)>0){
        char ch[N]={"\0"};
        c[index]=ch;
        for(int i=0;i<strlen(buffer);i++){
            if(buffer[i]=='\n'){//当遇到换行符时，创建子线程
                if(!fork()){
                    exec(argv[1],c);
                }
                wait(0);
            }
            else{
                ch[i]=buffer[i];
            }
        }
    }
    exit(0);
}