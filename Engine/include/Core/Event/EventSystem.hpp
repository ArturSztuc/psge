/**
 * @file EventSystem.hpp
 * @brief File holding the event system class
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
  * @see EventSystem
 */
#pragma once 

#include "defines.h"
#include "Core/Event/EventTypes.h"
#include "Core/Event/Event.hpp"
#include "Core/DataStructures/Queue.hpp"
#include "Core/Logging/LogManager.hpp"

#include <algorithm>
#include <functional>
#include <map>

using EventFunction = std::function<void(const Event&)>;

/**
 * @class EventSystem
 * @brief System registering and dispatching events
 */
class EventSystem
{
public:
  /// Singleton instance getter
  static EventSystem& GetInstance();

  /// Makes the calss non-copyable and non-movable
  NOCOPY(EventSystem);

  /**
   * @brief Subscribes functions and objects to event type
   * @param _type type of the event @see EventType
   * @param _handler function that handles the event.
   */
  // TODO: This will cause iteration over all the objects registered to the type of event. Not good...
  void Subscribe(const EventType& _type,
                 EventFunction&& _handler);

  /**
   * @brief Sends the events to a queue for dispatching
   * @param _event Event to dispatch to the subscribed functions
   */
  void SendEvent(const Event& _event);

  /**
   * @brief Dispatches the queued events to the subscribed function handlers
   * @todo TODO: Do we need time too?
   */
  void Update();

private:
  /**
   * @brief Default constructor, private to make it a singleton
   *
   * For now it is empty, and it might stay that way.
   */
  EventSystem();
  /// Map of event types and vectors of subscribed handlers for each event type
  std::map<EventType, std::vector<EventFunction>> m_handlers;

  /// The event queue
  psl::Queue<Event> m_queue;
};

/// Subscribes an object to handle a type of an event with specified function
#ifndef EVENT_REGISTER
#define EVENT_REGISTER(type, func, obj) EventSystem::GetInstance().Subscribe(type,\
    std::bind(&func, &obj, std::placeholders::_1));
#endif

/// Sends the event to a queue for dispatching.
#ifndef EVENT_FIRE
#define EVENT_FIRE(event) EventSystem::GetInstance().SendEvent(event);
#endif
