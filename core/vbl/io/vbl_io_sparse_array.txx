// This is vxl/vbl/io/vbl_io_sparse_array.txx

#include <vbl/vbl_sparse_array.h>
#include <vsl/vsl_binary_io.h>

//============================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream &os, const vbl_sparse_array<T> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  
  vsl_b_write(os, p.count_nonempty());
  for(vbl_sparse_array<T>::const_iterator s = p.begin(); s != p.end(); ++s){
    vsl_b_write(os, (*s).first);
    vsl_b_write(os, (*s).second);
  }
  
}
//===========================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vbl_sparse_array<T> & p)
{
  short v;
  vsl_b_read(is, v);
  switch(v)
  {
  case 1:
    
    unsigned size;
    unsigned key;
    T data;
    vsl_b_read(is, size);
    for(unsigned i=0; i<size; i++){
      vsl_b_read(is, key);
      vsl_b_read(is, data);
      p[key]=data;
    }
    
    break;
    
  default:
    vcl_cerr << "vsl_b_read() Unknown version number "<< v << vcl_endl;
    abort();
  }
  
}


//==========================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream& os,const vbl_sparse_array<T> & p)
{
  os<<"( nonempty elements ="<< p.count_nonempty() << ")" << vcl_endl;
  int k=0;
  
  for(vbl_sparse_array<T>::const_iterator s = p.begin(); 
      s != p.end() && k<5; ++s)
  {
    k++;
    os << " key " << (*s).first << " data "; 
    vsl_print_summary(os, (*s).second);
    os << vcl_endl;
  }
  if (p.count_nonempty() > 5) 
    os << " ..." << vcl_endl;
  
}

#define VBL_IO_SPARSE_ARRAY_INSTANTIATE(T) \
  template void vsl_print_summary(vcl_ostream &, const vbl_sparse_array<T> &); \
  template void vsl_b_read(vsl_b_istream &, vbl_sparse_array<T> &); \
  template void vsl_b_write(vsl_b_ostream &, const vbl_sparse_array<T> &); \
;
