#include"cpu/exec/template-start.h"

#define instr jump

static void do_execute()
{
  if(ops_decoded.opcode==0xeb || ops_decoded.opcode==0xe9)
  {
	  DATA_TYPE_S src=op_src->val;
	  cpu.eip+=src;
	  if(DATA_BYTE==2) cpu.eip=cpu.eip&0x0000ffff;
  }
  else if(ops_decoded.opcode==0xff)
  {
	  
	  if(DATA_BYTE==2) 
	  {
		  int16_t src=op_src->val;
		  int len=decode_rm_w(cpu.eip+1);
		  cpu.eip=src&0x0000ffff;
		  cpu.eip-=(len+1);
	  }
	  else if(DATA_BYTE==4) 
	  {
		  int32_t src=op_src->val;
		  int len=decode_rm_l(cpu.eip+1);
		  cpu.eip=src-len-1;
	  }
  }
  print_asm_template1();
}

make_instr_helper(si)
make_instr_helper(rm)

#include"cpu/exec/template-end.h"
