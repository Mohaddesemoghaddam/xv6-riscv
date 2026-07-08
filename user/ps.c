#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/pinfo.h"
#include "user/user.h"

char*
state_name(int state)
{
  switch(state){
  case 0:
    return "UNUSED";
  case 1:
    return "USED";
  case 2:
    return "SLEEPING";
  case 3:
    return "RUNNABLE";
  case 4:
    return "RUNNING";
  case 5:
    return "ZOMBIE";
  default:
    return "UNKNOWN";
  }
}

int
main(int argc, char *argv[])
{
  struct pinfo info;

  if(getpinfo(&info) < 0){
    printf("ps: getpinfo failed\n");
    exit(1);
  }

  printf("PID\tSTATE\t\tPRIO\tTICKETS\tNAME\n");

  for(int i = 0; i < NPROC; i++){
    if(info.inuse[i]){
      printf("%d\t%s\t%d\t%d\t%s\n",
             info.pid[i],
             state_name(info.state[i]),
             info.priority[i],
             info.tickets[i],
             info.name[i]);
    }
  }

  exit(0);
}
