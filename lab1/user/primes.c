#include "kernel/types.h"
#include "user/user.h"
#include "stddef.h"
void map(int x,int pd[])
{
    close(x);//关闭文件描述符,节约资源
    dup(pd[x]);//将管道的读或写端口复制到描述符x上
    close(pd[0]);//关闭管道描述符
    close(pd[1]);
}
void prime()
{
    int pd[2];
    int num1,num2;
    if(read(0,&num1,sizeof(int))){
        printf("prime %d\n",num1);
        pipe(pd);
        if(!fork()){
            map(1,pd);
            while(read(0,&num2,sizeof(int))){
                if(num2%num1!=0){
                    write(1,&num2,sizeof(int));// 写入管道
                }
            }
        }
        else{
            map(0,pd);//父进程将管道的读端口映射到描述符0
            prime();
        }
    }
}
int main(int argc, char *argv[])
{
    //定义描述符
    int pd[2];
    pipe(pd);
    if (!fork()){//子进程
        map(1,pd);//将管道的写端口映射到描述符1
        for(int i=2;i<=35;i++){
            write(1,&i,sizeof(int));
        }
    }
    else{
        map(0,pd);
        prime();
    }
    exit(0);
}