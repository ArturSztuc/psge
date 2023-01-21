#pragma once

#include <unordered_map>
#include <typeinfo>

class ComponentBase
{
public:
  virtual ~ComponentBase() = default;
};

class SystemBase
{
public:
  virtual ~SystemBase() = default;

  // TODO: Change to time object
  virtual void Update(float _deltaTime) = 0;
};

class Entity
{
private:
  /// Unique identifier for the entity
  unsigned int m_id;

  /// Static counter for generating unique eneity IDs
  static unsigned int m_nextId;

public:
  Entity();

  ~Entity() = default;

  unsigned int GetId() const;

  static void ClearEntities();
};

