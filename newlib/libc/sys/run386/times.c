#include <errno.h>
#undef errno
extern int errno;

#include <sys/times.h>


clock_t _times(struct tms *buf) {
   errno = ENOSYS;
   return (clock_t)-1;
}
