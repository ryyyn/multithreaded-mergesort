#ifndef SHM_HXX
#define SHM_HXX

#include <sys/mman.h>
#include <iterator>

namespace cs281
{

template <typename T>
class shm
{
public: 
  shm(std::string name, unsigned long numElems);
  ~shm();

  T& operator[] (const int& index);

  T* begin(void);
  T* end(void);

private:
  int size_;
  T* shm_segment_;
  std::string name_;
};

}

#include "shm.txx"

#endif /* SHM_HXX */

