// This is vxl/vil/io/vil_io_memory_image_format.h
#ifndef vil_io_memory_image_format_h_
#define vil_io_memory_image_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file 
// \date 22 Mar 2001
// \author Franck Bettinger
//
// \verbatim
// Modifications :
// 2001/03/22 Franck Bettinger  Creation
// \endverbatim

#include <vil/vil_memory_image.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vil_memory_image_format to stream.
void vsl_b_write(vsl_b_ostream &os, const vil_memory_image_format& v);

//: Binary load vil_memory_image_format from stream.
void vsl_b_read(vsl_b_istream &is, vil_memory_image_format& v);

//: Print human readable summary of a vil_memory_image_format object
void vsl_print_summary(vcl_ostream& os,const vil_memory_image_format& v);

#endif // #ifndef vil_io_memory_image_format_h_
