// This is core/vbl/vbl_protection_traits.h
#ifndef vbl_protection_traits_h_
#define vbl_protection_traits_h_
//:
// \file
// \brief Template to standardize protection of classes
// \author Rupert Curwen, GE CRD
// \date   6/3/98
//
// \verbatim
// Modifications
// PDA (Manchester) 23/03/2001: Tidied up the documentation
// IMS (Manchester) 21/10/2003: Deprecated - Decision at Providence 2002 Meeting.
//                              Can be deleted after VXL-1.1
// \endverbatim

#include <vcl_deprecated_header.h>

//: Template to standardize protection of classes
// The vbl_protection_traits class is specialized for each type which is to be
// contained as a reference counted pointer.  The specialized class defines
// the protect and unprotect methods to be used for the type.  The default is
// to use Protect and UnProtect.
//
// \deprecated Because no-one knows what it does.
template <class T> class vbl_protection_traits
{
 public:
  static void protect(T t);
  static void unprotect(T t);
  static void def(T& t);
};

#endif // vbl_protection_traits_h_
