// This is vxl/vil/io/vil_io_rgb.h
#ifndef vil_io_rgb_h_
#define vil_io_rgb_h_
//:
// \file 
// \date 22 Mar 2001
// \author Franck Bettinger
//
// \verbatim
// Modifications :
// 2001/03/22 Franck Bettinger    Creation
// \endverbatim

#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>

//: Binary save vil_rgb to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const vil_rgb<T>& v);

//: Binary load vil_rgb from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, vil_rgb<T>& v);

//: Print human readable summary of a vil_rgb object to a stream
template <class T>
void vsl_print_summary(vcl_ostream& os,const vil_rgb<T>& v);

#endif // #ifndef vil_io_rgb_h_
