// This is vil/io/vil_io_image.h
#ifndef vil_io_image_h_
#define vil_io_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Perform serialised binary IO for vil_image
// \author Ian Scott (Manchester)
// \date 23-Mar-2001

#include <vsl/vsl_binary_io.h>

// Predeclare classes
class vil_image;

//: Binary save vgl_my_class to stream.
void vsl_b_write(vsl_b_ostream &os, const vil_image & b);

//: Binary load vgl_my_class from stream.
void vsl_b_read(vsl_b_istream &is, vil_image & b);

//: Print human readable summary of object to a stream
void vsl_print_summary(vcl_ostream& os,const vil_image & b);

#endif // vil_io_image_h_
