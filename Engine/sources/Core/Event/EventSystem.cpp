#include "Core/Event/EventSystem.hpp"

EventSystem& EventSystem::GetInstance()
{
    static EventSystem instance;
    return instance;
};

  void EventSystem::Subscribe(const EventType& _type,
                              EventFunction&& _handler)
  {
    m_handlers[_type].push_back(_handler);
  };

void EventSystem::SendEvent(const Event& _event)
{
  m_queue.Push(_event);
}

void EventSystem::Update()
{
  bool noEvents = false;
  while (!noEvents){

    if(m_queue.Empty()){
      noEvents = true;
      continue;
    }

    // TODO: Should we implement a separate type of event loop for ECS?
    // TODO: Perhaps the handlers could also actually be whole system themselves, which will progress the event to objects that are actually specified in the event itself (if applicable)
    Event event = m_queue.Pop();
    for(auto&& handler : m_handlers[event.GetType()]){
      handler(event);
    }
  }
}
