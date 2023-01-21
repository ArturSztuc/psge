#pragma once

#include <stdexcept>
#include <iostream>

template <typename T>
class LockFreeQueue {
private:
  struct Node 
  {
    T item;
    Node* next;
    Node(T item) : item(item), next(nullptr) {}
  };
  Node* m_head;
  Node* m_tail;
  size_t m_size;

public:
  LockFreeQueue() 
  {
    Node* sentinel = new Node(T());
    m_head = m_tail = sentinel;
    m_size = 0;
  };

  ~LockFreeQueue() 
  {
    while (m_head != nullptr) {
      Node* node = m_head;
      m_head = node->next;
      delete node;
    }
    m_size = 0;
  };

  void Push(T item) 
  {
    Node* node = new Node(item);
    while (true) {
      Node* tail = m_tail;
      Node* next = tail->next;
      if (tail != m_tail)
        continue;

      if (next == nullptr) {
        if (__sync_bool_compare_and_swap(&tail->next, next, node)) {
          __sync_bool_compare_and_swap(&m_tail, tail, node);
          m_size++;
          return;
        }
      } else {
        __sync_bool_compare_and_swap(&m_tail, tail, next);
      }
    }
  };

  T Pop() 
  {
    while (true) {
      Node* head = m_head;
      Node* tail = m_tail;
      Node* next = head->next;

      if (head != m_head)
        throw std::underflow_error("We got head != m_head, which is strange");

      if(m_size == 0)
        throw std::underflow_error("Queue is empty!");

      if (head == tail) {
        __sync_bool_compare_and_swap(&m_tail, tail, next);
        continue;
      }

      T item = next->item;
      if (__sync_bool_compare_and_swap(&m_head, head, next)) {
        m_size--;
        delete head;
        return item;
      }
    }
  }

  bool Empty()
  {
    return (m_size == 0);
  }

  size_t Size()
  {
    return m_size;
  }


};

