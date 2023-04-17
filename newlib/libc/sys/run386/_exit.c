#include "dosext.h"

__attribute__((noreturn)) void _exit(int status) {
    dosext_end_process(status);
}
