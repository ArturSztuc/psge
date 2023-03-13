#include "Core/Threads/Task.hpp"

namespace psge
{
  Task::Task(Function _function)
  {
    m_functions.push_back(std::move(_function));
  }

  void Task::Execute()
  {
    // Execute all the functions in this task. This should be called from a
    // specific thread by a TaskManager, after resolving the dependencies
    for(Function f : m_functions)
      f();
  }

  void Task::AddDependency(TaskPtr _dependency)
  {
    m_dependencies.push_back(_dependency);
  }

  void Task::AddFunction(Function _function)
  {
    m_functions.push_back(_function);
  }

} // namespace psge