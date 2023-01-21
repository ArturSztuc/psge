#pragma once

#include "Core/Event/EventTypes.h"
#include "defines.h"

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
  Type m_type;

  // Variable value union
  // TODO: Do we need to pad Bool to be 32bit?
  // TODO: What if we want 62bit?
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

// TODO: Either make all the data members private, or remove this GetType()
// TODO: Making data private can be tough, need a way to add EventVar arguments
class Event
{
  public:
    Event(EventType _type, U8 _numArgs);

    virtual ~Event();

    EventType GetType();

  protected:
    static const U32 MAX_ARGS = 4;
    EventType m_type;
    // TODO: not using this anywhere?
    U8        m_numArgs;
    EventVar  m_args[MAX_ARGS];
};

//class CollisionEvent : public Event
//{
//  public:
//    CollisionEvent(U32 _obj1, U32 _obj2);
//
//    U32 GetObject1() const;
//    U32 GetObject2() const;
//};
