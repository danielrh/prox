#ifndef _PLATFORM_HPP_
#define _PLATFORM_HPP_

#define PLATFORM_WINDOWS 0
#define PLATFORM_LINUX   1
#define PLATFORM_MAC     2


#if defined(__WIN32__) || defined(_WIN32)
#define LIBPROX_PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE_CC__) || defined(__APPLE__)
#define LIBPROX_PLATFORM PLATFORM_MAC
#else
#define LIBPROX_PLATFORM PLATFORM_LINUX
#endif


#ifndef LIBPROX_EXPORT
# if LIBPROX_PLATFORM == _PLATFORM_WINDOWS
#   if defined(STATIC_LINKED)
#     define LIBPROX_EXPORT
#   else
#     if defined(LIBPROX_BUILD)
#       define LIBPROX_EXPORT __declspec(dllexport)
#     else
#       define LIBPROX_EXPORT __declspec(dllimport)
#     endif
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASVISIBILITY)
#     define LIBPROX_EXPORT __attribute__ ((visibility("default")))
#   else
#     define LIBPROX_EXPORT
#   endif
# endif
#endif

#ifndef LIBPROX_EXPORT_C
# define LIBPROX_EXPORT_C extern "C" LIBPROX_EXPORT
#endif

#include <string>
#include <cassert>
#include <map>
#include <list>
#include <vector>
#include <deque>

namespace Prox {

// numeric typedefs to get standardized types
typedef unsigned char uchar;
#if LIBPROX_PLATFORM == PLATFORM_WINDOWS
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

typedef uchar byte;

// just in case we want to swap in a different string implementation,
// e.g. one using wchar_t
typedef std::string String;

} // namespace Prox

#endif //_PLATFORM_HPP_
