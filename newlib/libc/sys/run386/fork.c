#include <errno.h>
#undef errno
extern int errno;

#include <unistd.h>


int _execve(const char *pathname, char *const argv[], char *const envp[]) {
   errno = ENOSYS;
   return -1;
}