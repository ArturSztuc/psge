#pragma once 

#include <algorithm>
#include <functional>
#include <map>

#include "Core/Event/EventTypes.h"
#include "Core/Event/Event.hpp"
#include "Core/DataStructures/Queue.hpp"
#include "Core/Logging/LogManager.hpp"
#include "defines.h"

using EventFunction = std::function<void(const Event&)>;

class EventSystem
{
public:
  static EventSystem& GetInstance();

  // TODO: This will cause iteration over all the objects registered to the type of event. Not good...
  void Subscribe(const EventType& _type,
                 EventFunction&& _handler);

  void SendEvent(const Event& _event);

  // TODO: Do we need time too?
  void Update();

private:
  std::map<EventType, std::vector<EventFunction>> m_handlers;
  psl::Queue<Event> m_queue;
};

// Subscribes an object to handle a type of an event with specified function
#ifndef EVENT_REGISTER
#define EVENT_REGISTER(type, func, obj) EventSystem::GetInstance().Subscribe(type,\
    std::bind(&func, &obj, std::placeholders::_1));
#endif

// Fires an event.
#ifndef EVENT_FIRE
#define EVENT_FIRE(event) EventSystem::GetInstance().SendEvent(event);
#endif

