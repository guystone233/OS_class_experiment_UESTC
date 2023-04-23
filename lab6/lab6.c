/*
利用POSIX API（文件操作也可以使用ANSI C标准I/O库）编程实现cp –r命令，
支持将源路径（目录）中的所有文件和子目录，以及子目录中的所有内容，全部拷贝到目标路径（目录）中。
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
// 首先实现文件到文件的拷贝
int copyfile(char *spath, char *dpath)
{
    int sfd, dfd;
    if ((sfd = open(spath, O_RDONLY)) == -1) // 只读
    {
        printf("Source file does not exist!\n");
        return -1;
    }
    if (dfd = open(dpath, O_WRONLY) == -1) // 目标文件不存在则创建
    {
        if ((dfd = open(dpath, O_WRONLY | O_CREAT, 0644)) == -1) // 只写，不存在则创建
        {
            printf("Destination file create error!\n");
            return -1;
        }
    }
    else
    {
        printf("File %s already exists, overwrite it(1) or skip it(else)?\n", dpath);
        char choice;
        while ((choice = getchar()) == '\n')
            ;
        if (choice == '1')
        {
            if ((dfd = open(dpath, O_WRONLY | O_TRUNC, 0644)) == -1) // 覆盖文件
            {
                printf("Destination file create error!\n");
                return -1;
            }
        }
        else
            return 0; // 跳过
    }
    char buf[1024]; // 缓冲区
    int n;
    while ((n = read(sfd, buf, 1024)) > 0) // 读取源文件,到达文件尾部时返回0
    {
        if (n == -1 || (write(dfd, buf, n)) == -1) // 写入目标文件
        {
            printf("Error copying file from %s to %s\n", spath, dpath);
            return -1;
        }
    }
    close(sfd);
    close(dfd);
    return 0;
}
// 实现文件属性的判断
int isdir(char *path)
{
    struct stat buf;
    if (stat(path, &buf) == -1)
    {
        printf("Error getting file status!\n");
        return -1;
    }
    return S_ISDIR(buf.st_mode);
}
// 然后实现目录的遍历及复制
int walkdir(char *spath, char *dpath)
{
    DIR *sdir;
    struct dirent *sdp;
    if ((sdir = opendir(spath)) == NULL)
    {
        printf("Source directory does not exist!\n");
        return -1;
    }
    while ((sdp = readdir(sdir)) != NULL)
    {
        if (strcmp(sdp->d_name, ".") == 0 || strcmp(sdp->d_name, "..") == 0) // 忽略.和..
            continue;
        char sfile[1024], dfile[1024];
        // 拼接源文件和目标文件的路径
        strcpy(sfile, spath);
        strcat(sfile, "/");
        strcat(sfile, sdp->d_name);
        strcpy(dfile, dpath);
        strcat(dfile, "/");
        strcat(dfile, sdp->d_name);
        if (isdir(sfile))
        {
            // 如果目录不存在则创建
            DIR *temp = opendir(dfile);
            if (temp != NULL)
                closedir(temp);
            else if (mkdir(dfile, 0777) == -1) // 创建目录
            {
                printf("Error creating directory %s\n", dfile);
                return -1;
            }
            if (walkdir(sfile, dfile) == -1) // 递归遍历子目录
                return -1;
        }
        else
        {
            if (copyfile(sfile, dfile) == -1) // 拷贝文件
                return -1;
        }
    }
    closedir(sdir);
    return 0;
}
// 主函数实现参数读取
int main(int argc, char *argv[])
{
    
    if (argc != 3)
    {
        printf("Usage: %s source_path destination_path\n", argv[0]);
        return -1;
    }
    // 计算拷贝时间
    struct timeval start,end;
    gettimeofday(&start, NULL );
    if ((walkdir(argv[1], argv[2])) == 0)
        printf("Copy successfully!\n");
    else
    {
        printf("Copy failed!\n");
        return -1;
    }
    gettimeofday(&end, NULL );
    long timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
    printf("time = %ld us\n", timeuse);
    return 0;
}