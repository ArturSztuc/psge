#include "Core/EntityComponentSystem/ECSManager.hpp"

#include <algorithm>
#include <type_traits>
#include <utility>

ECSManager& ECSManager::GetInstance()
{
  static ECSManager instance;
  return instance;
}

ECSManager::ECSManager()
{
};

ECSManager::~ECSManager()
{
  // TODO: Need to delete components too!
  //for(auto& entity : m_entities){
  //  DestroyEntity(entity);
  //}

  //for(auto& system : m_systems)
  //  delete system;
};

Entity* ECSManager::CreateEntity()
{
  Entity* entity = MEMALLOC().Allocate<Entity>();
  *entity = Entity();

  m_entities.push_back(entity);

  return entity;
}

void ECSManager::DestroyEntity(Entity* _entity)
{
  for(auto& componentArray : m_components){
    ComponentBase* bb = (componentArray.second[_entity->GetId()]);
    MEMALLOC().Deallocate(bb);
    componentArray.second.erase(_entity->GetId());
  }

  // TODO: Check if there are faster ways...
  MEMALLOC().Deallocate(_entity);
  auto it = std::find(m_entities.begin(), m_entities.end(), _entity);
  if(it != m_entities.end())
    m_entities.erase(it);
}

template <typename C, typename... Args>
void ECSManager::AddComponent(Entity* _entity, Args&&... _args)
{
  // TODO: Insert our own assert
  static_assert(std::is_base_of<ComponentBase, C>::value, "Component must be derived from ComponentBase!");

  // TODO: Use our own memory manager
  //C* component = MEMALLOC().Allocate<C>(std::forward<Args>(args_)...);
  C* component = MEMALLOC().Allocate<C>();
  *component = C(std::forward<Args>(_args)...);
  m_components[typeid(C)][_entity->GetId()] = component;
  //m_components[typeid(C)][entity_->GetId()] = new C(std::forward<Args>(args_)...);
};

template <typename C>
void ECSManager::RemoveComponent(Entity* _entity)
{
  static_assert(std::is_base_of<ComponentBase, C>::value, "Component must be derived from ComponentBase!");

  // TODO: Check if it should not be the other way round, like in deleting entities...
  delete m_components[typeid(C)][_entity->GetId()];
  m_components[typeid(C)].erase(_entity->GetId());
}

template <typename C>
C* ECSManager::GetComponent(Entity* _entity)
{
  static_assert(std::is_base_of<ComponentBase, C>::value, "Component must be derived from ComponentBase!");
  return static_cast<C*>(m_components[typeid(C)][_entity->GetId()]);
}

template <typename C>
std::unordered_map<U32, C*> ECSManager::GetComponents()
{
  std::unordered_map<U32, C*> derivedMap;
  for(auto& [key, value] : m_components[typeid(C)]){
    derivedMap[key] = static_cast<C*>(value);
  }

  return derivedMap;
}

template <typename C>
B8 ECSManager::HasComponent(Entity* _entity)
{
  static_assert(std::is_base_of<ComponentBase, C>::value, "Component must be derived from ComponentBase!");

  return m_components[typeid(C)].count(_entity->GetId()) > 0;
}

template <typename S, typename... Args>
S* ECSManager::AddSystem(Args&&... _args)
{
  static_assert(std::is_base_of<SystemBase, S>::value, "System must be derived from SystemBase!");

  // TODO: Figure out the memory allocator...
  // Not worth using our own memory manager because it will create 1024 (or
  // less/more) e.g. PhysicsSystem objects when we only need one. Shall we
  // introduce some other allocator for these? Effectively singletons...
  S* system = new S(std::forward<Args>(_args)...);

  m_systems.push_back(system);
  return system;
}

template <typename S>
void ECSManager::RemoveSystem()
{
  static_assert(std::is_base_of<SystemBase, S>::value, "System must be derived from SystemBase!");
  auto it = std::find_if(m_systems.begin(), m_systems.end(), [](SystemBase* system)
      {
      return dynamic_cast<S*>(system) != nullptr;
      });
  if(it != m_systems.end())
  {
    // TODO: Check if the order is right
    delete *it;
    m_systems.erase(it);
  }
}

void ECSManager::Update(F32 _deltaTime)
{
  for(auto& system : m_systems)
    system->Update(_deltaTime);
}
