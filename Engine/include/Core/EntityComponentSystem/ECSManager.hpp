/**
 * @file ECSManager.hpp
 * @brief Contains the Entity Component System.
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see ECSManager
 */
#pragma once

#include "defines.h"
#include "Core/EntityComponentSystem/ECSEntity.hpp"
#include "Core/Memory/Allocator.hpp"

#include <typeinfo>
#include <vector>
#include <typeindex>


/**
 * @class ECSManager
 * @brief Templated Entity Component System
 *
 * Templated Entity Component System manager singleton. Holds entities,
 * components for these entities, and systems that operate on these
 * components.
 *
 * @todo TODO: How can we implement multiple components of the same typename for one entity?
 */
class ECSManager
{
private:
  /// @brief Vector of entities
  /// @todo TODO: Implement our own, faster vector?
  std::vector<Entity*> m_entities;

  /// @brief Map of component types, holding maps of components for each entity.
  /// @todo TODO: unsigned int -> const Entity&?
  std::unordered_map<std::type_index, std::unordered_map<U32, ComponentBase*>> m_components;

  /// @brief Vector of systems operating on entities
  /// @todo TODO: Implement our own, faster vector?
  std::vector<SystemBase*> m_systems;

public:
  /// Singleton getter
  static ECSManager& GetInstance();

  /// Creates a new entity
  Entity* CreateEntity();

  /// Destroys an entity with it's components
  void DestroyEntity(Entity* _entity);

  /**
   * @brief Creates and assigns a component to an entity
   *
   * Creates a new component and assigns it to already-existing Entity
   * object. It uses our own custom memory allocator.
   *
   * @todo TODO: Test for speed with different memory allocators
   *
   * @tparam C Component's typename <>
   * @param _entity Entity to assign the component to
   * @param _args Arguments to be passed to the component template
   */
  template <typename C, typename... Args>
  void AddComponent(Entity* _entity, Args&&... _args);

  /**
   * @brief Removes a component from entity
   * @tparam C Component's typename <>
   * @param _entity Entity to assign the component to
   */
  template <typename C>
  void RemoveComponent(Entity* _entity);

  /**
   * @brief Returns a component from entity
   * @tparam C Component's typename <>
   * @param _entity Entity to get a component from
   * @return Entity's component of typename C
   */
  template <typename C>
  C* GetComponent(Entity* _entity);

  /**
   * @brief Returns a map of components of one type for each entity
   * @tparam C Component's typename <>
   * @return Map of components of one type for each entity
   * @todo TODO: unsigned int to const Entity& ?
   */
  template <typename C>
  std::unordered_map<U32, C*> GetComponents();

  /**
   * @brief Checks if entity contains a component of type C
   * @tparam C Component's typename <>
   * @param _entity Entity to check the component for
   * @return boolean if entity has component of type C
   */
  template <typename C>
  B8 HasComponent(Entity* _entity);

  /**
   * @brief Creates and adds a system to our vector of systems
   * @tparam S System's typename <>
   * @param _args Any arguments for system's initialization
   * @return System that was created
   */
  template <typename S, typename... Args>
  S* AddSystem(Args&&... _args);

  /**
   * @brief Removes system from memory and system vector
   * @tparam S System's typename <>
   */
  template <typename S>
  void RemoveSystem();

  /**
   * @brief Updates all the systems
   * @param _deltaTime last frame's time
   * @todo TODO: change float to time object
   */
  void Update(F32 _deltaTime);

private:
  ECSManager();
  ~ECSManager();
};

#define ECS() \
  ECSManager::GetInstance()
