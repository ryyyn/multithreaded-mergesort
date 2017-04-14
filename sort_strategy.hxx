#ifndef SORT_STRATEGY_HPP
#define SORT_STRATEGY_HPP

namespace cs281
{
  template< typename T >
  class Sort_Strategy
  {
  public:
    virtual ~Sort_Strategy (void) {}
    virtual void sort (std::vector< T > &data) = 0;

    static Sort_Strategy * get_strategy (const std::string type);
  };

  template< typename T >
  class Insertion_Sort :
    public virtual Sort_Strategy < T >
  {
  public:
    virtual void sort (std::vector< T > &data);
  };


  template< typename T >
  class GNU_Sort :
    public virtual Sort_Strategy < T >
  {
  public:
    virtual void sort (std::vector< T > &data);
  };


  template< typename T >
  class shm_sort :
    public virtual Sort_Strategy < T >
  {
  public:
    virtual void sort (std::vector< T > &data);
  };


  template< typename T >
  class mq_sort :
    public virtual Sort_Strategy < T >
  {
  public:
    virtual void sort (std::vector< T > &data);
  };




}

#include "sort_strategy.txx"

#endif
