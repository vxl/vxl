// This is vxl/vbl/vbl_protection_traits.h
#ifndef _vbl_protection_traits_h
#define _vbl_protection_traits_h
//:
// \file
// \brief Template to standardize protection of classes
// \author Rupert Curwen, GE CRD
// \date   6/3/98
//
// \verbatim
// Modifications
// PDA (Manchester) 23/03/2001: Tidied up the documentation
// \endverbatim

//: Template to standardize protection of classes
// The vbl_protection_traits class is specialized for each type which is to be
// contained as a reference counted pointer.  The specialized class defines
// the protect and unprotect methods to be used for the type.  The default is
// to use Protect and UnProtect.
template <class T> class vbl_protection_traits
{
 public:
  static void protect(T t);
  static void unprotect(T t);
  static void def(T& t);
};

#endif // _vbl_protection_traits_h
