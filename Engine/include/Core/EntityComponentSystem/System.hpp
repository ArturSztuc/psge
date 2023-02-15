/**
 * @file System.hpp
 * @brief File containing the System base class and enums
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-30
 *
 * @see SystemTypes
 * @see SystemBase
 */
#pragma once

#include "defines.h"

#include <unordered_map>
#include <typeinfo>
#include <vector>

/*! @enum SystemType
 *
 *  @brief Enumerator of system types
 */
enum SystemType 
{  
  SYSTEM_TYPE_INPUT = 1,
  SYSTEM_TYPE_EVENT,
  SYSTEM_TYPE_LOGGING,

  SYSTEM_TYPE_MAX = 255
};

/**
 * @class SystemBase
 * @brief A base system class, for all the systems operating on components
 *
 * @todo TODO: We need to start implementing it in all the systems that might operate on components. We probably don't have any yet?
 * @todo TODO: Need to converge on naming. We have Systems, Managers etc. System for components, managers for other singletons? All systems?
 */
class SystemBase
{
public:

  SystemBase(SystemType _type)
    : m_type(_type)
  {};

  virtual ~SystemBase() = default;

  /**
   * @brief Overloadable update function for each system
   * @param _deltaTime frame time for simulation
   * @todo TODO: change from float to a time object!
   */
  virtual void Update(F32 _deltaTime) = 0;

  virtual void Shutdown() = 0;

  SystemType GetSystemType() {return m_type;}

private:
  SystemType m_type;
};

class SystemManager
{
public:
  void AddSystem(SystemBase* _system)
  {
    m_systems.push_back(_system);
  }

private:
  std::vector<SystemBase*> m_systems;
};
