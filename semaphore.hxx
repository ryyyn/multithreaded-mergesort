#ifndef SEMAPHORE_HXX
#define SEMAPHORE_HXX

#include <semaphore.h>

namespace cs281
{

class semaphore
{
public:
  semaphore(std::string L_or_R);
  ~semaphore();
  void wait();
  void signal();

private:
  sem_t* sem_;
  std::string name_;
};

}

#include "semaphore.txx"

#endif /* SEMAPHORE_HXX */

