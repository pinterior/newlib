#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern char **environ;

struct triple {
   int narg;
   char **args;
   char **envs;
};

struct heap {
   uintptr_t ptr;
   size_t remain;
};

struct env_misc {
   char *arg0;
   char *cmdline;
};

struct cmdline {
   const char *cmds;
   size_t len;
};

typedef const char __seg_fs *psp_ptr;
typedef const char __seg_gs *env_ptr;


static inline void *alloc(struct heap *heap, size_t size) {
   size_t pad = -heap->ptr & (uintptr_t)3;
   size_t request = ((size + 3) & ~(size_t)3);
   size_t advance = pad + request;

   if (heap->remain < advance) {
      return 0;
   }

   uintptr_t r = heap->ptr + pad;
   heap->ptr += advance;
   heap->remain -= advance;
   return (void *)r;
}


static inline size_t env_size(env_ptr e) {
   env_ptr p = e;

   if (*p) {
      while (*p++);
      return p - e;
   } else {
      return 0;
   }
}

static inline size_t arg0_size(env_ptr e) {
   if (e[0] == 1 && e[1] == 0) {
      env_ptr p = e + 2;
      while (*p++);
      return p - e - 2;
   } else {
      return 1;
   }
}

static inline size_t env_copy(char *d, env_ptr s) {
   env_ptr p = s;
   while (*d++ = *p++);
   return p - s;
}

static bool setup_env(struct heap *heap, struct triple *triple, struct env_misc *misc) {
   static const char cmdline_eq[8] = "CMDLINE=";
   const size_t cmdline_eq_len = 8;

   env_ptr env = 0, p;
   int nenv, esize, a0size;
   size_t n;

   p = env;
   nenv = 0;
   esize = 0;

   while ((n = env_size(p)) != 0) {
      ++nenv;
      esize += n;
      p += n;
   }
   ++p;
   a0size = arg0_size(p);
   esize += a0size;

   size_t alloc_size = sizeof(char *) * (nenv + 1) + esize;
   void *alloced = alloc(heap, alloc_size);
   if (!alloced) {
      return false;
   }
   char **ptrs = (char **)alloced;
   char *buf = (char *)alloced + sizeof(char *) * (nenv + 1);

   p = env;
   misc->cmdline = 0;
   for (int i = 0; i < nenv; ++i) {
      ptrs[i] = buf;
      n = env_copy(buf, p);
      if (__builtin_strncmp(cmdline_eq, buf, cmdline_eq_len) == 0) {
         misc->cmdline = buf + cmdline_eq_len;
      }
      buf += n;
      p += n;
   }
   ptrs[nenv] = 0;
   ++p;

   if (a0size == 1) {
      *buf = '\0';
   } else {
      env_copy(buf, p + 2);
   }

   misc->arg0 = buf;
   triple->envs = ptrs;
   environ = ptrs;
   return true;
}

static inline void args_copy(char *d, psp_ptr s, size_t n) {
   for (size_t i = 0; i < n; ++i) {
      d[i] = s[i];
   }
}

static inline bool is_arg_char(char c) {
   return (c != '\t' && c != '\n' && c != '\v' && c != '\f' && c != '\r' && c != ' ');
}

static inline size_t arg_size(struct cmdline *cmd) {
   const char *p = cmd->cmds;
   size_t len = cmd->len;

   for (size_t i = 0; i < len; ++i) {
      if (is_arg_char(p[i])) {
         size_t j;
         for (j = i; j < len; ++j) {
            if (!is_arg_char(p[j])) {
               break;
            }
         }
         cmd->cmds += j;
         cmd->len -= j;
         return j - i + 1;
      }
   }
   return 0;
}

static inline size_t arg_copy(char *d, struct cmdline *cmd) {
   const char *p = cmd->cmds;
   size_t len = cmd->len;

   for (size_t i = 0; i < len; ++i) {
      if (is_arg_char(p[i])) {
         size_t j;
         for (j = i; j < len; ++j) {
            if (!is_arg_char(p[j])) {
               break;
            }
            d[j - i] = p[j];
         }
         d[j - i] = '\0';
         cmd->cmds += j;
         cmd->len -= j;
         return j - i + 1;
      }
   }
   __builtin_unreachable();
}

static inline bool setup_arg(struct heap *heap, struct triple *triple, const struct env_misc *misc) {
   psp_ptr psp = 0;
   struct cmdline cmd, w;
   char copy[127];
   size_t n, narg, asize;

   if (misc->cmdline) {
      cmd.cmds = misc->cmdline;
      cmd.len = __builtin_strlen(misc->cmdline);
   } else {
      size_t len = (unsigned char)psp[0x80];
      if (sizeof(copy) < len) {
         return false;
      }
      args_copy(copy, &psp[0x81], len);
      cmd.cmds = copy;
      cmd.len = len;
   }

   w = cmd;

   while ((n = arg_size(&w)) != 0) {
      ++narg;
      asize += n;
   }

   size_t alloc_size = sizeof(char *) * (narg + 2) + asize;
   void *alloced = alloc(heap, alloc_size);
   if (!alloced) {
      return false;
   }
   char **ptrs = (char **)alloced;
   char *buf = (char *)alloced + sizeof(char *) * (narg + 2);

   w = cmd;
   ptrs[0] = misc->arg0;
   for (int i = 0; i < narg; ++i) {
      ptrs[i + 1] = buf;
      buf += arg_copy(buf, &w);
   }
   ptrs[narg + 1] = 0;

   triple->args = ptrs;
   triple->narg = narg + 1;
   return true;
}

size_t __setup_arg_env(uintptr_t heap, size_t remain, struct triple *triple) {

   char *arg0, *cmdline = 0;
   struct heap h  = { heap, remain };
   struct env_misc misc;

   if (!setup_env(&h, triple, &misc)) {
      return 0;
   }

   if (!setup_arg(&h, triple, &misc)) {
      return 0;
   }

   return h.ptr - heap;
}
