#include <errno.h>
#undef errno
extern int errno;

#include <stdlib.h>
#include <sys/stat.h>


int _fstat(int file, struct stat *st) {
   memset(st, 0, sizeof(struct stat));

   int orig_errno = errno;

   if (isatty(file)) {
      st->st_mode = S_IFCHR | S_IRWXU;
      return 0;
   } else if (errno == ENOTTY) {
      st->st_mode = S_IFREG | S_IRWXU;
      errno = orig_errno;
      return 0;
   } else {
      return -1;
   }
}
