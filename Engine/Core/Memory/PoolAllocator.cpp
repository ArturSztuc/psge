#include "Core/Memory/PoolAllocator.hpp"

template <typename T>
PoolAllocator<T>::PoolAllocator(std::size_t poolSize_)
  : m_poolSize(poolSize_) 
{
  // Allocate the memory pool
  m_memoryPool = static_cast<T*>(std::malloc(m_poolSize * sizeof(T)));
  // Initialize the free list with the memory blocks
  for (std::size_t i = 0; i < m_poolSize; ++i) {
    m_freeList.Push(&m_memoryPool[i]);
  }
}

template <typename T>
PoolAllocator<T>::~PoolAllocator()
{
  std::free(m_memoryPool);
}

template <typename T>
T* PoolAllocator<T>::Allocate()
{
  std::lock_guard<std::mutex> lock(m_mutex);

  // Check if there are any blocks left in the free list
  if (m_freeList.Empty()) {
    LERROR("no more memory blocks available. Implement resizable version of this!");
    throw std::bad_alloc();
  }

  T* block = m_freeList.Pop();

  // Return the block
  return block;
}

template <typename T>
void PoolAllocator<T>::Deallocate(T* block_) {
  std::lock_guard<std::mutex> lock(m_mutex);

  // Add the block back to the free list
  m_freeList.Push(block_);
}

template <typename T>
std::size_t PoolAllocator<T>::GetAllocationCount() const 
{
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_poolSize - m_freeList.Size();
}

template <typename T>
std::size_t PoolAllocator<T>::GetAllocationSize() const 
{
  size_t size = GetAllocationCount();
  return size * sizeof(T);
}

template <typename T>
std::size_t PoolAllocator<T>::GetFreeAllocationCount() const 
{ 
  return m_freeList.Size();
}

template <typename T>
std::size_t PoolAllocator<T>::GetFreeAllocationSize() const 
{ 
  return m_freeList.Size() * sizeof(T);
}

