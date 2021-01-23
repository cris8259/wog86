#include"x86regs.h"
int x86_exec(x86regs * p_regs)
{
	x86regs x86regs=*p_regs;
	for(;;)
	{
		switch(*((uint32_t*)R_EIP))
		{}
	}
}
