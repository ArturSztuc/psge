#include "Core/EntityComponentSystem/ECSEntity.hpp"

Entity::Entity()
  : m_id(m_nextId++)
{
}

unsigned int Entity::GetId() const
{
  return m_id;
}

void Entity::ClearEntities()
{
  m_nextId = 0;
}


unsigned int Entity::m_nextId = 0;
