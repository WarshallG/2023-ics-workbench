#include "asm.h"
#include <assert.h>
#include <stdio.h>

int main() {
  asm_jmp_buf buf;
  int r = asm_setjmp(buf);
  printf("Setjump passed!\n");
  printf("r = %d\n",r);
  if (r == 0) {
    assert(asm_add(1234, 5678) == 6912);
    printf("Add passed!\n");
    
    assert(asm_popcnt(0x0123456789abcdefULL) == 32);
    printf("Popcnt passed!\n");
    
    asm_longjmp(buf, 123);
    printf("longjump passed!\n");
    
  }

  else {
    assert(r == 123);
    printf("PASSED.\n");
  }
}
