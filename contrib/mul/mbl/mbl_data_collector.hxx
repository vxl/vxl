// This is mul/mbl/mbl_data_collector.hxx
#ifndef mbl_data_collector_hxx_
#define mbl_data_collector_hxx_
//:
//  \file

#include <iostream>
#include <algorithm>
#include "mbl_data_collector.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================
// Dflt ctor
//=======================================================================

template<class T>
mbl_data_collector<T>::mbl_data_collector() = default;

//=======================================================================
// Destructor
//=======================================================================

template<class T>
mbl_data_collector<T>::~mbl_data_collector() = default;


//: Copy all the data from a mbl_data_wrapper<T> into a mbl_data_collector<T>
template<class T>
unsigned long mbl_data_collector_copy_all(mbl_data_collector<T> &dest, mbl_data_wrapper<T> &src)
{
  if (src.size() == 0) return 0;
  unsigned long i = 1;

  src.reset();
  dest.record(src.current());
  while (src.next())
  {
    dest.record(src.current());
    i++;
  }
  return i;
}

//: Merge all the data from the two mbl_data_wrapper-s into one mbl_data_collector<T>
template<class T>
unsigned long mbl_data_collector_merge_all(mbl_data_collector<T> &dest,
                                           mbl_data_wrapper<T > &src0,
                                           mbl_data_wrapper<T > &src1,
                                           std::vector<unsigned> *order /*=0*/)
{
  unsigned long n0 = src0.size();
  unsigned long n1 = src1.size();

// Deal with special cases

  if (n0 == 0L)
  {
    if (order)
    {
      order->resize(n1);
      std::fill(order->begin(), order->end(), 1);
    }
    return mbl_data_collector_copy_all(dest, src1);
  }
  if (n1 == 0L)
  {
    if (order)
    {
      order->resize(n0);
      std::fill(order->begin(), order->end(), 0);
    }
    return mbl_data_collector_copy_all(dest, src0);
  }

  if (order)
  {
    order->clear();
    order->reserve(n0+n1);
  }

  long remainder=0;
  src0.reset();
  src1.reset();
  for (unsigned long i=0; i < n0+n1; i++)
  {
    if (remainder <= 0L)
    {
      // Take from src0;
      dest.record(src0.current());
      remainder += n1;
      src0.next();
      if (order) order->push_back(0);
    }
    else
    {
      // Take from src1;
      dest.record(src1.current());
      remainder -= n0;
      src1.next();
      if (order) order->push_back(1);
    }
  }

  assert ((!src0.next()) && (!src1.next())); // check that we have reached the end.
  return n0 + n1;
}

#define MBL_DATA_COLLECTOR_INSTANTIATE(T) \
template class mbl_data_collector<T >; \
template unsigned long mbl_data_collector_copy_all(mbl_data_collector<T > &dest, \
                                    mbl_data_wrapper<T > &src);\
template unsigned long mbl_data_collector_merge_all(mbl_data_collector<T > &dest,\
                                    mbl_data_wrapper<T > &src1,\
                                    mbl_data_wrapper<T > &src2,\
                                    std::vector<unsigned > *order)

#endif // mbl_data_collector_hxx_
