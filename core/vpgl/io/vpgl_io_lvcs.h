#ifndef vpgl_io_lvcs_h_
#define vpgl_io_lvcs_h_
//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>

//: Binary save lvcs to stream
void vsl_b_write(vsl_b_ostream & os, vpgl_lvcs const& lvcs);

//: Binary load lvcs from stream.
void vsl_b_read(vsl_b_istream & is, vpgl_lvcs &lvcs);

//: Print human readable summary of object to a stream
void vsl_print_summary(std::ostream& os,const vpgl_lvcs & l);

//: Binary save lvcs sptr to stream
void vsl_b_write(vsl_b_ostream & os, vpgl_lvcs_sptr const& lvcs_sptr);

//: Binary load lvcs from stream.
void vsl_b_read(vsl_b_istream & is, vpgl_lvcs_sptr &lvcs);

#endif
