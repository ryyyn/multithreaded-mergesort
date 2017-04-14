#ifndef CS281_MQ_TXX
#define CS281_MQ_TXX

#include <mqueue.h>
#include <system_error>

namespace cs281
{

template <typename T> mq<T>::mq(const std::string &name, size_t max_messages) : name_(name)
{
  mq_unlink(name_.c_str());
  
  struct mq_attr my_mq_attr;
  // set up message queue traits
  my_mq_attr.mq_flags = 0;
  my_mq_attr.mq_maxmsg = max_messages;
  my_mq_attr.mq_msgsize = sizeof(T);
  my_mq_attr.mq_curmsgs = 0;
  mq_ = mq_open (name_.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, &my_mq_attr);
  
  if (mq_ == -1)
  {
    throw std::system_error(errno, std::system_category(), "Creation of message queue failed.");
  }
}

template <typename T> mq<T>::~mq(void)
{
  mq_close(mq_);
  mq_unlink(name_.c_str());
}

template <typename T> 
typename mq<T>::iterator mq<T>::begin(void)
{
  return iterator(*this, false);
}

template <typename T> 
typename mq<T>::iterator mq<T>::end(void)
{
  return iterator(*this, true);
}

template <typename T>
void mq<T>::send(const T &item, unsigned priority)
{
  int sent = mq_send(mq_, reinterpret_cast<const char*> (&item), sizeof(T), priority);
  
  if(sent == -1)
    throw std::system_error(errno, std::system_category(), "Error during message sending");

}

template <typename T> 
T mq<T>::receive(unsigned& prio)
{
  T received;
  mq_receive(mq_, reinterpret_cast<char*>(&received), sizeof(T), &prio);
  return received;
}


//iterator functions
template <typename T>
mq_iterator<T>::mq_iterator(mq<T> &mq, bool end /*=false*/) : mq_(mq), end_(end)
{
  ++(*this);
}

template <typename T> 
mq_iterator<T>::~mq_iterator(void)
{
}

template <typename T> 
typename mq_iterator<T>::value_type mq_iterator<T>::operator*(void)
{
  return item_;
}

template <typename T> 
mq_iterator<T> mq_iterator<T>::operator++(void)
{
  unsigned prio;
  item_ = mq_.receive(prio);
  
  if(prio == 0)
    end_ = true;
  
  return *this;
}

template <typename T> 
mq_iterator<T> mq_iterator<T>::operator++(int)
{
  mq_iterator tmp(*this);
  operator++();
  return tmp;
}

template <typename T> 
bool mq_iterator<T>::operator==(const mq_iterator<T> s)
{
  return end_ == s.end_;
}

template <typename T> 
bool mq_iterator<T>::operator!=(const mq_iterator<T> s)
{
  return !(*this == s);
}

}

#endif
