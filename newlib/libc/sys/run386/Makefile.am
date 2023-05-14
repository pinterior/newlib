AUTOMAKE_OPTIONS = cygnus
INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)
AM_CCASFLAGS = $(INCLUDES)

noinst_LIBRARIES = lib.a

EXTRA_lib_a_SOURCES = crt0.S
lib_a_SOURCES = \
    _exit.c \
    close.c \
    execve.c \
    fork.c \
    fstat.c \
    getpid.c \
    gettimeofday.c \
    isatty.c \
    kill.c \
    link.c \
    lseek.c \
    open.c \
    read.c \
    rename.c \
    sbrk.c \
    stat.c \
    times.c \
    unlink.c \
    wait.c \
    write.c \
    toerrno.c premain.c
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)

all-local: crt0.o

ACLOCAL_AMFLAGS = -I ../../.. -I ../../../..
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host
