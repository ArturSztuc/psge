#include <gtest/gtest.h>
#include <Core/DataStructures/Queue.hpp>
#include <Core/DataStructures/LockFreeQueue.hpp>
#include <thread>

TEST(DataStructuresTests, QueueTests)
{
  // Create variable to test
  double par = 555.555;

  // Create a queue and see if it's empty
  psl::Queue<double> queue;
  EXPECT_TRUE(queue.Empty());
  EXPECT_EQ(queue.Size(), 0);

  // Push a parameter and raw double into the queue
  queue.Push(par);
  queue.Push(333.333);

  // Queue should have 2 values and not be empty
  EXPECT_EQ(queue.Size(), 2);
  EXPECT_FALSE(queue.Empty());

  // Pop the parameter and see if it has the right value 
  double ppar = queue.Pop();
  EXPECT_EQ(ppar, 555.555);
  // Pop second parameter and see id it has the right value
  EXPECT_EQ(queue.Pop(), 333.333);

  // The queue should be empty now
  EXPECT_TRUE(queue.Empty());
  EXPECT_EQ(queue.Size(), 0);
}


TEST(DataStructuresTests, QueueTestsMultithreaded)
{
  // Create a queue and see if it's empty
  psl::Queue<double> queue;

  std::thread pushThread([&queue]() {
      for (int i = 0; i < 100000; ++i) {
        queue.Push(i);
      }
  });

  std::thread popThread([&queue]() {
      for (int i = 0; i < 100000; ++i) {
        int element = queue.Pop();
        EXPECT_EQ(element, i);
      }
  });


  pushThread.join();
  popThread.join();
}

TEST(DataStructuresTests, LockFreeQueueTests)
{
  // Create variable to test
  double par = 555.555;

  std::cout << "W" << std::endl;
  // Create a queue and see if it's empty
  LockFreeQueue<double> lqueue;
  EXPECT_TRUE(lqueue.Empty());
  EXPECT_EQ(lqueue.Size(), 0);

  // Push a parameter and raw double into the queue
  lqueue.Push(par);
  lqueue.Push(333.333);

  // Queue should have 2 values and not be empty
  EXPECT_EQ(lqueue.Size(), 2);
  EXPECT_FALSE(lqueue.Empty());

  // Pop the parameter and see if it has the right value 
  double ppar = lqueue.Pop();
  EXPECT_EQ(ppar, 555.555);
  // Pop second parameter and see id it has the right value
  EXPECT_EQ(lqueue.Pop(), 333.333);

  // The queue should be empty now
  EXPECT_TRUE(lqueue.Empty());
  EXPECT_EQ(lqueue.Size(), 0);
}
