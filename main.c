#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_THREAD_NUM 100               //最大线程数
#define MAX_RANGE      ((long)2<<31)    //最大累加范围
#define START_ADD      1                //累加起始数
#define INITIAL_SUM    0                //初始化总和
#define INITIAL_RANGE  0                //初始化累加范围
#define INPUT_FILE     "input.txt"      //输入文件
#define OUTPUT_FILE    "output.txt"     //输出文件
#define TIME_FILE      "timetest.csv"   //保存时间测试结果的文件

void *thread_counter(void *arg);

pthread_mutex_t sum_mutex;  //互斥量
long long sum   = INITIAL_SUM;   //总和
long long cur   = START_ADD;      //下一个被累加的数值
long long range = INITIAL_RANGE;  //累加范围

int main(void) {
    FILE *fp;
    pthread_t threads[MAX_THREAD_NUM];  //存储线程id号
    int thread_num;
    void* thread_return;    //线程返回值
    struct timeval start;   //记录开始测试的时间
    struct timeval end;     //记录结束测试的时间

    //读取输入文件并做相关错误处理
    if ((fp = fopen(INPUT_FILE, "r")) == NULL) {
        fprintf(stderr, "ERROR: open %s failed.\n", INPUT_FILE);
        exit(EXIT_FAILURE);
    }
    rewind(fp);
    if (fscanf(fp, " N = %d", &thread_num) != 1) {
        fprintf(stderr, "ERROR: read N failed.\n");
        exit(EXIT_FAILURE);
    } else if (fscanf(fp, " M = %lld", &range) != 1) {
        fprintf(stderr, "ERROR: read M failed.\n");
        exit(EXIT_FAILURE);
    } else if (range > MAX_RANGE || thread_num > MAX_THREAD_NUM) {
        fprintf(stderr, "ERROR: invalid args N=%d M=%lld\n", thread_num, range);
        exit(EXIT_FAILURE);
    } else {
        printf("args N=%d M=%lld\n", thread_num, range);
    }
    fclose(fp);

    //initial mutex
    if (pthread_mutex_init(&sum_mutex, NULL) != 0) {
        fprintf(stderr, "ERROR: mutex initialization failed.\n");
        exit(EXIT_FAILURE);
    }
    //记录开始测试的时间
    gettimeofday(&start, NULL);
    //创建线程
    for (int i = 0; i < thread_num; i++) {
        if (pthread_create(&threads[i], NULL, thread_counter, NULL) != 0) {
            fprintf(stderr, "ERROR: thread %d creation failed.\n", i);
            exit(EXIT_FAILURE);
        }
    }
    //等待线程结束
    for (int i = 0; i < thread_num; i++) {
        if (pthread_join(threads[i], &thread_return) != 0) {
            fprintf(stderr, "ERROR: thread %d join failed\n", i);
        } else {
            printf("thread %d exit.\n", i);
        }
    }
    //记录结束测试的时间
    gettimeofday(&end, NULL);
    printf("All done. sum is %lld\n", sum);

    //输出计算结果
    if ((fp = fopen(OUTPUT_FILE, "w")) == NULL) {
        fprintf(stderr, "ERROR: open %s failed\n", OUTPUT_FILE);
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%lld", sum);
    fflush(fp);
    fclose(fp);

    //输出时间测试结果
    if ((fp = fopen(TIME_FILE, "a")) == NULL) {
        fprintf(stderr, "ERROR: open %s failed\n", TIME_FILE);
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%d,%lld,%ld\n", thread_num, range, ((end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * (1<<6)));
    fflush(fp);
    fclose(fp);

    return 0;
}

//线程工作函数
void *thread_counter(void *arg) {
    while (1) {
        //加锁
        pthread_mutex_lock(&sum_mutex);
        if (cur <= range) {
            //累加当前值
            sum += cur;
            cur++;
            printf("now sum is %lld\n", sum);
        } else {
            //累加已完成，解锁并退出
            pthread_mutex_unlock(&sum_mutex);
            break;
        }
        //解锁
        pthread_mutex_unlock(&sum_mutex);
    }
    pthread_exit(EXIT_SUCCESS);
}
