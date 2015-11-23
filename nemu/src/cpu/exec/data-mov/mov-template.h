#include "cpu/exec/template-start.h"

#define instr mov

static void do_execute() {
        
	if(ops_decoded.opcode==0x8e) 
	{
	  if(op_dest->reg==0)
	  {
	    cpu.ES.seg_selector=op_src->val;
	  }
	  else if(op_dest->reg==1)
	  {
	    cpu.CS.seg_selector=op_src->val;
	  }
	  else if(op_dest->reg==2)
	  {
	    cpu.SS.seg_selector=op_src->val;
	  }
	  else if(op_dest->reg==3)
	  {
	    cpu.DS.seg_selector=op_src->val;
	  }
	}   
	else 
	{
	  OPERAND_W(op_dest, op_src->val);
	  print_asm_template2();
	}  
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX),2);
	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr,2);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#include "cpu/exec/template-end.h"
