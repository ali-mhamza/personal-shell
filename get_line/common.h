#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef float       f32;
typedef double      f64;

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #define STATIC_ASSERT _Static_assert
#else
    #define STATIC_ASSERT static_assert
#endif

STATIC_ASSERT(sizeof(f32) == 4, "Wrong float size.");
STATIC_ASSERT(sizeof(f64) == 8, "Wrong double size.");

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202000L)
    #define NODISCARD   [[nodiscard]]
#elif defined(__GNUC__) || defined(__clang__)
    #define NODISCARD    __attribute__ ((warn_unused_result))
#else
    #define NODISCARD
#endif

#endif