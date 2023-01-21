#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

// pint-sized library
namespace psl
{

 // Concurrent queue
template <typename T>
class Queue
{
public:
  void Push(const T& _element)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_queue.push(_element);
    m_condition.notify_one();
  }

  T Pop()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condition.wait(lock, [this]() -> bool { return !m_queue.empty(); });
    T element = m_queue.front();
    m_queue.pop();
    return element;
  };

  bool Empty()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  };

  size_t Size() const
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
  }

private:
  std::queue<T> m_queue;
  mutable std::mutex m_mutex;
  std::condition_variable m_condition;
};
};
