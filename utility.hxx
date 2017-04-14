#ifndef FILE_HXX_
#define FILE_HXX_

extern int sort_floor;

template < typename T >
class File
{
public:
  typedef std::vector<T> value_type;

  /// If the file name is the empty string,
  /// generates a temporary file name, which is
  /// automatically removed once
  /// 1) It's not open anywhere
  /// 2) The destructor for this class executes.
  File (const std::string &file);

  ~File (void);

  const std::string & name (void) const;

  void read (value_type &data);

  void write (const std::vector<T> &data);

  void write (typename value_type::const_iterator begin,
              typename value_type::const_iterator end);

private:
  T convert (const std::string &item);

  // Additional data members that you may require
  std::string file_;

  bool temporary_;
};

template < typename T >
void insertion_sort (std::vector <T> &data);

template < typename T >
void insertion_sort (T begin, T end);

#include "utility.txx"

#endif
