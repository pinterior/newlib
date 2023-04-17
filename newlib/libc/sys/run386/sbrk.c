#include <errno.h>
#undef errno
extern int errno;

#include <stdint.h>
#include <stddef.h>

#include "dosext.h"


extern uint32_t __program_initial;
extern uint32_t __program_size;
extern uint32_t __program_alloced;

#define PAGE_SIZE ((uint32_t)4096)
#define ALLOC_UNIT (PAGE_SIZE * 16)


void *_sbrk(ptrdiff_t incr) {
   uint32_t new_size, new_alloc, new_pages, old_size, r;

   old_size = __program_size;

   if (incr == 0) {
      return (void *)old_size;
   }

   if (0 < incr) {
      if (__builtin_add_overflow(__program_size, incr, &new_size)) {
         errno = ENOMEM;
         return (void *)-1;
      }
   } else {
      if (__builtin_sub_overflow(__program_size, -incr, &new_size)) {
         new_size = __program_initial;
      } else if (new_size < __program_initial) {
         new_size = __program_initial;
      }
   }

   new_alloc = (new_size + ALLOC_UNIT - 1) / ALLOC_UNIT * ALLOC_UNIT;

   if (new_alloc == __program_alloced) {
      __program_size = new_size;
      return (void *)old_size;
   }

   new_pages = new_alloc / PAGE_SIZE;

   if (dosext_set_block(new_pages, &r)) {
      __program_alloced = new_alloc;
      __program_size = new_size;
      return (void *)old_size;

   } else {
      if (new_size < __program_alloced) {
         __program_size = new_size;
         return (void *)old_size;
      } else {
         errno = ENOMEM;
         return (void *)-1;
      }
   }
}
