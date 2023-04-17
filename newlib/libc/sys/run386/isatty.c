#include <errno.h>
#undef errno
extern int errno;

#include <stdint.h>
#include <limits.h>

#include "dosext.h"


int _isatty(int file) {
   uint16_t r;

   if (file < 0 || UINT16_MAX < file) {
      errno = EBADF;
      return -1;
   }

   if (dosext_ioctl_data_get(file, &r)) {
      if (r & 0x80) {
         if ((r & 1) || (r & 2)) {
            return 1;
         }
      }
      errno = ENOTTY;
      return 0;

   } else {
      switch (r) {
      case 1:
         errno = EINVAL;
         break;
      case 6:
         errno = EBADF;
         break;
      default:
         errno = EIO;
         break;
      }
      return 0;
   }
}
