// Lab Xv6 and Unix utilities
// pingpong.c
#include "kernel/types.h"
#include "user/user.h"
#include "stddef.h"

int main(int argc, char *argv[])
{
    int pd1[2], pd2[2];
    pipe(pd1);
    pipe(pd2);
    char buf[20];
    if (!fork()) {
        //child process
        read(pd1[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
        write(pd2[1], "pong", strlen("pong"));
    }
    else {
        //parent process
        write(pd1[1], "ping", strlen("ping"));
        wait(NULL);
        read(pd2[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
    }
    exit(0);
}
