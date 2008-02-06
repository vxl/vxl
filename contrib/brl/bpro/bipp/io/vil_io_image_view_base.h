#ifndef vil_io_image_view_base_h_
#define vil_io_image_view_base_h_

#include <vsl/vsl_binary_io.h>

#include <vil/vil_image_view_base.h>



//: Binary write voxel world to stream
void vsl_b_write(vsl_b_ostream & os, vil_image_view_base_sptr const& view_base);


//: Binary load voxel world from stream.
void vsl_b_read(vsl_b_istream & is, vil_image_view_base_sptr &view_base);




#endif