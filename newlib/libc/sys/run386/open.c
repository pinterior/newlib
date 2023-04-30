#include <errno.h>
#undef errno
extern int errno;

#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "dosext.h"


int _open(const char *pathname, int flags, mode_t mode) {
   const int valid_flags =
      O_RDONLY | O_WRONLY | O_RDWR | O_CLOEXEC |
      O_CREAT | O_TRUNC | O_EXCL | O_NONBLOCK;
   uint8_t access, attrs;
   uint16_t err, fd;

   if ((flags & valid_flags) != flags) {
      errno = EINVAL;
      return -1;
   }

   switch (flags & O_ACCMODE) {
   case O_RDWR:
      access = 2;
      break;
   case O_RDONLY:
      access = 0;
      break;
   case O_WRONLY:
      access = 1;
      break;
   default:
      errno = EINVAL;
      return -1;
   }

   if (!(flags & O_CLOEXEC)) {
      access |= 0x80;
   }

   if (flags & O_CREAT) {
      if (mode & S_IWUSR) {
         attrs = 0x20;
      } else {
         attrs = 0x21;
      }
   }

   switch (flags & (O_CREAT | O_EXCL)) {
   case 0:
   case O_EXCL:
      if (err = dosext_open_handle(access, pathname, &fd)) {
         errno = dosext_to_errno(err);
         return -1;
      }
      break;

   case O_CREAT:
      if (err = dosext_create_new(attrs, pathname, &fd)) {
         // ignore
      } else {
         dosext_close_handle(fd, &err);
      }
      if (err = dosext_open_handle(access, pathname, &fd)) {
         errno = dosext_to_errno(err);
         return -1;
      }
      break;

   case O_CREAT | O_EXCL:
      if (err = dosext_create_new(attrs, pathname, &fd)) {
         errno = dosext_to_errno(err);
         return -1;
      } else {
         dosext_close_handle(fd, &err);
      }
      if (err = dosext_open_handle(access, pathname, &fd)) {
         errno = dosext_to_errno(err);
         return -1;
      }
      break;
   }

   if ((flags & O_ACCMODE) != O_RDONLY && (flags & O_TRUNC)) {
      uint32_t werr;
      if (!dosext_write_handle(fd, 0, 0, &werr)) {
         dosext_close_handle(fd, &err);
         errno = EIO;
         return -1;
      }
   }

   return fd;
}
