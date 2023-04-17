#include <errno.h>
#undef errno
extern int errno;

#include <limits.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "dosext.h"


off_t _lseek(int file, off_t offset, int whence) {
   int dir;
   uint32_t r;

   if (file < 0 || UINT16_MAX < file) {
      errno = EBADF;
      return -1;
   }

   switch(whence) {
   case SEEK_SET:
      dir = 0;
      break;
   case SEEK_CUR:
      dir = 1;
      break;
   case SEEK_END:
      dir = 2;
      break;
   default:
      errno = EINVAL;
      return (off_t)-1;
   }

   if (dosext_move_ptr(dir, file, offset, &r)) {
      return r;

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
      return -1;
   }
}
