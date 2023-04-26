#include <errno.h>
#undef errno
extern int errno;

#include <unistd.h>

#include "dosext.h"


int _unlink(const char *file) {
   uint16_t r;

   if (dosext_delete_entry(file, &r)) {
      return 0;

   } else {
      switch (r) {
      case 2:
         errno = ENOENT;
         break;
      case 5:
         errno = EACCES;
         break;
      default:
         errno = EIO;
         break;
      }
      return 0;
   }
}

