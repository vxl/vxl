#ifndef vbl_io_user_info_h
#define vbl_io_user_info_h
#ifdef __GNUC__
#pragma interface
#endif

#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_user_info.h>

// This is vxl/vbl/io/vbl_io_user_info.h

//:
// \file
// \brief binary io for vbl_user_info.
// \author Christine Beeston
// \date 22-Mar-2001

//: Binary save vbl_user_info to stream.
void vsl_b_write(vsl_b_ostream &os, const vbl_user_info & v);

//: Binary load vbl_user_info from stream.
void vsl_b_read(vsl_b_istream &is, vbl_user_info & v);

//: Print human readable summary of object to a stream
void vsl_print_summary(vcl_ostream& os,const vbl_user_info & v);


#endif // vbl_io_user_info_h
