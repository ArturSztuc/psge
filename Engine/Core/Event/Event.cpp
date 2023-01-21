#include "Core/Event/Event.hpp"
#include "Core/Logging/LogManager.hpp"

Event::Event(EventType _eventType, U8 _numArgs)
  : m_type(_eventType), m_numArgs(_numArgs)
{
  if(m_numArgs > MAX_ARGS)
  {
    LERROR("Event passed with number of arguments larger than MAXARGS");
    // TODO: change this to our own ASSERT or add some BREAK 
  }
};

Event::~Event()
{
};

EventType Event::GetType()
{
  return m_type;
}

//CollisionEvent::CollisionEvent(U32 _obj1, U32 _obj2)
//  :Event(EVENT_TYPE_COLLISION, 1)
//{
//  m_args[0].m_U32[0] = _obj1;
//  m_args[0].m_U32[1] = _obj2;
//}
//
//U32 CollisionEvent::GetObject1() const 
//{ 
//  return m_args[0].m_U32[0];
//};
//
//U32 CollisionEvent::GetObject2() const 
//{ 
//  return m_args[0].m_U32[1]; 
//};
