// This is vxl/vnl/io/vnl_io_matrix.txx


#include <vnl/vnl_matrix.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vsl/vsl_indent.h>

//=================================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream & os, const vnl_matrix<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.rows());
  vsl_b_write(os, p.cols());
  const T * block= p.begin();   // A pointer to the start of the matrix data block
  const unsigned mn = p.size();    // The size of the matrix to be loaded
  for (unsigned i=0; i < mn; ++i)
  vsl_b_write(os, block[i]);

}

//=================================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vnl_matrix<T> & p)
{
  short v;
  unsigned m, n;
  vsl_b_read(is, v);
  T * block;  // A pointer to the start of the matrix data block
  unsigned mn; // The size of the matrix to be loaded
  switch(v)
  {
  case 1:
    vsl_b_read(is, m);
    vsl_b_read(is, n);
    p.resize(m, n);
    mn = m*n;
    block= p.begin();
    for (unsigned i=0; i<mn; ++i)
      vsl_b_read(is, block[i]);
    break;

  default:
    vcl_cerr << "vnl_matrix::b_read() Unknown version number "<< v << vcl_endl;
    vcl_abort();
  }

}

//====================================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream & os,const vnl_matrix<T> & p)
{

  os<<"Size: "<<p.rows()<<" x "<<p.cols()<<vcl_endl;

  int m = 5; int n = 5;


  if (m>p.rows()) m=p.rows();
  if (n>p.cols()) n=p.cols();

  vsl_inc_indent(os);
  for (int i=0;i<m;i++)
  {
     os<<vsl_indent()<<" (";

     for ( int j=0; j<n; j++)
        os<<p(i,j)<<" ";
      if (p.cols()>n) os<<"...";
        os<<")"<<vcl_endl;
  }
  if (p.rows()>m) os <<vsl_indent()<<" (..." <<vcl_endl;
  vsl_dec_indent(os);
}

#define VNL_IO_MATRIX_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vnl_matrix<T > &); \
template void vsl_b_read(vsl_b_istream &, vnl_matrix<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vnl_matrix<T > &); \
;
