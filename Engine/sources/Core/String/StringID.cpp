#include "Core/String/StringID.hpp"
#include <unordered_map>

// TODO: page 461 for much better implementation
//
// Wrap this all into a StrindId or FName class
// Write macro e.g. SID("string") that will create an instance of that class,
// with it's hashed value
//
// We need to have a macro that takes "string" and creates "string"_sid
//
// Inspiration from UnrealEngine?
//  Header: UnrealEngine/Engine/Source/Runtime/Core/Public/UObject/NameTypes.h
//  Source: UnrealEngine/Engine/Source/Runtime/Core/Private/UObject/UnrealNames.cpp

static std::unordered_map<StringId, const char*> g_stringTable;

StringId InternString(const char* str)
{
  // TODO: need to change HashCrc34/64 to accept const char* str pointers
  StringId sid = HashCrc34(str);

  std::unordered_map<StringId, const char*>::iterator it = g_stringTable.find(sid);

  if(it == g_stringTable.end()){
    // This string was not added to the table yet.
    g_stringTable[sid] = strdup(str);
  }

  return sid;
}

U32 HashCrc34(const S32& str) {
  const U8* data = reinterpret_cast<const U8*>(str.Data());
  std::size_t size = str.Length();
  const U32 poly = 0x8E5D5257;
  std::array<U32, 256> table;
  for (std::size_t i = 0; i < 256; ++i) {
    U32 c = i;
    for (std::size_t j = 0; j < 8; ++j) {
      c = (c & 1) ? poly ^ (c >> 1) : c >> 1;
    }
    table[i] = c;
  }
  U32 hash = 0x3FFFFFFF;
  for (std::size_t i = 0; i < size; ++i) {
    hash = table[(hash ^ data[i]) & 0xFF] ^ (hash >> 8);
  }
  return hash ^ 0x3FFFFFFF;
};

U64 HashCrc64(const S32& str) {
  const U8* data = reinterpret_cast<const U8*>(str.Data());
  std::size_t size = str.Length();
  const U64 poly = 0xC96C5795D7870F42;
  std::array<U64, 256> table;
  for (std::size_t i = 0; i < 256; ++i) {
    U64 c = i;
    for (std::size_t j = 0; j < 8; ++j) {
      c = (c & 1) ? poly ^ (c >> 1) : c >> 1;
    }
    table[i] = c;
  }
  U64 hash = 0xFFFFFFFFFFFFFFFF;
  for (std::size_t i = 0; i < size; ++i) {
    hash = table[(hash ^ data[i]) & 0xFF] ^ (hash >> 8);
  }
  return hash ^ 0xFFFFFFFFFFFFFFFF;
}
