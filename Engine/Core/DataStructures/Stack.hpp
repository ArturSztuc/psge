/**
 * @file Stack.hpp
 * @brief This is a prototype, so no documentation yet. Might be removed anyway.
 */

#pragma once

#include <stdio.h>

template<class DataType>
class Node
{
public:
  DataType m_data;
  Node<DataType>* m_next;

  void InsertAfter(DataType _data)
  {
    Node<DataType>* newnode = new Node<DataType>;
    newnode->m_data = _data;
    newnode->m_next = m_next;
    m_next = newnode;
  };
};

template<class DataType>
class LinkedList
{
private:
  Node<DataType>* m_head;
  Node<DataType>* m_tail;
  size_t m_size;

public:
  LinkedList()
  {
    m_head = nullptr;
    m_tail = nullptr;
    m_size = 0;
  };

  ~LinkedList()
  {
    Node<DataType>* itr = m_head;
    Node<DataType>* next;

    while(itr != nullptr){
      next = itr->m_next;
      delete itr;
      itr = next;
      m_size--;
    }
  };

  void Push(DataType _data)
  {
    if(m_head == nullptr){
      m_head = m_tail = new Node<DataType>;
      m_head->m_data = _data;
    }
    else{
      m_tail->InsertAfter(_data);
      m_tail = m_tail->m_next;
    }
    m_size++;
  }
};
