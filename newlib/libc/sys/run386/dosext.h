#pragma once

#include <stdbool.h>
#include <stdint.h>


static inline void dosext_get_date(uint16_t *year, uint8_t *month, uint8_t *date, uint8_t *dow) {
   uint8_t al;
   uint16_t cx, dx;

   asm (
      "movb    $0x2a, %%ah\n\t"
      "int     $0x21"
      : "=a" (al), "=c" (cx), "=d" (dx));

   *year = cx;
   *month = dx >> 8;
   *date = dx;
   *dow = al;
}


static inline void dosext_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *centisecond) {
   uint16_t cx, dx;

   asm (
      "movb    $0x2c, %%ah\n\t"
      "int     $0x21"
      : "=c" (cx), "=d" (dx)
      :
      : "ah");

   *hour = cx >> 8;
   *minute = cx;
   *second = dx >> 8;
   *centisecond = dx;
}


static inline bool dosext_close_handle(uint16_t handle, uint16_t *r) {
   uint16_t result;
   bool cf;

   asm volatile (
      "movb    $0x3e, %%ah\n\t"
      "int     $0x21"
      : "=@ccc" (cf), "=a" (result)
      : "b" (handle));

   *r = result;
   return !cf;
}


static inline bool dosext_read_handle(uint16_t handle, uint32_t count, void *buf, uint32_t *r) {
   uint16_t result;
   bool cf;

   asm volatile (
      "movb    $0x3f, %%ah\n\t"
      "int     $0x21"
      : "=@ccc" (cf), "=a" (result), "=m" (*(char (*)[])buf)
      : "b" (handle), "c" (count), "d" (buf));

   if (cf) {
      *r = (uint16_t)result;
   } else {
      *r = result;
   }
   return !cf;
}


static inline bool dosext_write_handle(uint16_t handle, uint32_t count, const void *buf, uint32_t *r) {
   uint32_t result;
   bool cf;

   asm volatile (
      "movb    $0x40, %%ah\n\t"
      "int     $0x21"
      : "=@ccc" (cf), "=a" (result)
      : "b" (handle), "c" (count), "d" (buf), "m" (*(const char (*)[])buf));

   if (cf) {
      *r = (uint16_t)result;
   } else {
      *r = result;
   }
   return !cf;
}


static inline bool dosext_move_ptr(uint8_t origin, uint16_t handle, uint32_t offset, uint32_t *r) {
   uint16_t offset_hi, offset_lo, result_hi, result_lo;
   bool cf;

   offset_hi = offset >> 16;
   offset_lo = offset;

   asm volatile (
      "movb    $0x42, %%ah\n\t"
      "int     $0x21"
      : "=@ccc" (cf), "=a" (result_lo), "=d" (result_hi)
      : "a" (origin), "b" (handle), "c" (offset_hi), "d" (offset_lo));

   if (cf) {
      *r = result_lo;
   } else {
      *r = ((uint32_t)result_hi << 16) + result_lo;
   }
   return !cf;
}


static inline bool dosext_ioctl_data_get(uint16_t handle, uint16_t *r) {
   uint16_t error, result;
   bool cf;

   asm volatile (
      "movw    $0x4400, %%ax\n\t"
      "int     $0x21"
      : "=@ccc" (cf), "=a" (error), "=d" (result)
      : "b" (handle));

   if (cf) {
      *r = error;
   } else {
      *r = result;
   }
   return !cf;
}


static inline bool dosext_set_block(uint32_t pages, uint32_t *r) {
   uint16_t error, avail;
   bool cf;

   asm volatile (
      "movb    $0x4a, %%ah\n\t"
      "int     $0x21"
      : "=@ccc" (cf), "=a" (error), "=b" (avail)
      : "b" (pages));

   if (cf) {
      *r = error;
   } else {
      *r = avail;
   }
   return !cf;
}


__attribute__((noreturn))
static inline void dosext_end_process(uint8_t code) {

   asm volatile (
      "movb    $0x4c, %%ah\n\t"
      "int     $0x21"
      :
      : "a" (code));

   __builtin_unreachable();
}
