#include <iostream>
#include <chrono>
#include <pthread.h>
#include <stdlib.h>

#define MAX_N 4000000
#define thread_num 4
#define segmentsize MAX_N / thread_num

using namespace std;
using namespace std::chrono;
pthread_mutex_t mutex1;
int totalsum = 0;
int *arr;


// 여기에 작성해도 됨
void *threading(void *tid) {
    int i, begin, end, *threadnum;
    int localsum = 0;
    
    threadnum = (int *) tid;
    begin = (*threadnum * segmentsize);
    end = begin + segmentsize;
    printf("%d 번째 thread는 %d부터 %d까지 계산중! \n", *threadnum, begin, end-1);

    //calculate local sum
    for (i = begin; i < end ; i++) {
        localsum += arr[i];
    }
    
    printf("%d의 결과는 %d\n", *threadnum, localsum);

    // update global
    pthread_mutex_lock (&mutex1);
    totalsum += localsum;
    pthread_mutex_unlock (&mutex1);
    pthread_exit(NULL);


}

int main() {
    arr = new int[MAX_N];

	for(int i=0; i<MAX_N; i++)
		scanf("%d", arr + i);

	auto start = high_resolution_clock::now();

    // thread generate & setup
    int tids[thread_num];
    pthread_t threads[thread_num];
    pthread_mutex_init(&mutex1, NULL);
    for (int i = 0; i < thread_num; i++) {
        tids[i] = i;
    }
    
    // threading
    if (pthread_create(&(threads[0]), NULL ,threading, &tids[0]) != 0)
        perror("0번 스레드 생성 실패 ");
    if (pthread_create(&(threads[1]), NULL ,threading, &tids[1]) != 0)
        perror("1번 스레드 생성 실패 ");
    if (pthread_create(&(threads[2]), NULL ,threading, &tids[2]) != 0)
        perror("2번 스레드 생성 실패 ");
    if (pthread_create(&(threads[3]), NULL ,threading, &tids[3]) != 0)
        perror("3번 스레드 생성 실패 ");

    //join
    for (int i = 0; i<thread_num; i++) {
        if (pthread_join(threads[i], NULL) != 0)
            perror("스레드 조인 실패 ");
    }

    // timer, print result
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	printf("Time taken: %lld microseconds (%lld ms)\n", duration.count(), duration.count() / 1000);
	printf("Answer: %d\n", totalsum);

    // destroy
	delete[] arr;
	arr = NULL;
    pthread_mutex_destroy(&mutex1);
    pthread_exit(NULL);
	return 0;
}
