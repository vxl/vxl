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

//=======================================================================
// Method: version_no
//=======================================================================

template<class T>
short mbl_data_collector<T>::version_no() const
{
  return 1;
}

//=======================================================================
// Method: vsl_add_to_binary_loader
//=======================================================================

template<class T>
void vsl_add_to_binary_loader(const mbl_data_collector<T>& b)
{
  vsl_binary_loader<mbl_data_collector<T> >::instance().add(b);
}

//=======================================================================
// Method: is_a
//=======================================================================

template<class T>
vcl_string mbl_data_collector<T>::is_a() const
{
  return vcl_string("mbl_data_collector<T>");
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

template<class T>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector<T>* b)
{
  if (b)
  {
    vsl_b_write(bfs,b->is_a());
    b->b_write(bfs);
  }
  else
  vsl_b_write(bfs,vcl_string("VSL_NULL_PTR"));
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

template<class T>
void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector<T>& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

template<class T>
void vsl_b_read(vsl_b_istream& bfs, mbl_data_collector<T>& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

template<class T>
vcl_ostream& operator<<(vcl_ostream& os,const mbl_data_collector<T>& b)
{
  os << b.is_a() << ": ";
  vsl_inc_indent(os);
  b.print_summary(os);
  vsl_dec_indent(os);
  return os;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

template<class T>
vcl_ostream& operator<<(vcl_ostream& os,const mbl_data_collector<T>* b)
{
  if (b)
  return os << *b;
  else
  return os << "No mbl_data_collector<T> defined.";
}

//=======================================================================

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
/* template class mbl_data_wrapper<T >; */ \
template void vsl_add_to_binary_loader(const mbl_data_collector<T >& b); \
template void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector<T >& b); \
template void vsl_b_write(vsl_b_ostream& bfs, const mbl_data_collector<T >* b); \
template void vsl_b_read(vsl_b_istream& bfs, mbl_data_collector<T >& b); \
template vcl_ostream& operator<<(vcl_ostream& os,const mbl_data_collector<T >& b); \
template vcl_ostream& operator<<(vcl_ostream& os,const mbl_data_collector<T >* b); \
template unsigned long CopyAllData(mbl_data_collector<T > &dest,\
                                   mbl_data_wrapper<T > &src); \
template unsigned long MergeAllData(mbl_data_collector<T > &dest,\
                                    mbl_data_wrapper<T > &src1,\
                                    mbl_data_wrapper<T > &src2)

#endif // mbl_data_collector_txx_
