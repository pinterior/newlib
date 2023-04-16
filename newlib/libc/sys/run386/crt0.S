   .text
   .globl  _start
_start:
   movl  $0x4c00, %eax
   int   $0x21
