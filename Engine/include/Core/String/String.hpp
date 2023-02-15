/**
 * @file String.hpp
 * @brief Custom string class
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see String
 */
#pragma once

#include <cstddef>
#include <algorithm>
#include <cstring>
#include <ostream>

// TODO: Make sure Data() only returns filled part of the String, not the whole thing
// TODO: Add operators for +, += etc.

/**
 * @class String
 * @brief Custom string class
 *
 * std::string always uses heap memory which is what we want to avoid in a game
 * engine. 
 *
 * @todo TODO: Make sure the data's memory is correctly aligned
 */
template <size_t MAX_LENGTH>
class String
{
private:
  /// String's data holder
  char    m_data[MAX_LENGTH];

  /// Length of the currently held string
  size_t  m_length;

public:
  /// Default string's constructor
  String();
  String(const char* str);
  String(const String& str);

  ~String();

  String& operator=(const String& other);

  /**
   * @brief Introverted << operator
   * @param _out output stream
   * @param _str string to output into the stream
   */
  friend std::ostream& operator<<(std::ostream& _out, const String& _str)
  {
    for(size_t i = 0; i < _str.Length(); ++i)
      _out << _str.m_data[i];
    return _out;
  }

  /**
   * @brief Returns the raw data
   * @return Returns the raw data
   */
  const char* Data() const 
  {
    return m_data;
  };

  /**
   * @brief Returns length of the string
   * @return String's length
   */
  size_t Length() const {return m_length; };

  /**
   * @brief Sets the string to the given characters
   * @param _str characters to set the string to
   */
  void Set(const char* _str);

  /// Compares strings: case-sensitive
  bool Equals(const char* _str)    const;

  /// Compares strings: case-sensitive
  bool Equals(const String& _str)  const;

  /// Compares strings: case-insensitive
  bool EqualsIgnoreCase(const char* _str)    const;

  /// Compares strings: case-insensitive
  bool EqualsIgnoreCase(const String& _str)  const;

  /// Checks if string contains a substring
  bool Contains   (const char* _substring)   const;

  /// Checks if string contains a substring
  bool Contains   (const String& _substring) const;

  /// Checks if string begins with a substring
  bool BeginsWith (const char* _substring)   const;

  /// Checks if string begins with a substring
  bool BeginsWith (const String& _substring) const;

  /// Checks if string ends with a substring
  bool EndsWith   (const char* _substring)   const;

  /// Checks if string ends with a substring
  bool EndsWith   (const String& _substring) const;
};

typedef  String<4> String4;
typedef  String<8> String8;
typedef  String<16> String16;
typedef  String<32> String32;
typedef  String<64> String64;
typedef  String<128> String128;
