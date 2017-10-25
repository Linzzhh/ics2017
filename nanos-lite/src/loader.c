#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
size_t get_ramdisk_size();
//void ramdisk_read(void *buf,off_t offset, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern size_t fs_filesz(int fd);
uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();

  //size_t size=get_ramdisk_size();
  //ramdisk_read(DEFAULT_ENTRY,0,size);
  //filename = "/bin/text";
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  return (uintptr_t)DEFAULT_ENTRY;
}
