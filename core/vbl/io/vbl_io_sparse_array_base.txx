// This is vxl/vbl/io/vbl_io_sparse_array_base.txx

#include <vbl/vbl_sparse_array_base.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_pair_io.h>
//============================================================================
//: Binary save self to stream.
template<class T, class Index>
void vsl_b_write(vsl_b_ostream &os, const vbl_sparse_array_base<T, Index> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  
  vsl_b_write(os, p.count_nonempty());
  for(vbl_sparse_array_base<T, Index>::const_iterator s = p.begin(); s != p.end(); ++s){
    vsl_b_write(os, *s);
  }
  
}
//===========================================================================
//: Binary load self from stream.
template<class T, class Index>
void vsl_b_read(vsl_b_istream &is, vbl_sparse_array_base<T, Index> & p)
{
  p.clear();
  short v;
  vsl_b_read(is, v);

  vcl_pair<Index, T> value;
  unsigned size;
  switch(v)
  {
  case 1:
    vsl_b_read(is, size);
    for(unsigned i=0; i<size; i++){
      vsl_b_read(is, value);      
      p(value.first)=value.second;
    }
    
    break;
    
  default:
    vcl_cerr << "vsl_b_read() Unknown version number "<< v << vcl_endl;
    abort();
  }
  
}


//==========================================================================
//: Output a human readable summary to the stream
template<class T, class Index>
void vsl_print_summary(vcl_ostream& os,const vbl_sparse_array_base<T, Index> & p)
{
  os<<"nonempty elements: "<< p.count_nonempty() << vcl_endl;
  int k=0;
  
  for(vbl_sparse_array_base<T, Index>::const_iterator s = p.begin(); 
      s != p.end() && k<5; ++s)
  {
    k++;
    os << " ";
    vsl_print_summary(os, (*s).first);
    os << ": ";
    vsl_print_summary(os, (*s).second);
    os << vcl_endl;
  }
  if (p.count_nonempty() > 5) 
    os << " ..." << vcl_endl;
  
}

#define VBL_IO_SPARSE_ARRAY_BASE_INSTANTIATE(T, I) \
  template void vsl_print_summary(vcl_ostream &, const vbl_sparse_array_base<T , I > &); \
  template void vsl_b_read(vsl_b_istream &, vbl_sparse_array_base<T , I > &); \
  template void vsl_b_write(vsl_b_ostream &, const vbl_sparse_array_base<T , I > &); \
;
