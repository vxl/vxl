// This is core/vil1/io/vil1_io_rgb.h
#ifndef vil1_io_rgb_h_
#define vil1_io_rgb_h_
//:
// \file
// \brief Contains functions vsl_b_write, vsl_b_read and vsl_print_summary
// \author Franck Bettinger
// \date 22 Mar 2001
//
// \verbatim
//  Modifications
//   2001/03/22 Franck Bettinger    Creation
// \endverbatim

#include <vil1/vil1_rgb.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vil1_rgb to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vil1_rgb<T>& v);

//: Binary load vil1_rgb from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vil1_rgb<T>& v);

//: Print human readable summary of a vil1_rgb object to a stream
template <class T>
void vsl_print_summary(vcl_ostream& os,const vil1_rgb<T>& v);

#endif // #ifndef vil1_io_rgb_h_
