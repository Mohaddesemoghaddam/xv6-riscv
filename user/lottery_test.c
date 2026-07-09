#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILD 3
#define RUN_TICKS 300

struct result {
  int tickets;
  int count;
};

int tickets[NCHILD] = {10, 20, 60};

int
main(int argc, char *argv[])
{
  int pipes[NCHILD][2];
  int pids[NCHILD];

  printf("lottery_test: starting lottery scheduling test\n");
  printf("lottery_test: expected CPU share follows ticket counts\n");

  for(int i = 0; i < NCHILD; i++){
    if(pipe(pipes[i]) < 0){
      printf("lottery_test: pipe failed\n");
      exit(1);
    }

    int pid = fork();

    if(pid < 0){
      printf("lottery_test: fork failed\n");
      exit(1);
    }

    if(pid == 0){
      struct result r;
      int start;
      int count = 0;
      int dummy = 0;

      close(pipes[i][0]);

      if(settickets(tickets[i]) < 0){
        printf("lottery_test: settickets failed\n");
        exit(1);
      }

      start = uptime();

      while(uptime() - start < RUN_TICKS){
        for(int j = 0; j < 1000; j++){
          dummy += j + i;
        }
        count++;
      }

      if(dummy == 0)
        printf("lottery_test: unexpected zero dummy\n");

      r.tickets = tickets[i];
      r.count = count;

      write(pipes[i][1], &r, sizeof(r));
      close(pipes[i][1]);
      exit(0);
    }

    pids[i] = pid;
    close(pipes[i][1]);
  }

  for(int i = 0; i < NCHILD; i++){
    wait(0);
  }

  printf("lottery_test: results\n");

  for(int i = 0; i < NCHILD; i++){
    struct result r;

    if(read(pipes[i][0], &r, sizeof(r)) != sizeof(r)){
      printf("lottery_test: failed to read result\n");
      exit(1);
    }

    close(pipes[i][0]);

    printf("lottery_test: tickets=%d count=%d\n", r.tickets, r.count);
  }

  printf("lottery_test: higher ticket count should usually produce higher work count\n");
  printf("lottery_test: finished\n");

  exit(0);
}
