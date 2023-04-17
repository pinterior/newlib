#include <errno.h>
#undef errno
extern int errno;

#include <limits.h>
#include <stdint.h>
#include <sys/types.h>

#include "dosext.h"


ssize_t _write(int file, const char *ptr, size_t len) {
   uint32_t r;

   if (file < 0 || UINT16_MAX < file) {
      errno = EBADF;
      return -1;
   }

   if (len == 0) {
      return 0;
   }

   if (dosext_write_handle(file, len, ptr, &r)) {
      return r;

   } else {
      switch (r) {
      case 5:
         errno = EACCES;
         break;
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
