#include "cpu/exec.h"

make_EHelper(test) {
 // TODO();

  rtl_and(&t2, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t2,id_dest->width); 
  
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  print_asm_template2(test);
}
 

make_EHelper(and) {
  //TODO();

  rtl_and(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width); 
 
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  print_asm_template2(and);
}



make_EHelper(xor) {
 // TODO();
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&t2,id_dest->width); 
  
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
 
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
  
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  print_asm_template2(or);
}

make_EHelper(sar) {
 // TODO();
  // unnecessary to update CF and OF in NEMU
  if(id_dest->width==1) {
    uint8_t utmp=id_dest->val;
    int8_t  tmp=utmp;
    id_dest->val = tmp;
  }else if(id_dest->width==2){
  
    uint16_t utmp=id_dest->val;
    int16_t  tmp=utmp;
    id_dest->val = tmp;
  }
  if(id_src->val==1){
  t0=1;
  rtl_and(&t1,&id_dest->val,&t0);
  rtl_sar(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_set_CF(&t1);
  }else{
  t0=id_src->val-1;
  rtl_sar(&t2,&id_dest->val,&t0);
  t0=1;
  rtl_and(&t1,&t2,&t0);
  rtl_sar(&t2,&t2,&t0);
  operand_write(id_dest,&t2);
  rtl_set_CF(&t1);
  }
  rtl_set_OF(&tzero);
/*
  rtl_sar(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  */
  print_asm_template2(sar);
}

make_EHelper(rol) {
  t0 = id_src->val % (id_dest->width << 3);
  t1 = (id_dest->width << 3) - t0;
  t2 = (id_dest->val << t0) | (id_dest->val >> t1);
  operand_write(id_dest, &t2);
  t2 = (t2 & 0x00000001) == 1;
  rtl_set_CF(&t2);

  print_asm_template2(rol);
}
make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  
  if(id_src->val==1)
  {
  rtl_msb(&t0,&id_dest->val,id_dest->width);
  rtl_shl(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_msb(&t1,&t2,id_dest->width);
  
  t2=0; 
  rtl_set_CF(&t0);
  if(t0==t1) rtl_set_OF(&t2);
  else {t2=1; rtl_set_OF(&t2);}
  }
  else if(id_src->val!=0){  
  t0=id_src->val-1;
  rtl_shl(&t2,&id_dest->val,&t0);
  rtl_msb(&t0,&t2,id_dest->width);
  rtl_set_CF(&t0);
  t0=1;
  rtl_shl(&t2,&t2,&t0);
  operand_write(id_dest,&t2);
  }
  rtl_get_CF(&t0);
  rtl_get_OF(&t1);/*
	rtl_shl(&t2, &id_dest->val, &id_src->val);
	  operand_write(id_dest, &t2);
	    rtl_update_ZFSF(&t2, id_dest->width);*/
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU

  if(id_src->val==1){
  t0=1;
  rtl_msb(&t3,&id_dest->val,id_dest->width);
  rtl_and(&t1,&id_dest->val,&t0);
  rtl_shr(&t2,&id_dest->val,&id_src->val);
  rtl_msb(&t0,&t2,id_dest->width);
  operand_write(id_dest,&t2);
  if(t0==t3) rtl_set_OF(&tzero);
  else {t2=1; rtl_set_OF(&t2);}
  rtl_set_CF(&t1);
  }else{
  t0=id_src->val-1;
  rtl_shr(&t2,&id_dest->val,&t0);
  t0=1;
  rtl_and(&t1,&t2,&t0);
  rtl_shr(&t2,&t2,&t0);
  operand_write(id_dest,&t2);
  rtl_set_CF(&t1);
  }/*
	rtl_shr(&t2, &id_dest->val, &id_src->val);
	  operand_write(id_dest, &t2);
	    rtl_update_ZFSF(&t2, id_dest->width);
*/
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

}

make_EHelper(not) {
  //TODO();
  rtl_not( &id_dest->val);
  operand_write(id_dest,&id_dest->val);
  print_asm_template1(not);
}
