#pragma once

#include <typeinfo>
#include <vector>
#include <typeindex>

#include "Core/EntityComponentSystem/ECSEntity.hpp"
#include "Core/Memory/Allocator.hpp"

class ECSManager
{
private:
  std::vector<Entity*> m_entities;
  std::unordered_map<std::type_index, std::unordered_map<unsigned int, ComponentBase*>> m_components;

  std::vector<SystemBase*> m_systems;

public:
  static ECSManager& GetInstance();

  Entity* CreateEntity();

  void DestroyEntity(Entity* _entity);

  template <typename C, typename... Args>
  void AddComponent(Entity* _entity, Args&&... _args);

  template <typename C>
  void RemoveComponent(Entity* _entity);

  template <typename C>
  C* GetComponent(Entity* _entity);

  template <typename C>
  std::unordered_map<unsigned int, C*> GetComponents();

  template <typename C>
  bool HasComponent(Entity* _entity);

  template <typename S, typename... Args>
  S* AddSystem(Args&&... _args);

  template <typename S>
  void RemoveSystem();

  // TODO: Change this to time object
  void Update(float _deltaTime);

private:
  ECSManager();
  ~ECSManager();
};

#define ECS() \
  ECSManager::GetInstance()
