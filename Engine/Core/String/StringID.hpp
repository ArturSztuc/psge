#pragma once

#include "defines.h"
#include <array>

typedef U32 StringId;
typedef U64 StringId64;

extern StringId InternString(const char* str);

U32 HashCrc34(const S32& str);

U64 HashCrc64(const S32& str);
