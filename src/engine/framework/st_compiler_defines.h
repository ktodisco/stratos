#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

// Compilers.
#if defined(_MSC_VER)
#define st_MSVC
#elif defined(__MINGW32__)
#define st_MINGW
#endif

// Architecture.
#if defined(st_MSVC)
#if defined(_WIN64)
#define st_64_BIT
#elif defined(_WIN32)
#define st_32_BIT
#endif
#endif

#if defined(__MINGW32__)
#include <_mingw.h>
#if defined(__MINGW64_VERSION_MAJOR)
#define st_64_BIT
#else
#define st_32_BIT
#endif
#endif
