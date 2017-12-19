#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

// Compilers.
#if defined(_MSC_VER)
#define ST_MSVC
#elif defined(__MINGW32__)
#define ST_MINGW
#endif

// Architecture.
#if defined(ST_MSVC)
#if defined(_WIN64)
#define ST_64_BIT
#elif defined(_WIN32)
#define ST_32_BIT
#endif
#endif

#if defined(__MINGW32__)
#include <_mingw.h>
#if defined(__MINGW64_VERSION_MAJOR)
#define ST_64_BIT
#else
#define ST_32_BIT
#endif
#endif
