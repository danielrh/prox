#ifndef _PLATFORM_HPP_
#define _PLATFORM_HPP_

#define MERU_PLATFORM_WINDOWS 0
#define MERU_PLATFORM_LINUX   1
#define MERU_PLATFORM_MAC     2
#define MERU_PLATFORM_BSD     3


#if defined(__WIN32__) || defined(_WIN32)
#define MERU_PLATFORM MERU_PLATFORM_WINDOWS
#elif defined(__APPLE_CC__) || defined(__APPLE__)
#define MERU_PLATFORM MERU_PLATFORM_MAC
// some other packages (ogg) need this defined, and we're only supporting OS X
#define __MACOSX__
#else
#define MERU_PLATFORM MERU_PLATFORM_LINUX
#endif


#ifndef MERU_EXPORT
# if MERU_PLATFORM == MERU_PLATFORM_WINDOWS
#   if defined(STATIC_LINKED)
#     define MERU_EXPORT
#   else
#     if defined(MERU_BUILD)
#       define MERU_EXPORT __declspec(dllexport)
#     else
#       define MERU_EXPORT __declspec(dllimport)
#     endif
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASVISIBILITY)
#     define MERU_EXPORT __attribute__ ((visibility("default")))
#   else
#     define MERU_EXPORT
#   endif
# endif
#endif

#ifndef MERU_EXPORT_C
# define MERU_EXPORT_C extern "C" MERU_EXPORT
#endif


namespace Meru {

// numeric typedefs to get standardized types
typedef unsigned char uchar;
#if MERU_PLATFORM == MERU_PLATFORM_WINDOWS
typedef __int8 int8;
typedef unsigned __int8 uint8;
typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;
#else
#include <stdint.h>
typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
#endif
typedef float float32;
typedef double float64;

// floats or doubles, this #define should actually come from config.h
#define MERU_DOUBLE_PRECISION 1
#if MERU_DOUBLE_PRECISION
typedef float64 real;
#else
typedef float32 real;
#endif

typedef uchar byte;

// just in case we want to swap in a different string implementation,
// e.g. one using wchar_t
typedef std::string String;

} // namespace Meru

#endif //_PLATFORM_HPP_
