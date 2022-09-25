#pragma once

#include <assert.h>

#ifdef _WIN32
#include "unistd-windows.h"
#else
#include <unistd.h>
#include <string.h>
#endif

#ifndef _ASSERT
#ifdef _DEBUG
#define _ASSERT(condition) assert(condition)
#else
#define _ASSERT(condition)
#endif // _DEBUG
#endif // _ASSERT
#ifndef _ASSERT_EXPR
#define _ASSERT_EXPR(condition, message) _ASSERT(condition)
#endif // _ASSERT_EXPR

unsigned long long getCurrentTimeMs();