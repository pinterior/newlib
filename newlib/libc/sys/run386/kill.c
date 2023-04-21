#include <errno.h>
#undef errno
extern int errno;

#include <sys/types.h>
#include <signal.h>

int _kill(pid_t pid, int sig) {
   errno = EINVAL;
   return -1;
}
