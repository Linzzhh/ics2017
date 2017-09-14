#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char exp[256];
  uint32_t value;

} WP;
int record_watch(char *);
int exam_watch();
void delete_watch(int i);
void print_watch();
#endif
