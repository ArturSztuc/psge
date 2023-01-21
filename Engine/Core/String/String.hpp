#pragma once

#include <cstddef>
#include <algorithm>
#include <cstring>
#include <ostream>

// TODO: Make sure Data() only returns filled part of the String, not the whole thing
// TODO: Add operators for +, += etc.

template <size_t MAX_LENGTH>
class String
{
private:
  char    m_data[MAX_LENGTH];
  size_t  m_length;

public:
  String();
  String(const char* str);
  String(const String& str);

  String& operator=(const String& other);

  // Introveted << operator
  friend std::ostream& operator<<(std::ostream& out, const String& str)
  {
    for(size_t i = 0; i < str.Length(); ++i)
      out << str.m_data[i];
    return out;
  }

  const char* Data() const 
  {
    return m_data;
  };

  size_t Length() const {return m_length; };

  void Set(const char* str);

  bool Equals(const char* str)    const;
  bool Equals(const String& str)  const;
  bool EqualsIgnoreCase(const char* str)    const;
  bool EqualsIgnoreCase(const String& str)  const;

  bool Contains   (const char* substring)   const;
  bool Contains   (const String& substring) const;
  bool BeginsWith (const char* substring)   const;
  bool BeginsWith (const String& substring) const;
  bool EndsWith   (const char* substring)   const;
  bool EndsWith   (const String& substring) const;
};

typedef  String<4> String4;
typedef  String<8> String8;
typedef  String<16> String16;
typedef  String<32> String32;
typedef  String<64> String64;
typedef  String<128> String128;
