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
