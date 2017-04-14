#ifndef UTILITY_TXX
#define UTILITY_TXX

#include <iterator>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

template <typename T>
File<T>::File (const std::string &file)
  : file_ (file),
    temporary_ (false)
{
  if (file_ == "")
    {
      temporary_ = true; //added by ryan
      char tmp_file[] = "/tmp/mergesort.XXXXXX";
      int fd = mkstemp (tmp_file);

      if (fd < 0)
	{
	  std::stringstream err;
	  err << "Error creating temporary file: " << strerror (errno) << std::endl;
	  throw std::runtime_error (err.str ());
	}

      char path[PATH_MAX];

#ifdef __MACOSX__ // grumble, grumble, gotta work some black magic on Linux.

      if (fcntl (fd, F_GETPATH, path) == -1)
	{
	  std::stringstream err;
	  err << "Error locating temporary file: " << strerror (errno) << std::endl;
	  throw std::runtime_error (err.str ());
	}

#else

	std::stringstream rl;
	rl << "/proc/self/fd/" << fd;

	if (readlink (rl.str ().c_str (), path, PATH_MAX) <= 0)
	  {
	    std::stringstream err;
	    err << "Error locating temporary file: " << strerror (errno) << std::endl;
	    throw std::runtime_error (err.str ());
	  }

#endif
      this->file_ = const_cast <const char *> (path);

      close (fd);
    }
}

template <typename T>
File<T>::~File ()
{
  if (temporary_)
    {
      unlink (this->file_.c_str ());
    }
}

template <typename T>
const std::string &
File<T>::name (void) const
{
  return this->file_;
}

template <typename T>
void
File<T>::read (std::vector<T> &data)
{
  std::ifstream file (this->file_);

  std::copy (std::istream_iterator< T > (file),
             std::istream_iterator< T > (),
             std::back_inserter (data));
}

template <typename T>
void
File<T>::write (typename value_type::const_iterator begin,
                typename value_type::const_iterator end)
{
  std::ofstream file (this->file_, std::ios_base::trunc);
  std::copy (begin,
             end,
             std::ostream_iterator< T > (file, "\n"));
}

template <typename T>
void
File<T>::write (const std::vector<T> &data)
{
  this->write (data.begin (), data.end ());
}

template <typename T>
void
insertion_sort (T begin, T end)
{
  for (T i = begin + 1; i != end; ++i)
    {
      T j = i;

      while ( (j != begin) &&
              (*(j - 1) > *j))
        {
          std::swap (*j, *(j - 1));
          --j;
        }
    }
}

template <typename T>
void
insertion_sort (std::vector <T> &data)
{
  insertion_sort (data.begin (), data.end ());
}


#endif
