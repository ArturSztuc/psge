/**
 * @file ECSEntity.hpp
 * @brief File containing the Entity and base classes for ECS
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see Entity
 * @see ComponentBase
 * @see SystemBase
 */
#pragma once

#include "defines.h"
#include "Core/EntityComponentSystem/System.hpp"

#include <unordered_map>
#include <typeinfo>

/**
 * @class ComponentBase
 * @brief A base component class, from which all object types should be derived
 *
 * We need to ensure that all the objects that might belong to an entity
 * (whether model, texture, audio, text etc.) are derived from this
 * ComponentBase
 */
class ComponentBase
{
public:
  virtual ~ComponentBase() = default;
};

/**
 * @class Entity
 * @brief An entity class for our Entity Component System
 *
 * Entity is effectively a simple ID for an object to have, to be used in the
 * Entity Component System. We could in principle derive a more complex entity
 * from it, but that's not needed for the ECS itself.
 * Perhaps this class _could_ be used for other purposes, other entity systems.
 *
 * @todo TODO: Test for multithreading.
 * @todo TODO: Might need atomic and locks in the constructor
 * @todo TODO: Check if max number of entities reached
 */
class Entity
{
private:
  /// Unique identifier for the entity
  U32 m_id; // Max ~4.29B entities

  /// Static counter for generating unique entity IDs
  static unsigned int m_nextId;

public:
  /**
   * @brief Default constructor. Should have a unique ID.
   *
   * Default Entity constructor. It should contain it's own
   * unique ID, created by iterating a static member.
   */
  Entity();

  ~Entity() = default;

  /**
   * @brief Returns the Entity's ID.
   * @return Entity's unique ID.
   * @todo TODO: Change to our own basic types
   */
  U32 GetId() const;

  /**
   * @brief Resets the entity count to 0
   *
   * @todo TODO: This should be re-written, could cause errors if used carelessly...
   *
   */
  static void ClearEntities();
};
