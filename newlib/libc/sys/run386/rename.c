#include <errno.h>
#undef errno
extern int errno;

#include <unistd.h>

#include "dosext.h"


int _rename(const char *old, const char *new) {
   uint16_t r;

   if (dosext_rename_file(old, new, &r)) {
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
