#pragma once

// Internal includes
#include "defines.h"
#include "Core/Threads/Task.hpp"
#include "Core/Logging/LogManager.hpp"

// Std includes
#include <queue>
#include <list>
#include <vector>
#include <unordered_map>
#include <condition_variable>
#include <thread>
#include <mutex>
/// @todo Remove strings...
#include <string>

namespace psge
{

using TaskPtr = std::shared_ptr<Task>;
using Function = std::function<void()>;
  
/**
 * @class TaskManager
 * @brief Task manager singleton, schedules and dispatches tasks
 */
class TaskManager
{

public:
  /// Singleton instance getter
  static TaskManager& GetInstance();

  /**
   * @brief Initialises the threads for TaskManager
   * 
   * @param _numThreads number of threads for concurrent task dispatching
   */
  void Initialize(U8 _numThreads);

  /// Makes the calss non-copyable and non-movable
  NOCOPY(TaskManager);

  /**
   * @brief Creates task graph and dispatches to threads
   * 
   * @param _deltaTime time passed since the last frame
   */
  void Update(F32 _deltaTime);

// Private member functions
private:
  /**
   * @brief Get the next Task to execute
   * 
   * @return TaskPtr pointer to task containing function to execute
   */
  TaskPtr GetNextTask();

  /// @brief Resets all the tasks
  void ResetTasks();

  /// @brief Builds the task graph
  void BuildTaskGraph();

  /// @brief Dispatches the tasks
  void DispatchTasks();

private:
  /// @brief Singleton constructor
  TaskManager();
  ~TaskManager();

  /// @brief A vector of threads that the tasks will be dispatched to
  std::vector<std::thread>  m_threads;

  U8 m_numThreads{1};

  /// @brief A list of tasks to execute concurrently
  /// @todo Change to our internal queue?
  std::list<TaskPtr> m_tasks;

  /// @brief Mutex for locking the queue
  std::mutex m_queueMutex;

  /// @brief Mutex for locking the graph
  std::mutex m_graphMutex;

  std::condition_variable m_condition;

  /// @brief Bool deciding if we should stop updating/dispatching tasks
  /// @todo should use this more...
  B8 m_shouldStop;

  /// @brief Graph of tasks to execute
  std::unordered_map<TaskPtr, int> m_graph;
};

};