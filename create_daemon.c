#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

#define ERR_EXIT(m) \
    do {\
            perror(m);\
            exit(EXIT_FAILURE);\
        }\
    while(0);\

void create_daemon(void);

int main(void) {
    time_t t;
    int fd;
    create_daemon();
    while(1) {
        fd = open("test.txt", O_WRONLY|O_CREAT|O_APPEND, 0644);
        if (fd == -1) ERR_EXIT("open error");
        t = time(0);
        char *buf = asctime(localtime(&t));
        write(fd,buf,strlen(buf));
        close(fd);
        sleep(30);
    }
    return 0;
}

void create_daemon(void) {
    pid_t pid;
    pid = fork(); // 父子进程开始分岔，都从这句开始执行
    if (pid == -1) ERR_EXIT("fork error");
    if (pid > 0) exit(EXIT_SUCCESS); // 退出父进程（父进程fork返回值大于0，子进程fork返回0）
    if (setsid() == -1) ERR_EXIT("SETSID ERROR"); // 脱离父进程的会话和进程组，创建新的会话
    for (int i = 0; i < 3; ++i) { // 0标准输入 1标准输出 2标准错误
        close(i);
    };
    open("/dev/null", O_RDWR); // 因为open总是分配最小的文件描述符，且之前已经关闭了0，1，2描述符，所以三次open分别将0，1，2描述符重定向到/dev/null中
    dup(0); // 复制文件描述符（1） 并指向同一个文件
    dup(0); // 复制文件描述符（2） 并指向同一个文件
    umask(0); // 创建掩码从父进程继承，防止守护进程创建文件时权限出错，所以将守护进程的创建掩码置为0，使用open创建文件时，文件权限是（mode&~umask)
    return;
}