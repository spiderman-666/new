#include "kernel/types.h"
#include "user/user.h"
int 
main(int argc, char *argv[])
{
    if(argc!=2){
        fprintf(2,"Error:Sleep Error\n");
        exit(1);//异常退出
    }
    //把字符串型参数转换为整型
    int number=atoi(argv[1]);
    sleep(number);
    exit(0);//正常退出
}