#include "Core/String/String.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>


// We need to pre-define all of them here to have separete
// .cpp file
// TODO: Need to try padding them to actual number of bits...
template class String<4>;
template class String<8>;
template class String<16>;
template class String<32>;
template class String<64>;
template class String<128>;

template <size_t MAX_LENGTH>
String<MAX_LENGTH>::String()
  : m_length(0)
{
  m_data[0] = '\0';
  for(int i = 1; i < MAX_LENGTH; ++i)
    m_data[i] = '\0';
  //m_data[MAX_LENGTH - 1] = '\0';

}

template <size_t MAX_LENGTH>
String<MAX_LENGTH>& String<MAX_LENGTH>::operator=(const String& other)
{
  Set(other.Data());
  return *this;
}

template <size_t MAX_LENGTH>
bool String<MAX_LENGTH>::operator==(const String& rhs) const
{
  return Equals(rhs);
}

template <size_t MAX_LENGTH>
String<MAX_LENGTH>::operator const char* () const
{
  return m_data;
}


template <size_t MAX_LENGTH>
String<MAX_LENGTH>::String(const char *str)
    : String()
{
  Set(str);
}

template <size_t MAX_LENGTH>
String<MAX_LENGTH>::String(const String& str)
  : String()
{
  Set(str.Data());
}

template <size_t MAX_LENGTH>
String<MAX_LENGTH>::~String()
{
}

template <size_t MAX_LENGTH>
void String<MAX_LENGTH>::Set(const char* str)
{
  m_length = std::min(strlen(str), MAX_LENGTH - 1);
  memcpy(m_data, str, m_length);
  for(int i = m_length; i < MAX_LENGTH; ++i)
    m_data[i] = '0';

  m_data[m_length] = '\0';
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::Equals(const char* str) const
{
  if(strlen(str) != m_length)
    return false;

  return strncmp(m_data, str, m_length) == 0;
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::Equals(const String& str) const
{
  if(str.Length() != m_length)
    return false;

  return strncmp(m_data, str.Data(), m_length) == 0;
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::EqualsIgnoreCase(const char* str) const 
{
  if(strlen(str) != m_length)
    return false;

  return strncasecmp(m_data, str, m_length) == 0;
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::EqualsIgnoreCase(const String& str) const 
{
  if(str.Length() != m_length)
    return false;

  return strncasecmp(m_data, str.Data(), m_length) == 0;
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::Contains(const char* substring) const 
{
  return strstr(m_data, substring) != nullptr;
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::Contains(const String& substring) const 
{
  return strstr(m_data, substring.Data()) != nullptr;
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::BeginsWith(const char* substring) const 
{
  if(strlen(substring) > m_length)
    return false;

  return strncmp(m_data, substring, strlen(substring)) == 0;
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::BeginsWith(const String& substring) const 
{
  if(substring.Length() > m_length)
    return false;

  return strncmp(m_data, substring.Data(), substring.Length()) == 0;
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::EndsWith(const char* substring) const 
{
  size_t substring_length = strlen(substring);

  return (m_length >= substring_length) &&
    (strncmp(m_data + m_length - substring_length, substring, substring_length) == 0);
}

template<size_t MAX_LENGTH>
bool String<MAX_LENGTH>::EndsWith(const String& substring) const 
{
  size_t substring_length = substring.Length();
  return (m_length >= substring_length) &&
    (strncmp(m_data + m_length - substring_length, substring.Data(), substring_length) == 0);
}

