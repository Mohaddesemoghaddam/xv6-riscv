#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILD 3
#define RUN_TICKS 600

struct result {
  int tickets;
  int count;
};

int tickets[NCHILD] = {10, 20, 60};

int
main(int argc, char *argv[])
{
  int result_pipes[NCHILD][2];
  int start_pipes[NCHILD][2];

  printf("lottery_test: starting lottery scheduling test\n");
  printf("lottery_test: expected CPU share follows ticket counts\n");

#ifdef SCHED_LOTTERY
  printf("lottery_test: compiled with SCHED_LOTTERY\n");
#else
  printf("lottery_test: not compiled with SCHED_LOTTERY\n");
#endif

  for(int i = 0; i < NCHILD; i++){
    if(pipe(result_pipes[i]) < 0){
      printf("lottery_test: result pipe failed\n");
      exit(1);
    }

    if(pipe(start_pipes[i]) < 0){
      printf("lottery_test: start pipe failed\n");
      exit(1);
    }

    int pid = fork();

    if(pid < 0){
      printf("lottery_test: fork failed\n");
      exit(1);
    }

    if(pid == 0){
      struct result r;
      char start_signal;
      int start;
      int count = 0;
      int dummy = 0;

      close(result_pipes[i][0]);
      close(start_pipes[i][1]);

      if(settickets(tickets[i]) < 0){
        printf("lottery_test: settickets failed\n");
        exit(1);
      }

      // Wait until parent starts all children together.
      if(read(start_pipes[i][0], &start_signal, 1) != 1){
        printf("lottery_test: failed to receive start signal\n");
        exit(1);
      }

      close(start_pipes[i][0]);

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

      write(result_pipes[i][1], &r, sizeof(r));
      close(result_pipes[i][1]);
      exit(0);
    }

    close(result_pipes[i][1]);
    close(start_pipes[i][0]);
  }

  // Start all children after they have been created.
  for(int i = 0; i < NCHILD; i++){
    char start_signal = 's';
    write(start_pipes[i][1], &start_signal, 1);
    close(start_pipes[i][1]);
  }

  for(int i = 0; i < NCHILD; i++){
    wait(0);
  }

  printf("lottery_test: results\n");

  for(int i = 0; i < NCHILD; i++){
    struct result r;

    if(read(result_pipes[i][0], &r, sizeof(r)) != sizeof(r)){
      printf("lottery_test: failed to read result\n");
      exit(1);
    }

    close(result_pipes[i][0]);

    printf("lottery_test: tickets=%d count=%d\n", r.tickets, r.count);
  }

  printf("lottery_test: higher ticket count should usually produce higher work count\n");
  printf("lottery_test: finished\n");

  exit(0);
}
