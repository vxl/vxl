// This is vxl/vnl/io/vnl_io_vector_fixed.txx

#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
#include <vsl/vsl_indent.h>



//=================================================================================
//: Binary save self to stream.
template<class T, int n>
void vsl_b_write(vsl_b_ostream & os, const vnl_vector_fixed<T,n> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  for(int i=0;i<n;i++)
  {
    vsl_b_write(os, p.operator()(i));
  }
  
}

//=================================================================================
//: Binary load self from stream.
template<class T, int n>
void vsl_b_read(vsl_b_istream &is, vnl_vector_fixed<T,n> & p)
{
  
  short ver;
  T val;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    for (unsigned i=0; i<n; ++i)
    {
      vsl_b_read(is, val);
      p.put(i, val);
    }
    break;
    
  default:
    vcl_cerr << "vnl_vector_fixed::b_read() Unknown version number "<< ver << vcl_endl;
    abort();
  }
  
  
  
}

//====================================================================================
//: Output a human readable summary to the stream
template<class T, int n>
void vsl_print_summary(vcl_ostream & os,const vnl_vector_fixed<T,n> & p)
{
   os<<"Fixed vector size"<<n<<" (";

    int elems_out=n;

    if ( n>5)
        elems_out=5;

    for (int i=0;i<elems_out;i++)
    {
        os<< p.operator()(i)<<" ";
    }

    if (n > elems_out) os<<"...";
    os<<")"<<vcl_endl;

}

#define VNL_IO_VECTOR_FIXED_INSTANTIATE(T,n) \
template void vsl_print_summary(vcl_ostream &, const vnl_vector_fixed<T,n> &); \
template void vsl_b_read(vsl_b_istream &, vnl_vector_fixed<T,n> &); \
template void vsl_b_write(vsl_b_ostream &, const vnl_vector_fixed<T,n> &); \
;
