#include <errno.h>
#undef errno
extern int errno;

#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "dosext.h"


static inline bool is_wildcard(const char *s) {
   for (const char *p = s; ; ++p) {
      switch (*p) {
         case '?':
         case '*':
            return true;
         case '\0':
            return false;
      }
   }
}


int _stat(const char *pathname, struct stat *st) {
   uint16_t err;
   uint8_t dta[128];

   if (is_wildcard(pathname)) {
      errno = ENOENT;
      return -1;
   }

   memset(st, 0, sizeof(struct stat));

   dosext_set_dta(dta);

   if (!dosext_find_first_file(pathname, 0x77, &err)) {
      errno = ENOENT;
      return -1;
   }

   uint8_t attr = dta[0x15];
   uint16_t hms = dta[0x16] + ((uint16_t)dta[0x17] << 8);
   uint16_t ymd = dta[0x18] + ((uint16_t)dta[0x19] << 8);

   if (attr & 0x08) {
         // volume label
      errno = ENOENT;
      return -1;
   }

   if (attr & 0x40) {
      // character device???
      st->st_mode = S_IFCHR | S_IRWXU;
   } else if (attr & 0x10) {
      // subdirectory
      st->st_mode = S_IFDIR | S_IRWXU;
   } else if (attr & 0x01) {
      // read only
      st->st_mode = S_IFREG | S_IRUSR | S_IXUSR;
   } else {
      st->st_mode = S_IFREG | S_IRWXU;
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

   st->st_gid = 0;
   st->st_atime = st->st_ctime = st->st_mtime = ts;
   st->st_nlink = 1;

   st->st_size =
      dta[0x1a] + ((uint32_t)dta[0x1b] << 8) +
      ((uint32_t)dta[0x1c] << 16) + ((uint32_t)dta[0x1d] << 24);

   return 0;
}
