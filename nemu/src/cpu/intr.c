#include "cpu/exec.h"
#include "memory/mmu.h"
void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //cpu.idtr 
  //TODO();
  //push a point not a value
  rtl_push(&cpu.eflags.value);
  cpu.eflags.IF = 0; 
  rtl_push((rtlreg_t *)&cpu.cs);
  
  rtl_push(&ret_addr);
  //printf("in int :   0x%08x,~\n",cpu.idtr.base);
  //TODO();
  // each interupt has 8 bytes(2*32bit)
  uint32_t low_idtr = vaddr_read(cpu.idtr.base+NO*8,4);
  uint32_t high_idtr = vaddr_read(cpu.idtr.base+NO*8+4,4);
  uint32_t iaddr=(low_idtr&0xffff)+(high_idtr&0xffff0000);
  //Log("offset: 0x%-8x", iaddr);
  // jmp -> so need change jmp_eip  not EIP!!, and set is_jmp=1
  decoding.jmp_eip=iaddr;
  decoding.is_jmp = 1;
}

void dev_raise_intr() {
  cpu.INTR = true;
}
