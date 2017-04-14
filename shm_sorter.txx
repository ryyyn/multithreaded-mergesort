#ifndef SHM_SORTER_TXX
#define SHM_SORTER_TXX

namespace cs281
{

template<typename T>
shm_sorter<T>::shm_sorter(shm<T>& shm_segment, semaphore& sem, size_t startPos, size_t endPos) : shm_(shm_segment), sem_(sem), startPos_(startPos), endPos_(endPos)
{}

template<typename T>
shm_sorter<T>::~shm_sorter()
{}

template <typename T>
T* shm_sorter<T>::getBegin()
{
  return shm_.begin() + startPos_;
}

template <typename T>
T* shm_sorter<T>::getEnd()
{
  return shm_.begin() + endPos_;
}

template<typename T>
void shm_sorter<T>::business()
{
  if(endPos_ - startPos_ <= sort_floor)
  {
    // sort the array in place
    if(startPos_!=endPos_) // edge case checking
      insertion_sort(shm_.begin() + startPos_, shm_.begin() + endPos_);
    
    sem_.signal();
  }
  
  else
  {
    // create two semaphores
    semaphore sem1("L");
    semaphore sem2("R");

    // create the sorters
    shm_sorter<T> sort1(shm_, sem1, startPos_,       startPos_ + (endPos_ - startPos_)/2);
    shm_sorter<T> sort2(shm_, sem2, startPos_ + (endPos_ - startPos_)/2,         endPos_);
    
    sort1.launch();
    sort2.launch();
    
    // wait while the children sort
    sem1.wait();
    sem2.wait();
    
    std::inplace_merge(sort1.getBegin(), sort2.getBegin(), sort2.getEnd());

    // wait on the child processes
    sort1.wait();
    sort2.wait();

    if(sort1.exitStatus() != 0)
      throw std::system_error(sort1.exitStatus(), std::system_category(), "Error in child process.");

    if(sort2.exitStatus() != 0)
      throw std::system_error(sort2.exitStatus(), std::system_category(), "Error in child process.");
    
    sem_.signal();
  }

}

}


#endif /* SHM_SORTER_HXX */

