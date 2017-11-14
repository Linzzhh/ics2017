#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  int pde_index = PDX(va);//get index in page_dir_table;
  int pte_index = PTX(va);//get index in page_tab_table;
  PDE * updir = (PDE *)(p->ptr);//get the virtual address space
  PTE * uptab;
  if ((updir[pde_index] & 0x1) == 0){//if page_tab_table is not present,create it
    uptab = (PTE *)(void *)(palloc_f());//create a page_tab_table
    updir[pde_index] = (uintptr_t)uptab | PTE_P;// add it into page_dir_table ,set present = 1
  } 
  else {//if present,get from dir_table
    uptab = (PTE *)((uintptr_t)updir[pde_index] & 0xfffff000);//clear low 3bits
  }
  PTE pte = ((uintptr_t)pa & 0xfffff000) | PTE_P; //clear physical page low 3bits
  uptab[pte_index] = pte;//add pa into page_tab_table; 
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]){
  // decompile start.c get asm code
  // 08048000 <_start>:
  // 8048000:   55                      push   %ebp
  // 8048001:   89 e5                   mov    %esp,%ebp
  // 8048003:   83 ec 0c                sub    $0xc,%esp 
  // 8048006:   ff 75 10                pushl  0x10(%ebp)
  // 8048009:   ff 75 0c                pushl  0xc(%ebp)
  // 804800c:   ff 75 08                pushl  0x8(%ebp)   
  // 804800f:   e8 60 00 00 00          call   8048074 <main> 

  uint32_t *ptr = ustack.end;
  
  for (int i = 0; i < 8; i++) //push + push*3(sub -12) + pushl*3 +call(push addr) total 8 push
    *ptr-- = 0x0; 
  //order refer from am/include/proc.h
  //push eflags
  // *ptr-- = 0x2; 
  *ptr-- = 0x202;//open IF 
  //push cs
  *ptr-- = 0x8; 
  //push eip
  *ptr-- = (uint32_t)entry;
  //push error code
  *ptr-- = 0x0; 
  //push irq id
  *ptr-- = 0x81; 
  //push 8 general registers
  for (int i = 0; i < 8; i++)
	*ptr-- = 0x0;
  ptr++;
  return (_RegSet *)ptr;
}
