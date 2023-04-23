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

   // argc, argv, environ
   addl     $0x24, %eax
   movw     %ax, %fs
   addl     $(0x2c - 0x24), %eax
   movw     %ax, %gs
   movl     %esp, %esi
   subl     $12, %esp
   pushl    %esp
   movl     %fs:0x60, %edi
   movl     %edi, %ecx
   subl     %esi, %ecx
   pushl    %ecx
   pushl    %esi
   call     __setup_arg_env
   movl     %ds, %ecx
   movw     %cx, %fs
   movw     %cx, %gs
   testl    %eax, %eax
   jnz      .Larg_env_ok
   movb     $127, %al
   jmp      .Lexit
.Larg_env_ok:
   addl     %eax, %esi

   // setup heap
   movl     %esi, __program_initial
   movl     %esi, __program_size
   addl     $0xffff, %esi
   andl     $0xffff0000, %esi
   movl     %esi, %ebx
   shrl     $12, %ebx
   movb     $0x4a, %ah
   int      $0x21
   jnc      .Lresize_ok
   movl     %edi, %esi
.Lresize_ok:
   movl     %esi, __program_alloced

   // init, fini
   pushl    $__libc_fini_array
   call     atexit
   call     __libc_init_array

   addl     $16, %esp
   call     main
.Lexit:
   pushl    %eax
   call     exit

.comm    __program_initial, 4, 4
.comm    __program_size, 4, 4
.comm    __program_alloced, 4, 4
