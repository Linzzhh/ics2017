#include "cpu/exec.h"
#include "memory/mmu.h"
void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //cpu.idtr 
  //TODO();
  rtl_push(&cpu.eflags.value);
  rtl_push(&cpu.cs);
  //rtlreg_t* next_eip=cpu.eip;
  rtl_push(&cpu.eip);
  //printf("in int :   0x%08x,~\n",cpu.idtr.base);
  //TODO();
  uint32_t low_idtr = vaddr_read(cpu.idtr.base+NO,4);
  uint32_t high_idtr = vaddr_read(cpu.idtr.base+NO+4,4);
  uint32_t iaddr=(uint32_t)(low_idtr&0xffff)+(uint32_t)(high_idtr&0xffff0000);
  cpu.eip=iaddr;
}

void dev_raise_intr() {
}
