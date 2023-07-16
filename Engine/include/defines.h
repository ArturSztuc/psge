#pragma once
#include "Core/String/String.hpp"

// Unsigned int types
typedef unsigned char       U8;
typedef unsigned short      U16;
typedef unsigned int        U32;
typedef unsigned long long  U64;

// Signed int types
typedef signed char       I8;
typedef signed short      I16;
typedef signed int        I32;
typedef signed long long  I64;

// Floating point types
typedef float   F32;
typedef double  F64;

// Bool types
typedef char  B8;
typedef char  C8;

// String types
typedef String<4>   S4;
typedef String<8>   S8;
typedef String<16>  S16;
typedef String<32>  S32;
typedef String<64>  S64;
typedef String<128> S128;

// Bit manupulation, ripped from ROOT
#define BIT(n) (1ULL << (n))
#define SETBIT(n, i) ((n) |= BIT(i))
#define CLRBIT(n, i) ((n) &= ~BIT(i))
#define TESTBIT(n, i) ((B8)(((n) & BIT(i)) != 0))

// Platform detection
// Windows
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32)
  #define PLATFORM_WINDOWS 1
// MacOS
#elif defined(__APPLE__)
  #define PLATFORM_APPLE 1
  #include <TargetConditionals.h>
  // IPhone
  #if TARGET_OS_IPHONE
    #define PLATFORM_IPHONE 1
    #error "IPhone platform not supported."
  // IPad
  #elif TARGET_OP_IOS
    #define PLATFORM_IOS 1
    #error "IPad platform not supported."
  // MacOS
  #elif TARGET_OS_OSX
    #define PLATFORM_MAC
  #else
    #error "Apple platform, but cannot figure out which one"
  #endif
// Linux
#elif defined(__linux__)
  #define PLATFORM_LINUX 1
// Android
#elif defined(__ANDROID__) || defined(ANDROID)
  #define PLATFORM_ANDROID 1
#else
  #error "Platform unknown!"
#endif

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX) || defined (PLATFORM_MAC)
  #define PLATFORM_WINDOW_GLFW 1
#endif

//#define ASSETS CMAKE_INSTALL_LIBDIR"/PintSizedGameEngine/assets"

/**
 * @brief Declares the class as non-copyable and non-movable.
 * 
 * Simply insert in the public class header section.
 */
#define NOCOPY(ClassName) \
  ClassName(ClassName&&) = delete;                  \
  ClassName(const ClassName&) = delete;             \
  ClassName& operator=(const ClassName&) = delete;  \
  ClassName& operator=(ClassName&&) = delete;
