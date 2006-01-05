// This is core/vnl/xio/vnl_xio_vector_fixed.h
#ifndef vnl_xio_vector_fixed_h
#define vnl_xio_vector_fixed_h
//:
// \file
// \author Amitha Perera
// \date Oct 2002

#include <vsl/vsl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iosfwd.h>

//: XML save vnl_vector_fixed to stream.
template <class T, unsigned n>
void x_write(vcl_ostream & os, const vnl_vector_fixed<T,n> & v, vcl_string name);

#endif // vnl_io_vector_fixed_h
