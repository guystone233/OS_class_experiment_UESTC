/*
实验内容：
1.有一群生产者任务在生产产品，并将这些产品提供给消费者任务去消费。
为使生产者任务与消费者进程能并发执行，在两者之间设置了一个具有n个缓冲区的缓冲池：
生产者任务从文件中读取一个数据，并将它存放到一个缓冲区中
消费者任务从一个缓冲区中取走数据，并输出此数据（printf）
生产者和消费者之间必须保持同步原则：不允许消费者任务到一个空缓冲区去取产品；
也不允许生产者任务向一个已装满产品且尚未被取走的缓冲区中投放产品
2.创建3个进程（或者线程）作为生产者任务，4个进程（或者线程）作为消费者任务
3.创建一个文件作为数据源，文件中事先写入一些内容作为数据（字符串或者数值）
4.生产者和消费者任务（进程或者线程）都具有相同的优先级
*/
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define N 5            // 缓冲区个数
pthread_mutex_t mutex; // 互斥锁，保证缓冲区的互斥访问
sem_t empty;           // 信号量，表示空缓冲区个数
sem_t full;            // 信号量，表示满缓冲区个数
char buffer[N][1000];  // 缓冲区

int line = 0;                       // 读取文件的行数
int in = 0, out = 0;                // 缓冲区的写入指针和读取指针
int producer_num[3] = {0, 1, 2};    // 生产者标号
int consumer_num[4] = {0, 1, 2, 3}; // 消费者标号

FILE *fp; // 文件指针
struct paras
{
    int *num;
    FILE *fp;
};

void producer(void *arg)
{
    while (1)
    {
        // 从文件中读取一个数据
        char data[1000];
        // 初始化参数
        struct paras *p = (struct paras *)arg;
        int *num = p->num;
        FILE *fp = p->fp; 
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        memset(data, 0, sizeof(data));
        if(fscanf(fp, "%s", data)==EOF) // 读取文件结束
        {
            fseek(fp, 0, SEEK_SET); // 从头开始读取文件
            fscanf(fp, "%s", data);
        }
        printf("                producer %d put %s into buffer[%d]\n", *num, data, in); // 为方便调试，输出数据
        memset(buffer[in], 0, sizeof(buffer[in]));                             // 清空缓冲区
        strcpy(buffer[in], data);
        in = (in + 1) % N; // 循环队列，in指向下一个空缓冲区
        sleep(1);          // 为方便调试，每次生产者生产数据后休眠1秒
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void consumer(void *arg)
{
    while (1)
    {
        // 从一个缓冲区中取走数据
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        printf("consumer %d get %s from buffer[%d]\n", *(int *)arg, buffer[out], out);
        out = (out + 1) % N; // 循环队列，out指向下一个存放数据的缓冲区
        sleep(1);            // 为方便调试，每次消费者消费数据后休眠1秒
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}
int main()
{
    // 打开文件
    FILE *fp = fopen("data.txt", "r");
    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);
    // 初始化信号量
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    // 初始化缓冲区
    for (int i = 0; i < N; i++)
    {
        memset(buffer[i], 0, sizeof(buffer[i]));
    }
    // 创建3个生产者线程
    pthread_t producers[3];
    for (int i = 0; i < 3; i++)
    {
        struct paras p;
        p.num = &producer_num[i];
        p.fp = fp;
        pthread_create(&producers[i], NULL, (void *)producer, (void *)&p);
    }
    // 创建4个消费者线程
    pthread_t consumers[4];
    for (int i = 0; i < 4; i++)
    {
        pthread_create(&consumers[i], NULL, (void *)consumer, (void *)&consumer_num[i]);
    }
    // 等待3个生产者线程结束
    for (int i = 0; i < 3; i++)
    {
        pthread_join(producers[i], NULL);
    }
    // 等待4个消费者线程结束
    for (int i = 0; i < 4; i++)
    {
        pthread_join(consumers[i], NULL);
    }
    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);
    // 销毁信号量
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}