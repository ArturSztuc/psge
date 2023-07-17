#include "Core/Threads/Task.hpp"

namespace psge
{
  Task::Task(Function _function)
  {
    /// @todo Need to make sure the task is being registered in some form of a task pool.
    /// @todo e.g. TaskManager::GetInstance().RegisterTask(std::ref(*this))
    m_function = std::move(_function);
    m_executed = false;
  }

  void Task::Execute()
  {
    // Execute all the functions in this task. This should be called from a
    // specific thread by a TaskManager, after resolving the dependencies
    m_function();

    m_executed = true;
  }

  void Task::Reset()
  {
    m_executed = false;
  }

  B8 Task::IsComplete()
  {
    return m_executed;
  }

  void Task::AddDependant(TaskPtr _dependant)
  {
    m_dependents.push_back(_dependant);
  }

  void Task::AddDependency(TaskPtr _dependency)
  {
    m_dependencies.push_back(_dependency);
    _dependency->AddDependant(shared_from_this());
    m_dependenciesCount++;
  }
} // namespace psge