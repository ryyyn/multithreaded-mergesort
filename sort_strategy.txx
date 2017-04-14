#ifndef SORT_STRATEGY_TXX
#define SORT_STRATEGY_TXX

#include "utility.hxx"
#include <exception>
#include <system_error>

#include <sys/wait.h>

#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */


#include "mq_sorter.hxx"
#include "shm_sorter.hxx"
#include "semaphore.hxx"
#include "shm.hxx"

#include <sys/mman.h>

#include <algorithm>


namespace cs281
{
  template< typename T >
  Sort_Strategy<T> *
  Sort_Strategy<T>::get_strategy (const std::string type)
  {
    if (type == "insertion")
      return new Insertion_Sort<T> ();
    if (type == "gnusort1")
      return new GNU_Sort<T> ();
    if (type == "shm")
      return new shm_sort<T> ();
    if (type == "mq")
      return new mq_sort<T> ();


    throw std::runtime_error ("Invalid sort strategy\n");
  }

  template< typename T >
  void
  Insertion_Sort<T>::sort (std::vector< T > &data)
  {
    insertion_sort (data);
  }


  template< typename T >
  void
  GNU_Sort<T>::sort (std::vector< T > &data)
  {

    //create the temporary files
    File<int> temp_1 ("");
    File<int> temp_2 ("");

    int mid = data.size()/2;

    //write first half
    temp_1.write(data.begin(), data.begin()+mid);

   //write second half
    temp_2.write(data.begin() + mid, data.end());

    pid_t myPID_1 = fork();


    /* Sorter 1 */
    if (myPID_1 == 0)
    {
        //sort
        execlp("sort", "sort", "--numeric-sort", "-o", temp_1.name().c_str(),
            temp_1.name().c_str(), NULL);

        // will only be reached if sort fails

        exit(errno);
    }
    /* End Sorter 1 */

    if (myPID_1<0)
        throw std::system_error(errno, std::system_category(), "Forking to sort failed");

    /* Sorter 2 */
    pid_t myPID_2 = fork();

    if (myPID_2 == 0)
    {
        //sort
        execlp("sort", "sort", "--numeric-sort", "-o", temp_2.name().c_str(),
            temp_2.name().c_str(), NULL);

        //only called if exec errors and returns
        exit(errno);
    }
    /* End Sorter 2 */


    if (myPID_2<0)
    // !! First child isn't reaped
    {
        // reap the first child like this(?)
        waitpid(myPID_1, nullptr, 0);
        throw std::system_error(errno, std::system_category(), "Forking to sort failed");
    }

    //wait for both sorters to complete
    int stat1, stat2;

    pid_t sortEnded_1 = waitpid(myPID_1, &stat1, 0);
    pid_t sortEnded_2 = waitpid(myPID_2, &stat2, 0);


    std::string err;

    //waitpid error
    if(sortEnded_1==-1)
    {
        err = "waiting on child process " + std::to_string(myPID_1) + " failed";
        throw std::system_error(WEXITSTATUS(stat1), std::system_category(), err);
    }

    if(sortEnded_2==-1)
    {
        err = "waiting on child process " + std::to_string(myPID_2) + " failed";
        throw std::system_error(WEXITSTATUS(stat2), std::system_category(), err);
    }

    //exited normally or not
    if(!WIFEXITED(stat1))
    {
        err = "child process " + std::to_string(myPID_1) + " did not exit normally";
        throw std::system_error(WEXITSTATUS(stat1), std::system_category(), err);
    }

    if(!WIFEXITED(stat2))
    {
        err = "child process " + std::to_string(myPID_2) + " did not exit normally";
        throw std::system_error(WEXITSTATUS(stat2), std::system_category(), err);
    }

    //if exec failed and child exited with errno

    if(WEXITSTATUS(stat1))
    {
        err = "exec failed in child process " + std::to_string(myPID_1);
        throw std::system_error(WEXITSTATUS(stat1), std::system_category(), err);
    }

    if(WEXITSTATUS(stat2))
    {
        err = "exec failed in child process " + std::to_string(myPID_2);
        throw std::system_error(WEXITSTATUS(stat2), std::system_category(), err);
    }


    myPID_1 = fork();

    /* Merge */
    if (myPID_1 == 0)
    {
        execlp("sort", "sort", "--merge", "--numeric-sort", "-o", temp_1.name().c_str(),
            temp_1.name().c_str(), temp_2.name().c_str(), NULL);

        //only called if exec errors and returns
        exit(errno);
    }
    /* End Merge */

    else if (myPID_1<0)
        throw std::system_error(errno, std::system_category(), "Forking to merge failed");

    //Wait for merge to complete
    pid_t mergeEnded = waitpid(myPID_1, &stat1, 0);


    //waitpid error
    if(mergeEnded==-1)
    {
        err = "waiting on child process " + std::to_string(myPID_1) + " failed";
        throw std::system_error(WEXITSTATUS(stat1), std::system_category(), err);
    }

    //exited normally or not
    if(!WIFEXITED(stat1))
    {
        err = "child process " + std::to_string(myPID_1) + " did not exit normally";
        throw std::system_error(WEXITSTATUS(stat1), std::system_category(), err);
    }

    //if exec failed and child exited with errno
    if(WEXITSTATUS(stat1))
    {
        err = "exec failed in child process " + std::to_string(myPID_1);
        throw std::system_error(WEXITSTATUS(stat1), std::system_category(), err);
    }

    //return the sorted data to our vector
    data.clear();
    temp_1.read(data);
  }



  template< typename T >
  void
  mq_sort<T>::sort (std::vector< T > &data)
  {
    // create the sorters
    mq_sorter<T> sort1("L", data, 0u,            data.size()/2);
    mq_sorter<T> sort2("R", data, data.size()/2, data.size()  );
    
    sort1.launch();
    sort2.launch();

    std::merge(sort1.getBegin(), sort1.getEnd(), sort2.getBegin(), sort2.getEnd(), data.begin());

    sort1.wait();
    sort2.wait();

    if(sort1.exitStatus() != 0) 
      throw std::system_error(sort1.exitStatus(), std::system_category(), "Error in child process.");

    if(sort2.exitStatus() != 0)
      throw std::system_error(sort2.exitStatus(), std::system_category(), "Error in child process.");
   
  }

  template< typename T >
  void
  shm_sort<T>::sort (std::vector< T > &data)
  {
    //construct shared memory segment
    shm<T> shm_segment("/data_shm_", data.size());
    
    // write the data into the shm segment
    int i=0;
    for(T& msg : data)
      shm_segment[i++] = msg;

    // create two semaphores
    semaphore sem1("L");
    semaphore sem2("R");
    
    // create the sorters
    shm_sorter<T> sort1(shm_segment, sem1, 0u,            data.size()/2);
    shm_sorter<T> sort2(shm_segment, sem2, data.size()/2, data.size()  );

    sort1.launch();
    sort2.launch();
    
    // wait while the children sort
    sem1.wait();
    sem2.wait();

    std::merge(sort1.getBegin(), sort1.getEnd(), sort2.getBegin(), sort2.getEnd(), data.begin());

    // wait on the child processes
    sort1.wait();
    sort2.wait();

    if(sort1.exitStatus() != 0)
      throw std::system_error(sort1.exitStatus(), std::system_category(), "Error in child process.");

    if(sort2.exitStatus() != 0)
      throw std::system_error(sort2.exitStatus(), std::system_category(), "Error in child process.");


  }/* end shm sort */

}/* end namespace 281 */

#endif
