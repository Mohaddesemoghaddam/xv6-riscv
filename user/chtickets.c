#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int tickets;

  if(argc != 2){
    printf("usage: chtickets tickets\n");
    exit(1);
  }

  tickets = atoi(argv[1]);

  if(tickets < 1){
    printf("chtickets: tickets must be at least 1\n");
    exit(1);
  }

  if(settickets(tickets) < 0){
    printf("chtickets: failed to set tickets\n");
    exit(1);
  }

  printf("chtickets: changed tickets to %d\n", tickets);
  exit(0);
}
