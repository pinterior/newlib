#include <errno.h>
#undef errno
extern int errno;

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "dosext.h"


int _fstat(int file, struct stat *st) {

   if (file < 0 || UINT16_MAX < file) {
      errno = EBADF;
      return -1;
   }

   memset(st, 0, sizeof(struct stat));

   uint16_t r;
   if (!dosext_ioctl_data_get(file, &r)) {
      errno = dosext_to_errno(r);
      return -1;
   }

   if (r & 0x80) {
      // character device
      st->st_dev = file;
      st->st_rdev = file;
      st->st_nlink = 1;
      st->st_mode = S_IFCHR | S_IRWXU;

   } else {
      // regular file
      uint16_t ymd, hms;

      if (!dosext_get_date_time(file, &r, &ymd, &hms)) {
         errno = dosext_to_errno(r);
         return -1;
      }

      struct tm tm = { 0 };
      tm.tm_year = (ymd >> 9) + 1980 - 1900;
      tm.tm_mon  = ((ymd >> 5) & 0x0f) - 1;
      tm.tm_mday = ymd & 0x1f;
      tm.tm_hour = hms >> 11;
      tm.tm_min  = (hms >> 5) & 0x3f;
      tm.tm_sec  = (hms & 0x1f) * 2;

      time_t ts = mktime(&tm);
      if (ts < 0) {
         errno = EIO;
         return -1;
      }

      st->st_atime = st->st_mtime = st->st_ctime = ts;
      st->st_nlink = 1;
      st->st_mode = S_IFREG | S_IRWXU; // TODO: file attributes

      uint16_t nfd, err;
      uint32_t fsize;
      if (!dosext_xdup(file, &nfd)) {
         errno = EIO;
         return -1;
      }

      if (!dosext_move_ptr(2, nfd, 0, &fsize)) {
         dosext_close_handle(nfd, &err);
         errno = EIO;
         return -1;
      }

      st->st_size = fsize;
      dosext_close_handle(nfd, &err);
   }
   return 0;
}
