/**
 * @file Event.hpp
 * @brief Contains event class used for messaging
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see EventVar
 * @see Event
 */
#pragma once

#include "Core/Event/EventTypes.h"
#include "defines.h"

/**
 * @struct EventVar
 * @brief Event's data holder -- union
 */
struct EventVar
{
  // TODO: Add string... needs to be our own?
  enum Type
  {
    TYPE_UINTEGER,
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_BOOL,
    // Number of unique  types
    TYPE_COUNT
  };

  // Type of cariable
  // TODO: Currently not used. Make all of these private and write make GetAsType functions? With SetAsType and GetTyoe?
  Type m_type;

  // Variable value union
  // TODO: Do we need to pad Bool to be 32bit?
  // TODO: What if we want 64bit?
  // TODO: Add string somehow?
  union
  {
    U64 m_U64[2];
    I64 m_I64[2];
    F64 m_F64[2];

    U32 m_U32[4];
    I32 m_I32[4];
    F32 m_F32[4];

    U16 m_U16[8];
    I16 m_I16[8];

    U8  m_U8[16];
    I8  m_I8[16];
    B8  m_B8[16];
  };
};

/**
 * @class Event
 * @brief Event used for messaging
 *
 * Event class used with the event system for sending messages and events
 *
 * @todo TODO: Either make all the data members private, or remove this GetType()
 * @todo TODO: Making data private can be tough, need a way to add EventVar arguments
 * @todo TODO: Probably need some form of bool to see if the event was handled, and if so, remove it from the memory.
 */
class Event
{
  public:
    /**
     * @brief Event constructor
     * @param _type type of event. @see EventType
     * @param _numArgs number of data arguments in this event
     */
    Event(EventType _type, U8 _numArgs);

    /// Default deconstructor
    virtual ~Event();

    /**
     * @brief Returns the event's type
     * @return Event's type @see EventType
     */
    EventType GetType();

  protected:
    /// Maximum number of arguments in the Event.
    static const U32 MAX_ARGS = 4;

    /// Event type
    EventType m_type;

    /// Number of arguments in this event
    /// @todo TODO: not using this anywhere?
    U8        m_numArgs;

    /// Data holder for this event
    EventVar  m_args[MAX_ARGS];
};
