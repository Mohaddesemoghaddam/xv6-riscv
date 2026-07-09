#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILD 8
#define START_DELAY 5000000UL
#define WORK 20000000UL

int priorities[NCHILD] = {5, 5, 5, 5, 80, 80, 80, 80};

unsigned long
busy_work(int id)
{
  unsigned long x = 0;

  for(unsigned long i = 0; i < WORK; i++){
    x += i + id;
  }

  return x;
}

unsigned long
start_delay(void)
{
  unsigned long x = 0;

  for(unsigned long i = 0; i < START_DELAY; i++){
    x += i;
  }

  return x;
}

int
find_child(int pids[], int pid)
{
  for(int i = 0; i < NCHILD; i++){
    if(pids[i] == pid)
      return i;
  }

  return -1;
}

int
main(int argc, char *argv[])
{
  int pids[NCHILD];

  printf("priority_test: starting priority scheduling test\n");
  printf("priority_test: lower numeric value means higher priority\n");

  for(int i = 0; i < NCHILD; i++){
    int pid = fork();

    if(pid < 0){
      printf("priority_test: fork failed\n");
      exit(1);
    }

    if(pid == 0){
      unsigned long delay_result = start_delay();
      unsigned long work_result = busy_work(i);
      unsigned long result = delay_result + work_result;

      if(result == 0)
        printf("priority_test: unexpected zero result\n");

      exit(0);
    }

    pids[i] = pid;
  }

  for(int i = 0; i < NCHILD; i++){
    if(setpriority(pids[i], priorities[i]) < 0){
      printf("priority_test: failed to set priority for pid %d\n", pids[i]);
      exit(1);
    }

    printf("priority_test: pid=%d priority=%d\n", pids[i], priorities[i]);
  }

  printf("priority_test: waiting for children to finish\n");
  printf("priority_test: expected high-priority children to finish earlier\n");

  for(int order = 1; order <= NCHILD; order++){
    int pid = wait(0);
    int index = find_child(pids, pid);

    if(index < 0){
      printf("priority_test: unknown child pid %d finished\n", pid);
    } else {
      printf("priority_test: finish order %d -> pid=%d priority=%d\n",
             order, pid, priorities[index]);
    }
  }

  printf("priority_test: finished\n");
  exit(0);
}
