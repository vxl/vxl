// This is vxl/vnl/io/vnl_io_vector.txx

#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vsl/vsl_indent.h>


//=================================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream & os, const vnl_vector<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.size());
  for (unsigned i=0; i < p.size(); ++i)
    vsl_b_write(os, p[i]);

}

//=================================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vnl_vector<T> & p)
{

  short ver;
  unsigned n;
  T val;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
    vsl_b_read(is, n);
    p.resize(n);
    for (unsigned i=0; i<n; ++i)
  {
    vsl_b_read(is, val);
    p.put(i, val);
  }
    break;

  default:
    vcl_cerr << "vnl_vector::b_read() Unknown version number "<< ver << vcl_endl;
    abort();
  }

}

//====================================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream & os,const vnl_vector<T> & p)
{
	os<<"Len: "<<p.size()<<" (";
    for ( int i =0; i < p.size() && i < 5; i++ )
		  os << p.operator()(i) <<" " ;
    if (p.size() > 5) os << " ...";
	os << ")" << vcl_endl;
}

#define VNL_IO_VECTOR_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vnl_vector<T> &); \
template void vsl_b_read(vsl_b_istream &, vnl_vector<T> &); \
template void vsl_b_write(vsl_b_ostream &, const vnl_vector<T> &); \
;
