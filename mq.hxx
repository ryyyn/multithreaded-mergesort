#ifndef CS281_MQ_HXX
#define CS281_MQ_HXX

#include <string>
#include <iterator>
#include <mqueue.h>

namespace cs281
{
// forward declaration.
template <typename T> class mq_iterator;

template <typename T> class mq
{
public:
  typedef T value_type;
  typedef mq_iterator<T> iterator;
  friend class mq_iterator<T>;//why is this needed?

  mq(const std::string &name, size_t max_messages = 10);

  ~mq(void);

  iterator begin(void);

  iterator end(void);

  // You may want some additional operations...
  void send(const T &item, unsigned priority);
  
  T receive(unsigned& prio);
  
private:
  std::string name_;
  mqd_t mq_;
};

template <typename T> class mq_iterator
{
public:
  typedef void difference_type;
  typedef T value_type;
  typedef T *pointer;
  typedef T &reference;
  typedef std::forward_iterator_tag iterator_category;

  // Initialize the iterator, and get the first value, if appropriate.
  mq_iterator(mq<T> &mq, bool end = false);

  ~mq_iterator(void);

  value_type operator*(void);

  // pre-increment operator (e.g., ++i).  Receives the next item from
  // the queue.
  mq_iterator<T> operator++(void);

  // post-increment operator (e.g., i++).
  mq_iterator<T> operator++(int);

  // Determines if two iterators are equal.  Note this will be mostly used
  // to compare against an *end* iterator.
  bool operator==(const mq_iterator<T> s);

  bool operator!=(const mq_iterator<T> s);

private:
  mq<T>& mq_;
  bool end_;
  T item_;
};
}

#include "mq.txx"

#endif
