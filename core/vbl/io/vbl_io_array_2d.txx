// This is core/vbl/io/vbl_io_array_2d.txx
#ifndef vbl_io_array_2d_txx_
#define vbl_io_array_2d_txx_
//:
// \file
// \brief  binary IO functions for vbl_array_2d<T>
// \author K.Y.McGaul

#include "vbl_io_array_2d.h"
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_array_2d.h>

//=======================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream & os, const vbl_array_2d<T> &p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);

  int array_rows = p.rows();
  int array_cols = p.cols();
  vsl_b_write(os, array_rows);
  vsl_b_write(os, array_cols);
  for (int i=0; i<array_rows; i++)
  {
    for (int j=0; j<array_cols; j++)
      vsl_b_write(os, p(i,j));
  }
}

//=======================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vbl_array_2d<T> &p)
{
  if (!is) return;

  short ver;
  int array_rows, array_cols;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
    vsl_b_read(is, array_rows);
    vsl_b_read(is, array_cols);
    p.resize(array_rows, array_cols);
    for (int i=0; i<array_rows; i++)
    {
      for (int j=0; j<array_cols; j++)
        vsl_b_read(is, p(i,j));
    }
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_array_2d<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//=======================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream & os,const vbl_array_2d<T> & p)
{
  os << "Rows: " << p.rows() << vcl_endl
     << "Columns: " << p.cols() << vcl_endl;
  for (int i =0; i<p.rows() && i<5; i++)
  {
    for (int j=0; j<p.cols() && j<5; j++)
    {
      os << ' ';
      vsl_print_summary(os, p(i,j));
    }
    if (p.cols() > 5)
      os << "...";
    os << vcl_endl;
  }
  if (p.rows() > 5)
    os << " ...\n";
}

#define VBL_IO_ARRAY_2D_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vbl_array_2d<T > &); \
template void vsl_b_read(vsl_b_istream &, vbl_array_2d<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vbl_array_2d<T > &)

#endif // vbl_io_array_2d_txx_
