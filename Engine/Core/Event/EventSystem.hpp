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
  void RegisterHandler(EventType _type,
                       EventFunction _handler);

  void SendEvent(const Event& _event);

  void Update();

private:
  std::map<EventType, std::vector<EventFunction>> m_handlers;
  psl::Queue<Event> m_queue;
};

//class GameObject
//{
//  public:
//    GameObject(int id_) : m_id(id_) {};
//
//    void HandleCollision(const Event& _event)
//    {
//      const CollisionEvent& collision = static_cast<const CollisionEvent&>(_event);
//      if(collision.GetObject1() == m_id || collision.GetObject2() == m_id){
//      }
//    };
//
//  private:
//    int m_id;
//};
//
//
