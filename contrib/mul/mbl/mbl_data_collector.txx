#ifndef mbl_data_collector_txx_
#define mbl_data_collector_txx_

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <mbl/mbl_data_collector.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================
// Dflt ctor
//=======================================================================

template<class T>
mbl_data_collector<T>::mbl_data_collector()
{
}

//=======================================================================
// Destructor
//=======================================================================

template<class T>
mbl_data_collector<T>::~mbl_data_collector()
{
}


//: Copy all the data from a mbl_data_wrapper<T> into a mbl_data_collector<T>
template<class T>
unsigned long CopyAllData(mbl_data_collector<T> &dest, mbl_data_wrapper<T> &src)
{
  if (src.size() == 0) return 0;
  unsigned long i = 1;

  src.reset();
  dest.record(src.current());
  while(src.next())
  {
    dest.record(src.current());
    i++;
  }
  return i;
}

//: Merge all the data from two mbl_data_wrapper-s into one mbl_data_collector<T>
template<class T>
unsigned long MergeAllData(mbl_data_collector<T> &dest,
                              mbl_data_wrapper<T > &src1,
                mbl_data_wrapper<T > &src2)
{
  long n1 = src1.size();
  long n2 = src2.size();
// Deal with special cases
  if (n1 == 0) return CopyAllData(dest, src2);
  if (n2 == 0) return CopyAllData(dest, src1);

  long remainder=0;
  src1.reset();
  src2.reset();
  for(long i=0; i < n1+n2; i++)
  {
    if (remainder <= 0)
    {
      // Take from src1;
      dest.record(src1.current());
      remainder += n2;
      src1.next();
    }
    else
    {
      // Take from src2;
      dest.record(src2.current());
      remainder -= n1;
      src2.next();
    }
  }

  assert ((!src1.next()) && (!src2.next())); // check that we have reached the end.
  return n1 + n2;
}

#define MBL_DATA_COLLECTOR_INSTANTIATE(T) \
template class mbl_data_collector< T >; \
template unsigned long CopyAllData(mbl_data_collector<T > &dest, \
                                    mbl_data_wrapper<T > &src);\
template unsigned long MergeAllData(mbl_data_collector<T > &dest,\
                                    mbl_data_wrapper<T > &src1,\
                                    mbl_data_wrapper<T > &src2)

#endif // mbl_data_collector_txx_
