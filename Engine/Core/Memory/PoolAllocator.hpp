#pragma once
#include <memory>
#include <mutex>
#include <new>
#include <typeindex>

#include "Core/Logging/LogManager.hpp"
#include "Core/DataStructures/Queue.hpp"

template <typename T>
class PoolAllocator
{
// Private data members
private:
 // The size of the memory pool
 std::size_t m_poolSize;

 // The memory pool
 T* m_memoryPool;

 // The list of free blocks
 psl::Queue<T*> m_freeList;

 // Mutex for thread-safety
 mutable std::mutex m_mutex;

 // Pujblic data functions
public:
  // Constructor that initializes the memory pool with the given size
  explicit PoolAllocator(std::size_t poolSize_ = 1024);

  // Destructor that frees the memory pool
  ~PoolAllocator();
  
  // Allocates a block of memory from the pool
  T* Allocate();
  
  // Deallocates a block of memory back to the pool
  void Deallocate(T* block_);
  
  // Returns the number of blocks currently allocated
  std::size_t GetAllocationCount() const;

  std::size_t GetAllocationSize() const;

  std::size_t GetFreeAllocationCount() const;

  std::size_t GetFreeAllocationSize() const;
};

