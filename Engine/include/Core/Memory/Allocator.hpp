#pragma once
#include <memory>
#include <mutex>
#include <new>
#include <typeindex>
#include <cxxabi.h>

#include "Core/Logging/LogManager.hpp"
#include "Core/DataStructures/Queue.hpp"

template <typename T>
class PoolAllocator
{
public:
  // Constructor that initializes the memory pool with the given size
  explicit PoolAllocator(std::size_t poolSize_ = 1024) : m_poolSize(poolSize_) {
    // Allocate the memory pool
    m_memoryPool = static_cast<T*>(std::malloc(m_poolSize * sizeof(T)));
    // Initialize the free list with the memory blocks
    for (std::size_t i = 0; i < m_poolSize; ++i) {
      m_freeList.Push(&m_memoryPool[i]);
    }
  }
  
  // Destructor that frees the memory pool
  ~PoolAllocator() {
    std::free(m_memoryPool);
  };
  
  // Allocates a block of memory from the pool
  T* Allocate() {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Check if there are any blocks left in the free list
    if (m_freeList.Empty()) {
     LOG_ERROR("general", "no more memory blocks available. Implement resizable version of this!");
      throw std::bad_alloc();
    }

    T* block = m_freeList.Pop();

    // Return the block
    return block;
  }
  
  // Deallocates a block of memory back to the pool
  void Deallocate(T* block_) 
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Add the block back to the free list
    m_freeList.Push(block_);
  }
  
  // Returns the number of blocks currently allocated
  std::size_t GetAllocationCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_poolSize - m_freeList.Size();
  }

  std::size_t GetAllocationSize() const {
    size_t size = GetAllocationCount();
    return size * sizeof(T);
  }

  std::size_t GetFreeAllocationCount() const { 
    return m_freeList.Size();
  }

  std::size_t GetFreeAllocationSize() const { 
    return m_freeList.Size() * sizeof(T);
  }

private:
 // The size of the memory pool
 std::size_t m_poolSize;

 // The memory pool
 T* m_memoryPool;

 // The list of free blocks
 psl::Queue<T*> m_freeList;

 // Mutex for thread-safety
 mutable std::mutex m_mutex;
};

class MemoryManager
{
public:
  static MemoryManager& GetInstance()
  {
    static MemoryManager instance;
    return instance;
  };

  template <typename T>
  T* Allocate()
  {
    std::type_index type = typeid(T);
    if(m_allocationCounts.find(type) == m_allocationCounts.end()){
      // create new pool for this object type
      m_allocationCounts[type] = 0;
    }
    m_allocationCounts[type]++;

    return GetPoolAllocator<T>().Allocate();
  };

  // TODO: Write some test to see if there are any objects in this pool
  template <typename T>
  void Deallocate(T* ptr)
  {
    std::type_index type = typeid(T);
    m_allocationCounts[type]--;
    GetPoolAllocator<T>().Deallocate(ptr);
  };

  size_t GetAllocatorTypes() const { return m_allocationCounts.size(); };

  void Print()
  {
    for(auto &alloc : m_allocationCounts)
    {
      int status;
      char * demangled = abi::__cxa_demangle(alloc.first.name(),0,0,&status);
      std::cout <<  demangled << " : " << alloc.second << std::endl;
    }
  };

  template <typename T>
  size_t GetAllocationCount()
  {
    std::type_index type = typeid(T);
    size_t ret = 0;
    if(m_allocationCounts.find(type) != m_allocationCounts.end())
      ret = m_allocationCounts[type];
    return ret;
  }

private:
  MemoryManager(){};
  ~MemoryManager(){};

  template<typename T>
  PoolAllocator<T>& GetPoolAllocator()
  {
    static PoolAllocator<T> poolAllocator;
    return poolAllocator;
  }

  std::unordered_map<std::type_index, std::size_t> m_allocationCounts;
};

#define MEMALLOC() \
  MemoryManager::GetInstance()
