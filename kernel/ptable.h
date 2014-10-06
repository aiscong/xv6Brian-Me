#ifndef _PTABLE_H_
#define _PTABLE_H_

#include "spinlock.h"
#include "param.h"
#include "proc.h"

struct ptable{
  struct spinlock lock;
  struct proc proc[NPROC];
};

extern struct ptable ptable;

#endif // _PTABLE_H_
