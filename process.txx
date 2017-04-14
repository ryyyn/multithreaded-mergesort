#ifndef PROCESS_TXX
#define PROCESS_TXX

namespace cs281
{

process::process() : pid_(-1), exit_status_ (-1)
{}

process::~process()
{
  if(!exited())
    wait();
}

void process::launch()
{
  if(pid_ != -1)
    return;

  pid_ = fork();
  if(pid_ == -1)
    throw std::system_error(errno, std::system_category(), "Forking to sort failed");

  // if in parent
  if(pid_ !=  0)
    return;

  try
  {
    business();
    exit(0);
  }
  catch (std::exception &ex)
  {
    std::cerr << "Caught exception: " << ex.what () << std::endl;
    exit(-1);
  }
}

void process::wait()
{
  int status;
  pid_t ended = waitpid(pid_, &status, 0);

  pid_ = -1;

  std::string err;
  if(ended ==-1)
  {
    err = "waiting on child process " + std::to_string(pid_) + " failed";
    throw std::system_error(WEXITSTATUS(status), std::system_category(), err);
  }

  exit_status_ = WEXITSTATUS(status);
}

// returns true if process has exited, false otherwise
// will return true if process hasn't yet been launched
bool process::exited()
{
  return pid_ == -1;
}

// is default initialized to -1
int process::exitStatus()
{
  return exit_status_;
}


  
}
#endif /* PROCESS_TXX */
