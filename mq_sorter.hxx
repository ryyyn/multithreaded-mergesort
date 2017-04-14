#ifndef MQ_SORTER_HXX
#define MQ_SORTER_HXX

#include "process.hxx"
#include "mq.hxx"

#include <vector>

namespace cs281
{

template <typename T>
class mq_sorter : public process
{
public:
  mq_sorter(std::string name, std::vector<T>& data, size_t startPos, size_t endPos);
  ~mq_sorter();
  mq_iterator<T> getBegin();
  mq_iterator<T> getEnd();
  virtual void business();
  
private:
  mq<T> mq_;
  std::vector<T>& data_;
  size_t startPos_;
  size_t endPos_;
};

}

#include "mq_sorter.txx"

#endif /* MQ_SORTER_HXX */
