#include "cpu/exec.h"

make_EHelper(test) {
  TODO();

  print_asm_template2(test);
}

make_EHelper(and) {
  TODO();

  print_asm_template2(and);
}

make_EHelper(xor) {
 // TODO();
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width); 
  
  rtl_xor(&t0,&t0,&t0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
 
  /*
  t0=1;
  rtl_and(&t1,&t2,&t0);
  while(t2!=0){
  rtl_shr(&t2,&t2,&t0);
  rtl_and(&t3,&t2,&t0);
  rtl_xor(&t1,&t3,&t1);
  }
  rtl_not(&t1);
  rtl_set_PF(&t1);
  
  rtl_get_CF(&t0);
  rtl_get_SF(&t1);
  rtl_get_PF(&t2);
  rtl_get_OF(&t3);
  printf("CF: %d SF:%d PF:%d OF:%d\n",t0,t1,t2,t3);*/
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();

  rtl_or(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width); 
  
  rtl_xor(&t0,&t0,&t0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  print_asm_template2(or);
}

make_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
