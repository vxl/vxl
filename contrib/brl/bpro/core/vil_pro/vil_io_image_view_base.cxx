#include "vil_io_image_view_base.h"

#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view_base.h>


//: Binary write image view base to stream
void vsl_b_write(vsl_b_ostream & os, vil_image_view_base_sptr const& view_base)
{
  vcl_cerr << "warning: vsl_b_write not implemented for vil_image_view_base_sptr" << vcl_endl;
}


//: Binary load image view base from stream.
void vsl_b_read(vsl_b_istream & is, vil_image_view_base_sptr &view_base)
{
  vcl_cerr << "warning: vsl_b_read not implemented for vil_image_view_base_sptr" << vcl_endl;

}
//: Binary write voxel world to stream
void vsl_b_write(vsl_b_ostream & os, vil_image_view_base const& view_base)
{
  vcl_cerr << "warning: vsl_b_write not implemented for vil_image_view_base" << vcl_endl;
}


//: Binary load voxel world from stream.
void vsl_b_read(vsl_b_istream & is, vil_image_view_base &view_base)
{
  vcl_cerr << "warning: vsl_b_read not implemented for vil_image_view_base" << vcl_endl;
}
