// This is vxl/vnl/io/vnl_io_diag_matrix.txx

#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_diag_matrix.h>
#include <vnl/io/vnl_io_vector.h>



//=================================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream & os, const vnl_diag_matrix<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.diagonal());
}

//=================================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vnl_diag_matrix<T> & p)
{

  short ver;
  vnl_vector<T> v;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    vsl_b_read(is, v);
    p.set(v);
    break;

  default:
    vcl_cerr << "ERROR: vsl_b_read(s, vnl_diag_matrix&): Unknown version number "<< ver << vcl_endl;
    abort();
  }

}

//====================================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream & os,const vnl_diag_matrix<T> & p)
{
  os<<"Diagonal: ";
  vsl_print_summary(os, p.diagonal());
}

#define VNL_IO_DIAG_MATRIX_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vnl_diag_matrix<T > &); \
template void vsl_b_read(vsl_b_istream &, vnl_diag_matrix<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vnl_diag_matrix<T > &); \
;
