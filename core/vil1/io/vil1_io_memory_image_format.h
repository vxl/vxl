// This is core/vil1/io/vil1_io_memory_image_format.h
#ifndef vil1_io_memory_image_format_h_
#define vil1_io_memory_image_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Contains functions vsl_b_write, vsl_b_read and vsl_print_summary
// \author Franck Bettinger
// \date 22 Mar 2001
//
// \verbatim
//  Modifications
//   2001/03/22 Franck Bettinger  Creation
// \endverbatim

#include <vil1/vil1_memory_image.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vil1_memory_image_format to stream.
void vsl_b_write(vsl_b_ostream &os, const vil1_memory_image_format& v);

//: Binary load vil1_memory_image_format from stream.
void vsl_b_read(vsl_b_istream &is, vil1_memory_image_format& v);

//: Print human readable summary of a vil1_memory_image_format object
void vsl_print_summary(vcl_ostream& os,const vil1_memory_image_format& v);

#endif // #ifndef vil1_io_memory_image_format_h_
