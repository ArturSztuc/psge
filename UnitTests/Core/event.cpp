#include <gtest/gtest.h>
#include <Core/Event/EventSystem.hpp>

#include <iostream>


class CollideEvent : public Event
{
public:
  CollideEvent(U32 _id1, U32 _id2, F32 _m1, F32 _m2, F32 _v1, F32 _v2)
    :Event(EVENT_TYPE_COLLISION, 2)
  {
    // Object IDs
    m_args[0].m_U32[0] = _id1;
    m_args[0].m_U32[1] = _id2;

    // Object 1 mass and 1D velocity
    m_args[1].m_F32[0] = _m1;
    m_args[1].m_F32[1] = _v1;

    // Object 2 mass and 1D velocity
    m_args[1].m_F32[2] = _m2;
    m_args[1].m_F32[3] = _v2;
  }

  U32 GetId1() const { return m_args[0].m_U32[0]; };
  U32 GetId2() const { return m_args[0].m_U32[1]; };

  F32 GetMass1()  const { return m_args[1].m_F32[0]; };
  F32 GetSpeed1() const { return m_args[1].m_F32[1]; };
  F32 GetMass2()  const { return m_args[1].m_F32[2]; };
  F32 GetSpeed2() const { return m_args[1].m_F32[3]; };
};

struct Object
{
  U32 m_id;
  F32 m_mass;
  F32 m_speed;

  // Handler! This could be e.g. OnEvent etc.
  void HandleCollision(const Event& _event)
  {
    // Cast to a collision object
    const CollideEvent& collision = static_cast<const CollideEvent&>(_event);

    // Don't continue if wrong object chosen
    if(!(collision.GetId1() == m_id || collision.GetId2() == m_id )){
      // TODO: Ideally we would want to implement the event system in a way that this will never ever be triggered...
      std::cout << "Ideally you would never see this message with well-implemented event system!" << std::endl;
      return;
    }

    // Calculate the change in speed after the collision
    F32 mSum = (collision.GetMass1() + collision.GetMass2());
    if(collision.GetId1() == m_id){
      F32 mDiff = (collision.GetMass1() - collision.GetMass2());
      m_speed = (mDiff/mSum)*collision.GetSpeed1() + (2.0f*collision.GetMass2()/mSum)*collision.GetSpeed2();
    }
    else if(collision.GetId2() == m_id){
      F32 mDiff = (collision.GetMass2() - collision.GetMass1());
      m_speed = (mDiff/mSum)*collision.GetSpeed2() + (2.0f*collision.GetMass1()/mSum)*collision.GetSpeed1();
    }
  }
};

TEST(EventSystemTests, EventTests)
{
  CollideEvent collide(0, 1, 10.0f, 20.0f, 10.0f, -20.0f);

  EXPECT_EQ(collide.GetId1(), 0);
  EXPECT_EQ(collide.GetId2(), 1);

  EXPECT_EQ(collide.GetMass1(), 10);
  EXPECT_EQ(collide.GetMass2(), 20);

  EXPECT_EQ(collide.GetSpeed1(), 10);
  EXPECT_EQ(collide.GetSpeed2(), -20);
}

TEST(EventSystemTests, EventSystemTests)
{
  // Create a few objects with specific masses
  Object obj1;
  Object obj2;
  obj1.m_id = 0;
  obj2.m_id = 1;
  obj1.m_speed = 10;
  obj2.m_speed = -20;

  Object obj3;
  obj3.m_id = 3;
  Object obj4;
  obj4.m_id = 4;
  Object obj5;
  obj5.m_id = 5;

  // Register the event handler. Perhaps this could be done inside of the class
  // itself? Constructor? Created object would register itself?
  EVENT_REGISTER(EVENT_TYPE_COLLISION, Object::HandleCollision, obj1);
  EVENT_REGISTER(EVENT_TYPE_COLLISION, Object::HandleCollision, obj2);
  EVENT_REGISTER(EVENT_TYPE_COLLISION, Object::HandleCollision, obj3);
  EVENT_REGISTER(EVENT_TYPE_COLLISION, Object::HandleCollision, obj4);
  EVENT_REGISTER(EVENT_TYPE_COLLISION, Object::HandleCollision, obj5);

  // Create collide-type event
  CollideEvent collide(0, 1, 10, 20, 10, -20);

  // Fire the event
  EVENT_FIRE(collide);

  // Update a few times
  EventSystem::GetInstance().Update();
  EventSystem::GetInstance().Update();
  EventSystem::GetInstance().Update();
  EventSystem::GetInstance().Update();

  //EXPECT_NEAR(obj1.m_speed, -30.0, 0.00001);
  EXPECT_FLOAT_EQ(obj1.m_speed, -30.0);

  EXPECT_EQ(obj2.m_speed, 0.0);
}
