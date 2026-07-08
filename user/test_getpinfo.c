#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/pinfo.h"
#include "user/user.h"

static struct pinfo before;
static struct pinfo after;

int
count_used(struct pinfo *info)
{
  int count = 0;

  for(int i = 0; i < NPROC; i++){
    if(info->inuse[i])
      count++;
  }

  return count;
}

int
find_pid(struct pinfo *info, int pid)
{
  for(int i = 0; i < NPROC; i++){
    if(info->inuse[i] && info->pid[i] == pid)
      return i;
  }

  return -1;
}

int
main(int argc, char *argv[])
{
  int child_index;

  printf("test_getpinfo: starting getpinfo syscall test\n");

  if(getpinfo(&before) < 0){
    printf("test_getpinfo: FAILED - getpinfo returned error\n");
    exit(1);
  }

  printf("test_getpinfo: active processes before fork = %d\n",
         count_used(&before));

  int pid = fork();

  if(pid < 0){
    printf("test_getpinfo: FAILED - fork failed\n");
    exit(1);
  }

  if(pid == 0){
    while(1)
      ;
  }

  for(volatile int i = 0; i < 1000000; i++)
    ;

  if(getpinfo(&after) < 0){
    printf("test_getpinfo: FAILED - getpinfo after fork failed\n");
    kill(pid);
    wait(0);
    exit(1);
  }

  child_index = find_pid(&after, pid);

  if(child_index < 0){
    printf("test_getpinfo: FAILED - child pid %d not found in pinfo\n", pid);
    kill(pid);
    wait(0);
    exit(1);
  }

  printf("test_getpinfo: child found successfully\n");
  printf("test_getpinfo: pid=%d state=%d priority=%d tickets=%d name=%s\n",
         after.pid[child_index],
         after.state[child_index],
         after.priority[child_index],
         after.tickets[child_index],
         after.name[child_index]);

  if(after.priority[child_index] != 50){
    printf("test_getpinfo: FAILED - default priority is not 50\n");
    kill(pid);
    wait(0);
    exit(1);
  }

  if(after.tickets[child_index] != 1){
    printf("test_getpinfo: FAILED - default tickets is not 1\n");
    kill(pid);
    wait(0);
    exit(1);
  }

  kill(pid);
  wait(0);

  printf("test_getpinfo: PASSED\n");
  exit(0);
}
