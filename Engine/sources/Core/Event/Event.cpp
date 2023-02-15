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
