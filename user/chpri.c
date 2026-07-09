#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  int priority;

  if(argc != 3){
    printf("usage: chpri pid priority\n");
    exit(1);
  }

  pid = atoi(argv[1]);
  priority = atoi(argv[2]);

  if(priority < 0 || priority > 100){
    printf("chpri: priority must be between 0 and 100\n");
    exit(1);
  }

  if(setpriority(pid, priority) < 0){
    printf("chpri: failed to set priority for pid %d\n", pid);
    exit(1);
  }

  printf("chpri: changed priority of pid %d to %d\n", pid, priority);
  exit(0);
}
