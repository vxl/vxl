// This is core/vbl/io/vbl_io_bounding_box.txx
#ifndef vbl_io_bounding_box_txx_
#define vbl_io_bounding_box_txx_
//:
// \file

#include "vbl_io_bounding_box.h"
#include <vsl/vsl_binary_io.h>

//==========================================================================
//: Binary save self to stream.
template<class T, class DIM_>
void vsl_b_write(vsl_b_ostream &os, const vbl_bounding_box_base<T, DIM_> & p)
{
  const short io_version_no = 1;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, !p.empty());
  for (int i = 0; i< p.dimension(); i++)
  {
    vsl_b_write(os, p.min()[i]);
    vsl_b_write(os, p.max()[i]);
  }
}

//=========================================================================
//: Binary load self from stream.
template<class T, class DIM_>
void vsl_b_read(vsl_b_istream &is, vbl_bounding_box_base<T, DIM_> & p)
{
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case 1:
    p.reset(); // empty the bounding box
    bool b; vsl_b_read(is, b);
    if (b) {
      T min_point[DIM_::value], max_point[DIM_::value];
      for (int i = 0; i< p.dimension(); i++)
      {
        vsl_b_read(is, min_point[i]);
        vsl_b_read(is, max_point[i]);
      }
      p.update(min_point); p.update(max_point);
    }
    break;

   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_bounding_box_base<T, DIM_>&)\n"
             << "           Unknown version number "<< v << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//===========================================================================
//: Output a human readable summary to the stream
template<class T, class DIM_>
void vsl_print_summary(vcl_ostream& os,const vbl_bounding_box_base<T, DIM_> & p)
{
    os << '\n';
    if (p.empty())
      os << "empty\n";
    else {
      for (int i=0;i<p.dimension();i++)
      {
        os << "min[" << i << "] = " << p.min()[i] << '\n';
      }
      os << '\n';
      for (int i=0;i<p.dimension();i++)
      {
        os << "max[" << i << "] = " << p.max()[i] << '\n';
      }
    }
    os << '\n';
}

#define VBL_IO_BOUNDING_BOX_INSTANTIATE(T, DIM) \
template void vsl_print_summary(vcl_ostream&,const vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM > >&);\
template void vsl_b_read(vsl_b_istream &, vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM > > &); \
template void vsl_b_write(vsl_b_ostream &, const vbl_bounding_box_base<T, vbl_bounding_box_DIM<DIM >  > &)

#endif // vbl_io_bounding_box_txx_
