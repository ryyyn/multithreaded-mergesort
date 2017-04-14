#ifndef SHM_TXX
#define SHM_TXX

namespace cs281
{

template <typename T>
shm<T>::shm(std::string name, unsigned long numElems) : name_(name), size_(numElems)
{
  int shm_fd = shm_open (name_.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  
  if (shm_fd == -1)
    throw std::system_error(errno, std::system_category(), "Error creaing shared memory segment");
  
  if (ftruncate(shm_fd, sizeof (T) * size_) != 0)
    throw std::system_error(errno, std::system_category(), "Error sizing memory segment");
  
  shm_segment_ = reinterpret_cast<T*> (mmap(0, sizeof(T) * size_, (PROT_READ | PROT_WRITE),  MAP_SHARED, shm_fd, 0));
  
  if (shm_segment_ == MAP_FAILED)
    throw std::system_error(errno, std::system_category(), "Error mapping memory"); 
}

template <typename T>
shm<T>::~shm()
{
  munmap(shm_segment_, sizeof(T) * size_);
  shm_unlink(name_.c_str());
}

template <typename T>
T& shm<T>::operator[] (const int& index)
{
  return shm_segment_[index];
}

template <typename T> 
T* shm<T>::begin(void)
{
  return shm_segment_;
}

template <typename T> 
T* shm<T>::end(void)
{
  return shm_segment_ + size_;
}

}

#endif /* SHM_TXX */

