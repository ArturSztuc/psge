/**
 * @file ConfigParser.h
 * @brief A class parsing jaml config files
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-14
 *
 * @see JsonConfigParser
 */

#pragma once 

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

/// @todo TODO: Need to have this as an internal dependency
#include "nlohmann/json.hpp"

using json = nlohmann::json;

/**
 * @class JsonConfigParser
 * @brief A class parsing json config files
 */
class JsonConfigParser
{
public:
  JsonConfigParser(const std::string& _filename)
  {
    Load(_filename);
  }

  ~JsonConfigParser(){};

  void Load(const std::string& _filename)
  {
    std::ifstream file(_filename);
    if(!file.is_open())
      throw std::runtime_error("Error opening config file: " + _filename);

    file >> m_config;
  };

  template <typename T>
  T Get(const std::string& _key, const T& _default_value = T())
  {
    auto it = m_config.find(_key);
    if(it == m_config.end())
      return _default_value;
    return it.value().get<T>();
  };

private:
  json m_config;
};
