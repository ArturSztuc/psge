#include "Core/Threads/TaskManager.hpp"

namespace psge
{

TaskManager::TaskManager()
{
}

TaskManager::~TaskManager()
{
  // Lock the queue and issue stop
  {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_shouldStop = true;
  }
  m_condition.notify_all();

  // Join the worker threads
  for(std::thread& thread : m_threads){
    thread.join();
  }
}

TaskManager& TaskManager::GetInstance()
{
  static TaskManager instance;
  return instance;
}

void TaskManager::Initialize(U8 _numThreads)
{
  m_numThreads = _numThreads;
  for(U8 idx = 0; idx < m_numThreads; ++idx){
    m_threads.emplace_back([this](){
      while(true){
        // Get the next task
        TaskPtr task = GetNextTask();

        // Break if no tasks left
        if(!task)
          break;

        // Break if task was already executed
        if(task->IsComplete())
          break;

        // Execute the task
        task->Execute();
      }
    });
  }

  LINFO("Initialized the task manager with %i threads!", _numThreads);
}

TaskPtr TaskManager::GetNextTask()
{
  // Lock the thread
  std::unique_lock<std::mutex> lock(m_queueMutex);
  m_condition.wait(lock, [this](){
    return m_shouldStop || !m_tasks.empty();
  });

  // Return nothing if no tasks left
  if(m_shouldStop && m_tasks.empty())
    return nullptr;
  
  // Get a task and remove from the queue
  TaskPtr task = m_tasks.front();
  if(task->m_dependenciesCount == 0 && !task->IsComplete()){
    // Remove this task from dependencies of other tasks...
    for(const TaskPtr& dependent : task->m_dependents){
      /// @todo should we remove it from the vector too though? :|
      --dependent->m_dependenciesCount;
    }
  }

  // Remove this task from our todo list
  m_tasks.erase(m_tasks.begin());

  // Return the task
  return std::move(task);
}

void TaskManager::ResetTasks()
{
  // Lock the threads
  std::unique_lock<std::mutex> lock(m_graphMutex);

  // Re-load the dependencies count
  for(const TaskPtr& task : m_tasks){
    task->m_dependenciesCount = task->m_dependencies.size();
    task->m_executed = false;
  }
}

void TaskManager::BuildTaskGraph()
{
  // Lock and clear the graph
  std::unique_lock<std::mutex> lock(m_graphMutex);
  m_graph.clear();

  // Dependencies per task
  for(const TaskPtr& task : m_tasks){
    m_graph[task] = task->m_dependenciesCount.load();
  }
}

void TaskManager::DispatchTasks()
{
  std::unique_lock<std::mutex> lock(m_queueMutex);

  // Find all the tasks with no remaining dependencies
  std::vector<TaskPtr> readyTasks;
  for(const auto& entry : m_graph){
    if(entry.second == 0)
      readyTasks.push_back(entry.first);
  }

  // Dispatch tasks to the worker threads
  for(const auto& task : readyTasks){
    m_tasks.push_back(task);
  }
  m_condition.notify_all();
}

void TaskManager::Update(F32 _deltaTime)
{
  // Reset the task states
  ResetTasks();

  // Build the task graph
  BuildTaskGraph();

  // Dispatch tasks to worker threads
  DispatchTasks();
}

};