#ifndef SHM_SORTER_HXX
#define SHM_SORTER_HXX

#include "process.hxx"
#include "shm.hxx"
#include "semaphore.hxx"

namespace cs281
{

template<typename T>
class shm_sorter : public process
{
  
public:
  shm_sorter(shm<T>& shm_segment, semaphore& sem, size_t startPos, size_t endPos);
  ~shm_sorter();
  virtual void business();
  T* getBegin();
  T* getEnd();

//owns a shared memory segment and a semaphore
private:
  shm<T>& shm_;
  semaphore& sem_;
  size_t startPos_;
  size_t endPos_;

};

}

#include "shm_sorter.txx"

#endif /* SHM_SORTER_HXX */

