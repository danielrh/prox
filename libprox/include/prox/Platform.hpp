/*  libprox
 *  Platform.hpp
 *
 *  Copyright (c) 2009, Ewen Cheslack-Postava
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of libprox nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

#include <cstring>
#include <cassert>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <deque>
#include <set>
#include <queue>
#include <stack>

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
