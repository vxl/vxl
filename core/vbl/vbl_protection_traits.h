#ifndef _vbl_protection_traits_h
#define _vbl_protection_traits_h
//-----------------------------------------------------------------------------
//
// .NAME vbl_protection_traits - Template to standardize protection of classes.
// .LIBRARY vbl
// .HEADER Basics Package
// .INCLUDE vbl/vbl_protection_traits.h
// .FILE vbl/vbl_protection_traits.cxx
//
// .SECTION Description
//
//   The vbl_protection_traits class is specialized for each type which is to be
//   contained as a reference counted pointer.  The specialized class defines
//   the protect and unprotect methods to be used for the type.  The default is
//   to use Protect and UnProtect.
//
//
// .SECTION See also
//   DBGenWrapper in Database.
//
// .SECTION Author
//   Rupert Curwen, GE CRD, 6/3/98
//
// .SECTION Modifications
//   <none yet>
//
//-----------------------------------------------------------------------------

template <class T> class vbl_protection_traits
{
public:
  static void protect(T t);
  static void unprotect(T t);
  static void def(T& t);
};

#endif // _vbl_protection_traits_h
