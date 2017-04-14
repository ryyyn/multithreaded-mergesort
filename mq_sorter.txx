#ifndef MQ_SORTER_TXX
#define MQ_SORTER_TXX

namespace cs281
{

template <typename T>
mq_sorter<T>::mq_sorter(std::string L_or_R, std::vector<T>& data, size_t startPos, size_t endPos) :
             mq_("/mq_sorter_"+std::to_string(getpid()) + "_" + L_or_R, 10u), data_(data), 
             startPos_(startPos), endPos_(endPos)
{
}

template <typename T>
mq_sorter<T>::~mq_sorter()
{}

template <typename T>
mq_iterator<T>  mq_sorter<T>::getBegin()
{
  return mq_.begin();
}

template <typename T> 
mq_iterator<T>  mq_sorter<T>::getEnd()
{
  return mq_.end();
}

template <typename T>
void mq_sorter<T>::business()
{

  if(endPos_ - startPos_ <= sort_floor)
  {
    // sort the array
    if(startPos_!=endPos_) // edge case checking
      insertion_sort(data_.begin() + startPos_, data_.begin() + endPos_);
    
    T jnk;
    mq_.send(jnk, 1u);
    
    std::for_each(data_.begin() + startPos_, data_.begin() + endPos_,
      [&](T msg)
      {
        mq_.send(msg, 1u);
      }
    );
    
    mq_.send(jnk, 0u);
  }
  
  // spawns two children and merges
  else
  {
    mq_sorter<T> sort1("L", data_, startPos_,       startPos_ + (endPos_ - startPos_)/2);
    mq_sorter<T> sort2("R", data_, startPos_ + (endPos_ - startPos_)/2,         endPos_);
    
    sort1.launch();
    sort2.launch();
    
    std::merge(sort1.getBegin(), sort1.getEnd(), sort2.getBegin(), sort2.getEnd(), data_.begin()+startPos_);
    
    T jnk;
    mq_.send(jnk, 1u);
    
    std::for_each(data_.begin() + startPos_, data_.begin() + endPos_,
      [&](T msg)
      {
        mq_.send(msg, 1u);
      }
    );
    
    mq_.send(jnk, 0u);

  }

}

}


#endif /* MQ_SORTER_TXX */
