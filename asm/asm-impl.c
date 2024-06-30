#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b) {
  int64_t ans=0;
  asm (
  	"addq %%rbx,%%rax;"
  	:"=a"(ans)
  	:"a"(a),"b"(b)
  	:
  );
  return ans;
}

int asm_popcnt(uint64_t x) {
  uint64_t tmp=0;
  uint64_t t1 = 0x5555555555555555, t2 = 0x3333333333333333, t3 = 0x0F0F0F0F0F0F0F0F, 
    	   t4 = 0x00FF00FF00FF00FF, t5 = 0x0000FFFF0000FFFF, t6 = 0x00000000FFFFFFFF;
  asm(
  	"movq %1, %0;"
  	"shrq $1, %1;"
  	"andq %2, %1;" 
  	"andq %2, %0;"
  	"addq %1, %0;" // s & t1 + (s>>1) & t1
  	
  	"movq %0, %1;" //move ans to tmp
  	"shrq $2, %1;"
  	"andq %3, %1;"
  	"andq %3, %0;"
  	"addq %1, %0;" // s & t2 + (s>>2) & t2
  	
  	"movq %0, %1;"
  	"shrq $4, %1;"
  	"andq %4, %1;"
  	"andq %4, %0;"
  	"addq %1, %0;" // s & t3 + (s>>4) & t3
  	
  	"movq %0, %1;"
  	"shrq $8, %1;"
  	"andq %5, %1;"
  	"andq %5, %0;"
  	"addq %1, %0;" // s & t4 + (s>>8) & t4
  	
  	"movq %0, %1;"
  	"shrq $16,%1;"
  	"andq %6, %1;"
  	"andq %6, %0;"
  	"addq %1, %0;" // s & t5 + (s>>16) & t5
  	
  	"movq %0, %1;"
  	"shrq $32,%1;"
  	"andq %7, %1;"
  	"andq %7, %0;"
  	"addq %1, %0;"
  	//"movq %0, %1;" // s & t6 + (s>>32) & t6

  	:"=r"(tmp)
  	:"r"(x),"m"(t1),"m"(t2),"m"(t3),"m"(t4),"m"(t5),"m"(t6)

  );
  return tmp;
}

void *asm_memcpy(void *dest, const void *src, size_t n) {
  size_t i=0;
  asm(
  	"movq $0, %3;"
  	"1:;"
  	"movb (%0, %3), %%al;"
  	"movb %%al, (%1, %3);"
  	"addq $1, %3;"
  	"cmp %3, %2;"
  	"jg 1b;"
  	:
  	:"r"(src),"r"(dest),"r"(n),"r"(i)
  	:"rax"
  );
  return dest;
}


/*
	my_jmp_buf寄存器存放顺序如下：
	rbx 0
	rbp 8
	r12 16
	r13 24
	r14 32
	r15 40
	rsp 48
	pc  56
*/



int asm_setjmp(asm_jmp_buf env) {
  asm(
  	"movq %%rbx, 0(%%rdi);"

  	"movq %%r12, 16(%%rdi);"
  	"movq %%r13, 24(%%rdi);"
  	"movq %%r14, 32(%%rdi);"
  	"movq %%r15, 40(%%rdi);"
  	
  	//存pc
  	"movq (%%rsp), %%rdx;"
  	"movq %%rdx, 56(%%rdi);"
  	
  	//rbp
  	"movq %%rbp, 8(%%rdi);"
  	
  	//rsp
  	"leaq 8(%%rsp), %%rdx;"
  	"movq %%rdx, 48(%%rdi);"
  	:::
  );
  return 0;
}



void asm_longjmp(asm_jmp_buf env, int val) {
  asm(
  
	"movq %%rsi, %%rax;"
	
  	"movq 0(%%rdi), %%rbx;"
  	
  	"movq 16(%%rdi), %%r12;"
  	"movq 24(%%rdi), %%r13;"
  	"movq 32(%%rdi), %%r14;"
  	"movq 40(%%rdi), %%r15;"
  	
  	//恢复rbp
  	"movq 8(%%rdi), %%rbp;"
  	
  	"movq 56(%%rdi), %%rdx;"
  	
  	"movq 48(%%rdi), %%rsp;"
  	"jmp *%%rdx;"
  	:::
  );
}


