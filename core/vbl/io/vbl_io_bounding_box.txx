// This is vxl/vbl/io/vbl_io_bounding_box.txx

#include <vbl/vbl_bounding_box.h>
#include <vsl/vsl_binary_io.h>

//==========================================================================
//: Binary save self to stream.
template<class T,int DIM>
void vsl_b_write(vsl_b_ostream &os, const vbl_bounding_box<T, DIM> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.initialized_);
  for (int i = 0; i< DIM; i++)
  {
    vsl_b_write(os, p.min_[i]);
    vsl_b_write(os, p.max_[i]);
  }
}

//=========================================================================
//: Binary load self from stream.
template<class T, int DIM>
void vsl_b_read(vsl_b_istream &is, vbl_bounding_box<T, DIM> & p)
{
  short v;
  vsl_b_read(is, v);
  switch(v)
  {
  case 1:
    vsl_b_read(is, p.initialized_);
    for (int i = 0; i< DIM; i++)
    {
      vsl_b_read(is, p.min_[i]);
      vsl_b_read(is, p.max_[i]);
    }
    break;

  default:
    vcl_cerr << "vsl_b_read(is, vbl_bounding_box&): Unknown version number "<< v << vcl_endl;
    vcl_abort();
  }

}


//===========================================================================
//: Output a human readable summary to the stream
template<class T, int DIM>
void vsl_print_summary(vcl_ostream& os,const vbl_bounding_box<T, DIM> & p)
{
    os << vcl_endl;
    os << "initialized_ : " << p.initialized_ << vcl_endl << vcl_endl;
    for(int i=0;i<DIM;i++)
    {
      os << "min[" << i << "] = " << p.min_[i] << vcl_endl;
    }
    os << vcl_endl;
    for(int i=0;i<DIM;i++)
    {
      os << "max_[" << i << "] = " << p.max_[i] << vcl_endl;
    }
    os << vcl_endl;

}

#define VBL_IO_BOUNDING_BOX_INSTANTIATE(T,DIM) \
template void vsl_print_summary(vcl_ostream&,const vbl_bounding_box<T,DIM >&);\
template void vsl_b_read(vsl_b_istream &, vbl_bounding_box<T ,DIM > &); \
template void vsl_b_write(vsl_b_ostream &, const vbl_bounding_box<T ,DIM > &)

