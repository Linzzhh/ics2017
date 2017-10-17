#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
size_t get_ramdisk_size();
void ramdisk_read(void *buf,off_t offset, size_t len);
uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();

  size_t size=get_ramdisk_size();
 Log("here!!");
  ramdisk_read(DEFAULT_ENTRY,0,size);
 // printf("size :%d  \nproject: %d \n filename:%s\n",size,*as,filename);
 Log("here!!!");
  return (uintptr_t)DEFAULT_ENTRY;
}
