   .globl   __program_initial
   .globl   __program_size
   .globl   __program_alloced
   .globl   _start

.text
_start:
   // clear bss
   movl     $__bss_start, %edi
   movl     $_end, %edx
   subl     %edi, %edx
   cld
   movl     %edx, %ecx
   shrl     $2, %ecx
   rep stosl
   movl     %edx, %ecx
   andl     $3, %ecx
   rep stosb

   // setup heap
   movl     %esp, %ecx
   movl     %ecx, __program_initial
   movl     %ecx, __program_size
   addl     $0xffff, %ecx
   andl     $0xffff0000, %ecx
   movl     %ecx, %ebx
   shrl     $12, %ebx
   movb     $0x4a, %ah
   int      $0x21
   jnc      .Lresize_ok
   pushl    $0x24
   popl     %es
   movl     %es:0x60, %ecx
   pushl    %ds
   popl     %es
.Lresize_ok:
   movl     %ecx, __program_alloced

   pushl    $0
   pushl    $0
   pushl    $0
   call     main

   movb     $0x4c, %ah
   int      $0x21

.comm    __program_initial, 4, 4
.comm    __program_size, 4, 4
.comm    __program_alloced, 4, 4
