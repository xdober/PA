#ifndef __REG_H__

#define __REG_H__

#include "common.h"
#include<stdlib.h>
#include<time.h>
enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
	union
	{
	  union
	  {
		uint32_t _32;
		uint16_t _16;
		uint8_t _8[2];
	  }gpr[8];

	/* Do NOT change the order of the GPRs' definitions. */
      struct 
	   {
	     uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
	   };
	};
	swaddr_t eip;
	struct 
	{
		unsigned int CF:1;
		unsigned int PF:1;
		unsigned int ZF:1;
		unsigned int SF:1;
		unsigned int IF:1;
		unsigned int DF:1;
		unsigned int OF:1;
	}eflags;
	struct
	{
	  struct
	  {
	    struct
	    {
	      unsigned int valid;
	      unsigned int tag;
	      uint8_t data[64];
	    }cache_block[8];
	  }cache_group[128];
	}cache;

} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

#define cache_index(addr) ((addr)&0x00001fc0)>>6
#define cache_tag(addr) ((addr)&0xffffe000)>>13
#define cache_offset(addr) ((addr)&0x0000003f)
static inline int hit(hwaddr_t addr,int* num)
 {
    int i;
    for(i=0;i<8;++i) 
    {
      if(cpu.cache.cache_group[cache_index(addr)].cache_block[i].tag==cache_tag(addr) && cpu.cache.cache_group[cache_index(addr)].cache_block[i].valid==1) 
      {
        *num=i;
        return 1;
      }
    }
    return 0;
}
static inline void cache_read(hwaddr_t addr,uint8_t* result)
{
   int num;
   if(hit(addr,&num))
   {
     int offset=cache_offset(addr);
     *result = cpu.cache.cache_group[cache_index(addr)].cache_block[num].data[offset];
   }
}
static inline void cache_write(hwaddr_t addr,uint8_t data)
{
  int num;
  if(hit(addr,&num))
  {
    int offset=cache_offset(addr);
    cpu.cache.cache_group[cache_index(addr)].cache_block[num].data[offset]=data;
  }
}
uint32_t dram_read(hwaddr_t, size_t);
static inline void cache_misspro(hwaddr_t addr)
{
   int num=-1;
   int i;
   hwaddr_t newaddr=addr&0xffffffc0;
   for(i=0;i<8;++i)
   {
      if(cpu.cache.cache_group[cache_index(addr)].cache_block[i].valid==0) num=i;
   }
   if(num==-1)
   {
     srand(time(NULL));
     num=rand()%8;
    }
    cpu.cache.cache_group[cache_index(addr)].cache_block[num].valid=1;
    cpu.cache.cache_group[cache_index(addr)].cache_block[num].tag=cache_tag(addr);
    for(i=0;i<64;++i)
      cpu.cache.cache_group[cache_index(addr)].cache_block[num].data[i]=dram_read(newaddr+i,1);
} 
extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif
