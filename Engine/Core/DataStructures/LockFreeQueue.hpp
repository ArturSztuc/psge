/**
 * @file LockFreeQueue.hpp
 * @brief A custom Queue class that has no multithreading locks
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-21
 *
 * @see LockFreeQueue
 */
#pragma once

#include <stdexcept>
#include <iostream>

/**
 * @class LockFreeQueue
 * @brief A queue class without multithreading locks
 *
 * This will become the default queue class once more unit tests are written
 * and passed, whereas the currently-used "Queue" class (based on std::queue)
 * will be removed completely.
 * This LockFreeQueue class should work with multi-threading, but does not have
 * locks. This will make it faster, but will also mean that if e.g. pushing AND
 * popping are multi-threaded, the popping won't be necessarily in the same
 * order as pushing. Need to investigate further to see when this breaks down.
 *
 * @todo TODO: Need to use our custom memory allocators!
 * @todo TODO: Need to implement CountNodes() to compare against Size() etc.
 */
template <typename T>
class LockFreeQueue {
private:
  struct Node 
  {
    T item;
    Node* next;
    Node(T item) : item(item), next(nullptr) {}
  };

  /// Head node of the queue
  Node* m_head;

  /// Tail node of the queue
  Node* m_tail;

  /// The current size of the queue
  /// @todo TODO: Do we need std::atomic here?
  size_t m_size;

public:
  /// Default constructor
  LockFreeQueue() 
  {
    Node* sentinel = new Node(T());
    m_head = m_tail = sentinel;
    m_size = 0;
  };

  /// Default deconstructor -- clears RAM
  ~LockFreeQueue() 
  {
    while (m_head != nullptr) {
      Node* node = m_head;
      m_head = node->next;
      delete node;
    }
    m_size = 0;
  };

  /**
   * @brief Pushes an object into the queue, and expands if needed
   * @param _item templated object to push into the queue
   */
  void Push(T _item) 
  {
    // Create a new node
    Node* node = new Node(_item);

    while (true) {
      // Pointer to the tail
      Node* tail = m_tail;

      // Pointer to where we want to push the item
      Node* next = tail->next;

      // TODO: Need some assert here...
      if (tail != m_tail)
        continue;

      // If the space we want is empty, we swap the node with next
      if (next == nullptr) {
        if (__sync_bool_compare_and_swap(&tail->next, next, node)) {
          __sync_bool_compare_and_swap(&m_tail, tail, node);
          m_size++;
          return;
        }
      } else {
        // If the space is not empty then we re-define it as the next tail.
        // TODO: Need some asserts/warnings here?
        __sync_bool_compare_and_swap(&m_tail, tail, next);
      }
    }
  };

  /**
   * @brief Pops the templated object from the queue's head
   *
   * Pops the templated object from the queue's head, and re-defines where the
   * new head and tail are.
   *
   * @return templated object that we obtain from the queue's head.
   */
  T Pop() 
  {
    while (true) {
      Node* head = m_head;
      Node* tail = m_tail;
      Node* next = head->next;

      // This should never happen, but can do when multithreading breaks.
      // TODO: Use our own error handlers...
      if (head != m_head)
        throw std::underflow_error("We got head != m_head, which is strange");

      // Cannot pop if the queue is empty.
      if(m_size == 0)
        throw std::underflow_error("Queue is empty!");

      // Can happen if the size is 1? Need to test when this is called.
      if (head == tail) {
        __sync_bool_compare_and_swap(&m_tail, tail, next);
        continue;
      }

      // Shouldn't this be head->item? Test!
      T item = next->item;
      if (__sync_bool_compare_and_swap(&m_head, head, next)) {
        // Finally, reduce the size, delete the head and return the item.
        m_size--;
        delete head;
        return item;
      }
    }
  }

  /// Return if the queue is empty
  bool Empty()
  {
    return (m_size == 0);
  }

  /// Return the currently-stored size of the queue
  size_t Size()
  {
    return m_size;
  }
};
