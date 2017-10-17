#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();
extern void raise_intr(uint8_t NO,vaddr_t ret_addr);
make_EHelper(lidt) {
  //printf("in lidt: ");
  //printf("0x%08x   ",id_dest->val);
  
  //TODO();
  Log("cpu.eip: 0x%-8x", cpu.eip);
  Log("cpu.seq_eip: 0x%-8x", decoding.seq_eip);

  //48bit length value
  cpu.idtr.limit = vaddr_read(id_dest->addr,2);   // get 16bit limit
   
  uint32_t base = vaddr_read(id_dest->addr+2,4); // get 32bit base
  if(id_dest->width==2){ //if length = 16, get 24bit of value
    base = base & 0xffffff;
  }
  cpu.idtr.base = base;
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
  //seq_eip is the address of next instruction, seq != eip+1
  //eip is the current address 
  raise_intr(id_dest->val,decoding.seq_eip);
  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&cpu.eip);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.value);
  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();

  t0=pio_read(id_src->val,id_src->width);
  operand_write(id_dest,&t0);
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  //TODO();
  pio_write(id_dest->val,id_src->width,id_src->val);
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
