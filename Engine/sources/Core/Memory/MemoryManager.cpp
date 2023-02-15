#include "Core/Memory/MemoryManager.hpp"

MemoryManager& MemoryManager::GetInstance()
{
  static MemoryManager instance;
  return instance;
}

template <typename T>
T* MemoryManager::Allocate()
{
  std::type_index type = typeid(T);
  if(m_allocationCounts.find(type) == m_allocationCounts.end()){
    // create new pool for this object type
    m_allocationCounts[type] = 0;
  }
  m_allocationCounts[type]++;

  return GetPoolAllocator<T>().Allocate();
}

template <typename T>
void MemoryManager::Deallocate(T* ptr)
{
  std::type_index type = typeid(T);
  m_allocationCounts[type]--;
  GetPoolAllocator<T>().Deallocate(ptr);
}

size_t MemoryManager::GetAllocatorTypes() const
{
  return m_allocationCounts.size();
}

template <typename T>
size_t MemoryManager::GetAllocationCount()
{
  std::type_index type = typeid(T);
  size_t ret = 0;
  if(m_allocationCounts.find(type) != m_allocationCounts.end())
    ret = m_allocationCounts[type];
  return ret;
}

template<typename T>
PoolAllocator<T>& MemoryManager::GetPoolAllocator()
{
  static PoolAllocator<T> poolAllocator;
  return poolAllocator;
}
