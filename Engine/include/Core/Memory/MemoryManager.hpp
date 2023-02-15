#pragma once
#include <memory>
#include <mutex>
#include <new>
#include <typeindex>

#include "Core/Logging/LogManager.hpp"
#include "Core/DataStructures/Queue.hpp"

#include "Core/Memory/PoolAllocator.hpp"


class MemoryManager
{
// Private data members
private:
  std::unordered_map<std::type_index, std::size_t> m_allocationCounts;

// Public data members
public:

// Public function members
public:
  static MemoryManager& GetInstance();

  template <typename T>
  T* Allocate();

  // TODO: Write some test to see if there are any objects in this pool
  template <typename T>
  void Deallocate(T* ptr);

  size_t GetAllocatorTypes() const;

  template <typename T>
  size_t GetAllocationCount();

// Private functions members
private:
  MemoryManager(){};
  ~MemoryManager(){};

  template<typename T>
  PoolAllocator<T>& GetPoolAllocator();

};

#define MEMALLOC() \
  MemoryManager::GetInstance()
