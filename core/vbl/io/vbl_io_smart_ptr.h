// This is core/vbl/io/vbl_io_smart_ptr.h
#ifndef vbl_io_smart_ptr_h
#define vbl_io_smart_ptr_h
//:
// \file
// \brief Serialised binary IO functions for vbl_smart_ptr<T>
// \author Ian Scott (Manchester)
// \date 26-Mar-2001
//
// In order to use IO for smart pointers you will need to have
// the IO functions defined for pointers to MY_CLASS (class T.)
// If you have written I/O for polymorphic classes, some of these
// functions may already be defined.
//
// If you need to write them, you can use the following as examples
// \code
// void vsl_b_read(vsl_b_istream& is, impl * &p)
// {
//   delete p;
//   bool not_null_ptr;
//   vsl_b_read(is, not_null_ptr);
//   if (not_null_ptr)
//   {
//     p = new MY_CLASS();
//     vsl_b_read(is, *p);
//   }
//   else
//     p = 0;
// }
//
// void vsl_b_write(vsl_b_ostream& os, const MY_CLASS *p)
// {
//   if (p==0)
//   {
//     vsl_b_write(os, false); // Indicate null pointer stored
//   }
//   else
//   {
//     vsl_b_write(os,true); // Indicate non-null pointer stored
//     vsl_b_write(os,*p);
//   }
// }
//
// void vsl_print_summary(std::ostream& os, const MY_CLASS *p)
// {
//   if (p==0)
//     os << "NULL PTR";
//   else
//   {http://www.isbe.man.ac.uk/internal/software/c++/vxl-doxygen/
//                              vcl/html/class_vcl_not_equal_to.html
//     os << "T: ";
//     vsl_print_summary(os, *p);
//   }
// }
// \endcode
//
// Objects using I/O via a smart ptr, should not save the objects reference count.

#include <iosfwd>
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_smart_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save vbl_smart_ptr to stream.
template <class T>
void vsl_b_write(vsl_b_ostream & os, const vbl_smart_ptr<T> & v);

//: Binary load vbl_smart_ptr from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vbl_smart_ptr<T> & v);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream & os,const vbl_smart_ptr<T> & b);

#endif // vbl_io_smart_ptr_h
