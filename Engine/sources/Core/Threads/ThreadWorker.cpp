#include "Core/Threads/ThreadWorker.hpp"

namespace psge 
{
ThreadWorker::ThreadWorker(std::function<void(std::atomic<bool>&)> _do_work)
  : m_shouldRun(false), m_thread(nullptr), m_threadFunction(_do_work)
{
}

void ThreadWorker::StartThread(const S32&  _name)
{
  if(ThreadRunning())
    throw std::runtime_error("Attempted to start thread when it's already running!");

  m_shouldRun= true;
  m_thread.reset(new std::thread([&]{m_threadFunction(std::ref(m_shouldRun)); }));

  auto handle = m_thread->native_handle();
  auto rc = pthread_setname_np(handle, _name.Data());

  if(rc != 0){
    std::cout << "The name \"" << _name << "\" is too long!" << std::endl;
  }

}

void ThreadWorker::StopThread()
{
  if(!ThreadRunning())
    throw std::runtime_error("Attempted to stop thread that isnot running!");

  m_shouldRun = false;

  if(!m_thread->joinable())
    throw std::runtime_error("Thread is not in a joinable state during thread stop!");

  try{
    m_thread->join();
  }
  catch ( std::system_error const& e){
    throw std::runtime_error((std::string("Error while joining the thread, ") + e.what()).c_str());
  }
}

}; // namespace psge
