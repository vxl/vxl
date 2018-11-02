// This is core/vbl/io/vbl_io_bounding_box.h
#ifndef vbl_io_bounding_box_h
#define vbl_io_bounding_box_h
//:
// \file
// \author P D Allen
// \date 21-Mar-2001

#include <iosfwd>
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_bounding_box.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save vbl_bounding_box to stream.
template <class T, class DIM_>
void vsl_b_write(vsl_b_ostream &os, const vbl_bounding_box_base<T, DIM_> & p);

//: Binary load vbl_bounding_box from stream.
template <class T, class DIM_>
void vsl_b_read(vsl_b_istream &is, vbl_bounding_box_base<T, DIM_> & p);

//: Print human readable summary of object to a stream
template <class T, class DIM_>
void vsl_print_summary(std::ostream& os,const vbl_bounding_box_base<T, DIM_> & p);

#endif // vbl_io_bounding_box_h
