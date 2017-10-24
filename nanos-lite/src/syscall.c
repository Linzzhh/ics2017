#include "common.h"
#include "syscall.h"

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  
  switch (a[0]) {
    case SYS_none: SYSCALL_ARG1(r)=1;break;
    case SYS_exit: _halt(a[1]);break;
    case SYS_write:  
	if(a[1]==1||a[1]==2){ //a[1] is fd  if stdout or stderr
	  // _putc((a[2]));
	   char * buf =  (char *)a[2];
	   int size = a[3];
	   for(int i = 0; i < size; i++){ 
	     _putc(buf[i]);
	   }
	}
        SYSCALL_ARG1(r)= a[3];
	//a[0] = 1;
	break;
    case SYS_brk: SYSCALL_ARG1(r)=0;break;
    default: panic("Unhandled syscall ID = %d", a[0]);break;
  }

  return r;
}
