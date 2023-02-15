/**
 * @file Queue.hpp
 * @brief A thread-safe overlay over std::queue.
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see Queue
 */
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

// pint-sized library. Remove this...
namespace psl
{

/**
 * @class Queue
 * @brief A thread-safe overlay over std::queue
 *
 * A very thread-safe overlay over std::queue that uses mutex locks to ensure
 * the safety. Locks are slow, however, so this should be used with it in mind.
 * This will hopefully be deprecated soon with LockFreeQueue (renamed to Queue)
 * that is lock-free but still thread-safe.
 */
template <typename T>
class Queue
{
public:
  /**
   * @brief Pushes an object into the queue, and expands if needed
   * @param _item templated object to push into the queue
   */
  void Push(const T& _element)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_queue.push(_element);
    m_condition.notify_one();
  }

  /**
   * @brief Pops the templated object from the queue's head
   * @return templated object that we obtain from the queue's head.
   */
  T Pop()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condition.wait(lock, [this]() -> bool { return !m_queue.empty(); });
    T element = m_queue.front();
    m_queue.pop();
    return element;
  };

  /// Returns if the queue is empty
  bool Empty()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  };

  /// Returns the queue size
  size_t Size() const
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
  }

private:
  /// Templated queue object
  std::queue<T> m_queue;

  /// Mutex used for locking the threads
  mutable std::mutex m_mutex;

  /// Condition object to see if locking is needed
  std::condition_variable m_condition;
};
};
