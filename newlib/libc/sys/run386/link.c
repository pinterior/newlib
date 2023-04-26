#include <errno.h>
#undef errno
extern int errno;

#include <unistd.h>

int _link(const char *, const char *) {
   errno = EMLINK;
   return -1;
}
