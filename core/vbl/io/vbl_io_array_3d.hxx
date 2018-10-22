// This is core/vbl/io/vbl_io_array_3d.hxx
#ifndef vbl_io_array_3d_hxx_
#define vbl_io_array_3d_hxx_
//:
// \file
// \brief  binary IO functions for vbl_array_3d<T>
// \author K.Y.McGaul

#include <iostream>
#include "vbl_io_array_3d.h"
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_array_3d.h>

//====================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream & os, const vbl_array_3d<T> &p)
{
  constexpr short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  typedef typename vbl_array_3d<T>::size_type size_type;
  size_type row1_count = p.get_row1_count();
  size_type row2_count = p.get_row2_count();
  size_type row3_count = p.get_row3_count();
  vsl_b_write(os, row1_count);
  vsl_b_write(os, row2_count);
  vsl_b_write(os, row3_count);
  for (size_type i=0; i<row1_count; i++)
    for (size_type j=0; j<row2_count; j++)
      for (size_type k=0; k<row3_count; k++)
        vsl_b_write(os, p(i,j,k));
}

//=====================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vbl_array_3d<T> &p)
{
  if (!is) return;

  short ver;
  typedef typename vbl_array_3d<T>::size_type size_type;
  size_type row1_count, row2_count, row3_count;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, row1_count);
    vsl_b_read(is, row2_count);
    vsl_b_read(is, row3_count);
    p.resize(row1_count, row2_count, row3_count);
    for (size_type i=0; i<row1_count; i++)
      for (size_type j=0; j<row2_count; j++)
        for (size_type k=0; k<row3_count; k++)
          vsl_b_read(is, p(i,j,k));
    break;

   default:
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_array_3d<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//====================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(std::ostream & os,const vbl_array_3d<T> & p)
{
  os << "Row1 count: " << p.get_row1_count() << std::endl
     << "Row2 count: " << p.get_row2_count() << std::endl
     << "Row3 count: " << p.get_row3_count() << std::endl;
  typedef typename vbl_array_3d<T>::size_type size_type;
  for (size_type i =0; i<p.get_row1_count() && i<5; i++)
  {
    for (size_type j=0; j<p.get_row2_count() && j<5; j++)
    {
      for (size_type k=0; k<p.get_row3_count() && k<5; k++)
      {
        os << ' ';
        vsl_print_summary(os, p(i,j,k));
      }
      if (p.get_row3_count() > 5)
        os << " ...\n";
    }
    if (p.get_row2_count() > 5)
      os << " ...";
    os << std::endl << std::endl;
  }
  if (p.get_row1_count() > 5)
    os << " ...\n";
}

#define VBL_IO_ARRAY_3D_INSTANTIATE(T) \
template void vsl_print_summary(std::ostream &, const vbl_array_3d<T > &); \
template void vsl_b_read(vsl_b_istream &, vbl_array_3d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vbl_array_3d<T > &)

#endif // vbl_io_array_3d_hxx_
