#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <iostream>

#include "defines.h"

namespace psge
{

/**
 * @class ThreadWorker
 * @brief Class that executes a given function on a separate thread
 * 
 * @todo TODO: Perhaps we should have a ThreadManager which distributes functions across threads.
 */
class ThreadWorker
{
private:
  std::atomic<bool> m_shouldRun;
  std::unique_ptr<std::thread> m_thread;
  std::function<void(std::atomic<bool>&)> m_threadFunction;

public:
  explicit ThreadWorker(std::function<void(std::atomic<bool>&)> _shouldRun);

  /// Makes the calss non-copyable and non-movable
  NOCOPY(ThreadWorker);

  void StartThread(const S32& _threadName);
  void StopThread();
  bool ThreadRunning() { return m_shouldRun.load(); };

  // std::unique_ptr<std::thread> GetThread();
};

} // namespace psge