#include <errno.h>
#undef errno
extern int errno;

#include <stdint.h>
#include <limits.h>

#include "dosext.h"


int _close(int file) {
   uint16_t r;

   if (file < 0 || UINT16_MAX < file) {
      errno = EBADF;
      return -1;
   }

   if (dosext_close_handle(file, &r)) {
      return 0;

   } else {
      switch (r) {
      case 6:
         errno = EBADF;
         break;
      default:
         errno = EIO;
         break;
      }
      return -1;
   }
}
