#ifndef PROCESS_HXX
#define PROCESS_HXX

namespace cs281
{

class process
{
public:
  process();
  ~process();
  void launch();
  void wait();
  bool exited();
  virtual void business() = 0;
  int exitStatus();
  
private:
  pid_t pid_;
  int exit_status_;
};

}

#include "process.txx"

#endif /* PROCESS_HXX */
