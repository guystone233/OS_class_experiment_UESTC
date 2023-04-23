/*
在linux系统下实现教材2.5.2节中所描述的哲学家就餐问题。
要求显示出每个哲学家的工作状态，如吃饭，思考。连续运行30次以上都未出现死锁现象。
*/
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
sem_t chopsticks[5];
sem_t room;
int philosopher_num[5] = {0, 1, 2, 3, 4};
/*
哲学家的实现：
1.开始思考（打印一个信息）
2.思考一段时间（sleep 1s）
3.饿了（打印一个信息）
4.循环尝试拿起两边筷子（四位哲学家竞争五只筷子，剩下一位必须等待，以防止死锁）
5.开始进餐（打印一个信息）
6.进餐一段时间（sleep 1s）
7.放下筷子，重复1-7
*/
void philosopher(void *arg)
{
    int id = *(int *)arg;
    while (1)
    {
        printf("philosopher %d is thinking\n", id);
        sleep(1);
        printf("philosopher %d is hungry\n", id);
        while (1)
        {
            sem_wait(&room);
            sem_wait(&chopsticks[id]);
            sem_wait(&chopsticks[(id + 1) % 5]);
            printf("philosopher %d is eating\n", id);
            sleep(1);
            sem_post(&chopsticks[id]);
            sem_post(&chopsticks[(id + 1) % 5]);
            sem_post(&room);
            break;
        }
    }
}
int main()
{
    // 初始化信号量
    sem_init(&room, 0, 4);
    for (int i = 0; i < 5; i++)
    {
        sem_init(&chopsticks[i], 0, 1);
    }
    // 创建5个哲学家线程
    pthread_t philosophers[5];
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&philosophers[i], NULL, (void *)philosopher, (void *)&philosopher_num[i]);
    }
    // 等待5个哲学家线程结束
    for (int i = 0; i < 5; i++)
    {
        pthread_join(philosophers[i], NULL);
    }
    // 销毁信号量
    sem_destroy(&room);
    for (int i = 0; i < 5; i++)
    {
        sem_destroy(&chopsticks[i]);
    }
    return 0;
}
