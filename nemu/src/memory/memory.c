#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int map_NO=is_mmio(addr);
  if(map_NO!=-1) {
    return mmio_read(addr,len,map_NO);
  }else
  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int map_NO=is_mmio(addr);
  if(map_NO!=-1) {
     mmio_write(addr,len,data,map_NO);
  }else
  memcpy(guest_to_host(addr), &data, len);
}

/*
paddr_t page_translate(vaddr_t addr, bool is_write) {
  if (cpu.PG == 1) {
	paddr_t page_dir_base = cpu.cr3;
	paddr_t page_dir_off = (addr >> 22) << 2;
	paddr_t pde_addr = page_dir_base + page_dir_off;
	uint32_t pde = paddr_read(pde_addr, 4);//(pde)page_tab_base with 3bits U/S R/W P
	if ((pde & 0x1) == 0) {//if page_tab present
	  Log("pde_base: 0x%-8x", page_dir_base);
	  Log("pde: 0x%-8x", pde);
	  Assert(0, "vaddr: 0x%-8x. The present bit of pde is wrong!!!", addr);
	}
	paddr_t page_tab_base = pde & 0xfffff000;//clear low 3bits,then get the page_tab_base
	paddr_t page_tab_off  = (addr & 0x003ff000) >> 10; //equal to >>12 <<2
	paddr_t pte_addr = page_tab_base + page_tab_off; 
	uint32_t pte = paddr_read(pte_addr, 4);//(pte)page_base with 3bits U/S R/W P
	if ((pte & 0x1) == 0) {//if page present
	  Log("pde: 0x%-8x", pde);
	  Log("pte: 0x%-8x", pte);
	  Assert(0, "The present bit of pte is wrong!!!");
	}
	paddr_t page_base = pte & 0xfffff000;//clear low 3bits,then get the page_base
	pte |= 0x40;
	if (is_write)
		pte |= 0x20;
	paddr_write(pte_addr, 4, pte);
	return page_base + (addr & 0x00000fff);
  } 
	return addr;
}
*/

paddr_t page_translate(vaddr_t addr, bool is_write) {
  if (cpu.PG == 1) {
	paddr_t pde_base = cpu.cr3;
	paddr_t pde_addr = pde_base + ((addr >> 22) << 2);
	uint32_t pde = paddr_read(pde_addr, 4);
	if ((pde & 0x1) == 0) {
	  Log("pde_base: 0x%-8x", pde_base);
	  Log("pde: 0x%-8x", pde);
	  Assert(0, "vaddr: 0x%-8x. The present bit of pde is wrong!!!", addr);
	}
	paddr_t pte_base = pde & 0xfffff000;
	paddr_t pte_addr = pte_base + ((addr & 0x003ff000) >> 10);
	uint32_t pte = paddr_read(pte_addr, 4);
	if ((pte & 0x1) == 0) {
	  Log("addr: 0x%-8x", addr);

	  Log("pde: 0x%-8x", pde);
	  Log("pte: 0x%-8x", pte);
	  Assert(0, "The present bit of pte is wrong!!!");
	}
	paddr_t page_base = pte & 0xfffff000;
	pte |= 0x20;
	if (is_write)
		pte |= 0x40;
	paddr_write(pte_addr, 4, pte);
	return page_base + (addr & 0x00000fff);
  } else {
	return addr;
  }
}

uint32_t vaddr_read(vaddr_t addr, int len) {

  //if addr+len goto next page and not the begin of page
  if ((((addr ^ (addr + len)) & 0x003ff000) != 0) && (((addr + len) & 0x00000fff) != 0)) { 
    int i;
    for (i = 1; i < 4; i++)
      if (((addr + i) & 0x00000fff) == 0){  //if 4byte data cross two pages
	paddr_t paddr = page_translate(addr, false);//get lowbyte from first page
	uint32_t low = paddr_read(paddr, i);
	paddr = page_translate(addr+i, false);//get highbyte from second page
	uint32_t high = paddr_read(paddr, len-i);
	return (high << (i << 3)) + low;//join them
      }
     Log("addr: 0x%-8x len: %d", addr, len);
     Assert(0, "Data cross the page boudary!!!");
  }
   else {
     paddr_t paddr = page_translate(addr, false);
     return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  //if addr+len goto next page and not the begin of page
  if ((((addr ^ (addr + len)) & 0x003ff000) != 0) && (((addr + len) & 0x00000fff) != 0)) { 
    int i;
    for (i = 1; i < 4; i++)
      if (((addr + i) & 0x00000fff) == 0){  //if 4byte data cross two pages
	paddr_t paddr = page_translate(addr, true);//get lowbyte from first page
	paddr_write(paddr, i, (data & (~0u >> ((4 - i) << 3))));
	paddr = page_translate(addr+i, true);//get highbyte from second page
	paddr_write(paddr, len - i, (data >> (i << 3)));
      }
     Log("addr: 0x%-8x len: %d", addr, len);
     Assert(0, "Data cross the page boudary!!!");
  }
   else {
     paddr_t paddr = page_translate(addr, true);
     paddr_write(paddr, len, data);
  }
}
