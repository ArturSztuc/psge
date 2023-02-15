/**
 * @file ConfigParser.h
 * @brief A class parsing json config files
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-14
 *
 * @see JsonConfigParser
 */

/// @todo TODO: All major objects should be able to take json object for initialization.

#pragma once 

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

/// @todo TODO: Need to have this as an internal dependency in Engine/Externals folder
#include "nlohmann/json.hpp"

using json = nlohmann::json;

/**
 * @class JsonConfigParser
 * @brief A class parsing json config files
 * @see Load
 * @see Get
 */
class JsonConfigParser
{
public:
  /**
   * @brief Constructor also loads the json object
   * @param _filename Json location/file
   */
  JsonConfigParser(const std::string& _filename)
  {
    Load(_filename);
  }

  ~JsonConfigParser(){};

  /**
   * @brief Loads the Json file into an internal object
   * @param _filename Json location/file
   */
  void Load(const std::string& _filename)
  {
    std::ifstream file(_filename);
    if(!file.is_open())
      throw std::runtime_error("Error opening config file: " + _filename);

    file >> m_config;
  };

  static JsonConfigParser LoadDefaultConfig()
  {
    /// @todo TODO: This is very, very bad! Need to include it another way. This is todo next.
    return JsonConfigParser("/home/artur/projects/game_engines/psge/Engine/include/Core/Config/DefaultConfig.json");
  }

  /**
   * @brief Gets an entry from json config given they key string
   * @param _key key that corresponds to value we want to load
   * @param _default_value default value to be loaded if config doesn't have it
   */
  template <typename T>
  T Get(const std::string& _key, const T& _default_value = T())
  {
    auto it = m_config.find(_key);

    // Return default if the key doesn't exist
    if(it == m_config.end())
      return _default_value;

    return it.value().get<T>();
  };

private:
  /// Internal Json config holder
  json m_config;
};
