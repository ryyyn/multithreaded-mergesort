#ifndef SEMAPHORE_TXX
#define SEMAPHORE_TXX

namespace cs281
{

semaphore::semaphore(std::string L_or_R) : name_("/" + std::to_string(getpid()) + L_or_R)
{
  sem_ = sem_open (name_.c_str(), O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH, 0);
  if(sem_ == SEM_FAILED)
    throw std::system_error(errno, std::system_category(), "Error creaing semaphore");
}

semaphore::~semaphore()
{
  sem_close(sem_);
  sem_unlink(name_.c_str());
}

void semaphore::wait()
{
  int err = sem_wait(sem_);
  if(err == -1)
    throw std::system_error(errno, std::system_category(), "sem_wait error");
}

void semaphore::signal()
{
  int err = sem_post(sem_);
  if(err == -1)
    throw std::system_error(errno, std::system_category(), "sem_post error");

}


}

#endif /* SEMAPHORE_TXX */

