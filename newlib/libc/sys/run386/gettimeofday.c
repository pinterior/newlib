#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "dosext.h"


int _gettimeofday(struct timeval *tv, struct timezone *tz) {
   if (tv) {
      uint16_t year;
      uint8_t  month, date, dow, prev;

      uint8_t hour, min, sec, csec;

      struct tm tm;
      time_t tt;

      dosext_get_date(&year, &month, &date, &dow);

      do {
         prev = date;

         dosext_get_time(&hour, &min, &sec, &csec);
         dosext_get_date(&year, &month, &date, &dow);
      } while (prev != date);

      tm.tm_year = year;
      tm.tm_mon = month - 1;
      tm.tm_mday = date;
      tm.tm_hour = hour;
      tm.tm_min = min;
      tm.tm_sec = sec;
      tm.tm_isdst = -1;
      tt = mktime(&tm);
      if (tt == (time_t)-1) {
         return -1;
      }

      tv->tv_sec = tt;
      tv->tv_usec = csec * 10000;
   }

   if (tz) {
      tz->tz_minuteswest = 0;
      tz->tz_dsttime = 0;
   }

   return 0;
}
