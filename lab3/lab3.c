/*
实验内容：
父进程首先使用系统调用pipe()建立一个管道，然后使用系统调用fork() 创建子进程1
子进程1关闭管道读文件
子进程1通过文件I/O操作向管道写文件写一句话（向文件中写入字符串）：Child process 1 is sending a message!
然后子进程1调用exit（）结束运行
父进程再次使用系统调用fork() 创建子进程2
子进程2关闭管道读文件
子进程2通过文件I/O操作向管道写文件写一句话（向文件中写入字符串）：Child process 2 is sending a message!
然后子进程2调用exit（）结束运行
父进程关闭管道写文件
父进程通过文件I/O操作从管道读文件中读出来自于两个子进程的信息，通过printf语句打印输出在屏幕上
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h> 

#define BUFSIZE 100
#define MAX_PROCESS_NUM 2
int process_num = 0;

int main(void)
{
    char bufin[BUFSIZE] = "empty";

    int bytesin;
    pid_t childpid[MAX_PROCESS_NUM];
    int fd[2];          // fd[0] 读数据, fd[1] 写数据
    if (pipe(fd) == -1) // 创建管道
    {
        perror("Failed to create the pipe");
        return 1;
    }
    bytesin = strlen(bufin);
    for (int i = 0; i < MAX_PROCESS_NUM; i++)
    {
        process_num++; // 子进程标号
        childpid[i] = fork(); // 创建子进程
        switch (childpid[i])
        {
        case -1:
            perror("Failed to fork");
            return 1;
        case 0:
        {
            char bufout[] = "Child process %d is sending a message!\n"; // 子进程写入的字符串
            sprintf(bufout, bufout, process_num);
            write(fd[1], bufout, strlen(bufout) + 1); // 写入管道
            printf("Child process %d is sending a message!\n", process_num);
            exit(0); // 子进程退出
        }
        default:
            // 等待子进程i结束
            waitpid(childpid[i], NULL, 0);
            break;
        }
    }
    printf("Parent process is reading from pipe...\n");
    bytesin = read(fd[0], bufin, BUFSIZE);
    for(int i = 0; i < BUFSIZE; i++)
    {
        printf("%c", bufin[i]); // 读取管道，由于字符串在bufin中连续存储，不能直接printf(bufin)，否则只会输出第一个字符串
    }
    return 0;
}
