#pragma once

// Internal includes
#include "defines.h"

// Std includes
#include <vector>
#include <memory>
#include <functional>

namespace psge
{


/**
 * @class Task
 * @brief Creates a task to be multithreaded
 * 
 * Tasks are functions to be executed on thread pool. These taks can depend upon
 * the completion of other tasks, and can contain multiple functions to be
 * executed one straight after another on the same thread e.g. if they are
 * relatively small.
 * 
 * @todo Add Task ID? But new tasks would have to be made via some central manager then.
 */
class Task
{
  using TaskPtr = std::shared_ptr<Task>;
  using Function = std::function<void()>;
  public:
    /**
     * @brief Construct a new Task to be added to the graph
     * 
     * @param _function Function that corresponds to the task
     */
    explicit Task(Function _function);

    /**
     * @brief Executes the tasks
     */
    void Execute();

    /**
     * @brief Adds a pointer to a task that needs to be executed prior to this one
     * 
     * @param _dependency Pointer to a task to be executed prior to this one
     */
    void AddDependency(TaskPtr _dependency);

    /**
     * @brief Adds another function to this task
     * 
     * @param _function Function pointer to add to this Task
     */
    void AddFunction(Function _function);

    /**
     * @brief Get all the thasks that need to be executed prior to this one
     * @todo TODO: Could this be a const vector pointer of TaskPtrs?
     * 
     * @return std::vector<TaskPtr> Vector of task pointers 
     */
    std::vector<TaskPtr> GetDependencies() { return m_dependencies; };


  private:
    /// @brief Vector of functions that correspond to this Task
    std::vector<Function> m_functions;

    /// @brief Vector of tasks that this one depends upon
    std::vector<TaskPtr> m_dependencies;
};


  
}; // namespace psge
