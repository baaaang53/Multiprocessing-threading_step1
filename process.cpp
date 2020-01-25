#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>
#include <sys/wait.h>


#define MAX_N 4000000
#define process_num 4
// 여기에 작성해도 됨

using namespace std;
using namespace std::chrono;

void* create_shared_memory(size_t size) {

  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, visibility, -1, 0);
}


int main() {
    // array는 똑같이 복사되므로 shared memory에 올릴 필요 없음.
	int *arr;
	arr = new int[MAX_N];
	for(int i=0; i<MAX_N; i++)
		scanf("%d", arr + i);

	auto start = high_resolution_clock::now();
    
    int *ans;
    
    void* space = create_shared_memory(MAX_N); // shared memory 생성
    memcpy(space, &ans, sizeof(ans));
    
    
    // child process 생성
    pid_t pids[process_num], pid;
    int status;
    
    for (int i = 0; i < process_num ; i++) {
        pids[i] = fork();
        if (pids[i] == 0) //child가 child process를 생성하는 것을 방지
            break;
    }
    
    // child의 연산
    for(int i =0; i<process_num; i++) {
        if (pids[i] == 0) {
            int start = i * 1000000;
            int end = start + 1000000;
            int localsum = 0;
            for (int j = start; j < end; j++) {
                localsum += arr[j];
            }
            //shared memory에 있는 값을 불러오고, update
            localsum += *(int*)space;
            memcpy(space, &localsum, sizeof(localsum));
            
            _exit(0);
        }
    }
    
    //parent가 child 끝날때까지 기다림.
    pid = wait(&status);
    pid = wait(&status);
    pid = wait(&status);
    pid = wait(&status); 
    
    if (WIFEXITED(status)) {
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        printf("Time taken: %lld microseconds (%lld ms)\n", duration.count(), duration.count() / 1000);
        printf("Answer: %d\n", *(int*)space);
            
        
        space = NULL;
        delete[] arr;
        arr = NULL;
        return 0;
    }
    else {
        printf("%d가 이상하게 죽음 \n", pid);
    }
    

	
}
