#ifndef _PINFO_H_
#define _PINFO_H_

#include "param.h"

#define PINFO_NAME_LEN 16

struct pinfo {
  int inuse[NPROC];
  int pid[NPROC];
  int state[NPROC];
  int priority[NPROC];
  int tickets[NPROC];
  char name[NPROC][PINFO_NAME_LEN];
};

#endif
