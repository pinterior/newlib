#include <errno.h>
#undef errno
extern int errno;

#include <sys/types.h>
#include <sys/wait.h>

#include "dosext.h"


pid_t _wait(int *status) {
   errno = ENOSYS;
   return -1;
}
