#include <errno.h>
#include <stdint.h>

#include "dosext.h"


int dosext_to_errno(uint16_t err) {
   switch (err) {
   case 1: return EINVAL;
   case 2: return ENOENT;
   case 3: return ENOENT;
   case 4: return EMFILE;
   case 5: return EACCES;
   case 12: return EINVAL;
   case 80: return EEXIST;
   default: return EIO;
   }
}
