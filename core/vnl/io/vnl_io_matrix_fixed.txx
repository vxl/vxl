// This is vxl/vnl/io/vnl_io_matrix_fixed.txx
#ifndef vnl_io_matrix_fixed_txx_
#define vnl_io_matrix_fixed_txx_

#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vsl/vsl_indent.h>


//=================================================================================
//: Binary save self to stream.
template<class T, int m, int n >
void vsl_b_write(vsl_b_ostream & os, const vnl_matrix_fixed<T,m,n> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  for(int i=0;i<m*n;i++)
  {
    vsl_b_write(os, p.data_block()[i]);
  }
}

//=================================================================================
//: Binary load self from stream.
template<class T, int m, int n>
void vsl_b_read(vsl_b_istream &is, vnl_matrix_fixed<T,m,n> & p)
{
  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    for(int i=0;i<m*n;i++)
    {
      vsl_b_read(is, p.data_block()[i]);
    }
    break;

  default:
    vcl_cerr << "ERROR: vsl_b_read(s, vnl_matrix_fixed&): Unknown version number "<< ver << vcl_endl;
    vcl_abort();
  }
}

//====================================================================================
//: Output a human readable summary to the stream
template<class T, int m, int n>
void vsl_print_summary(vcl_ostream & os,const vnl_matrix_fixed<T,m,n> & p)
{
  os<<"Size: "<<m<<" x "<<n<<vcl_endl;

  int rows_out=m;
  int cols_out=n;

  if ( m>5)
  {
    rows_out=5;
  }

  if ( n>5)
  {
    cols_out=5;
  }

  vsl_inc_indent(os);
  for (int i=0;i<rows_out;++i)
  {
    os<<vsl_indent()<<" (";

    for ( int j=0; j<cols_out; ++j)
      if (n > cols_out) os<<"...";

    os<<")"<<vcl_endl;
  }
  if (m>rows_out) os <<vsl_indent()<<" (...)" <<vcl_endl;
  vsl_dec_indent(os);
}


#define VNL_IO_MATRIX_FIXED_INSTANTIATE(T,m,n) \
template void vsl_print_summary(vcl_ostream &, const vnl_matrix_fixed<T,m,n> &); \
template void vsl_b_read(vsl_b_istream &, vnl_matrix_fixed<T,m,n> &); \
template void vsl_b_write(vsl_b_ostream &, const vnl_matrix_fixed<T,m,n> &)

#endif // vnl_io_matrix_fixed_txx_
