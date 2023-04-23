AUTOMAKE_OPTIONS = cygnus
INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)
AM_CCASFLAGS = $(INCLUDES)

noinst_LIBRARIES = lib.a

EXTRA_lib_a_SOURCES = crt0.S
lib_a_SOURCES = \
    _exit.c \
    close.c \
    fstat.c \
    getpid.c \
    gettimeofday.c \
    isatty.c \
    kill.c \
    lseek.c \
    read.c \
    sbrk.c \
    write.c \
    premain.c
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)

all-local: crt0.o

ACLOCAL_AMFLAGS = -I ../../.. -I ../../../..
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host
