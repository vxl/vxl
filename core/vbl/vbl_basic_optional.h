// This is core/vbl/vbl_basic_optional.h
#ifndef vbl_basic_optional_h
#define vbl_basic_optional_h
//:
// \file
// \author Rupert Curwen GE Corporate Research and Development
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// IMS (Manchester) 21/10/2003: Deprecated - Decision at Providence 2002 Meeting.
//                              Can be deleted after VXL-1.1
//        
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_deprecated_header.h>

#ifdef __STL_USE_ABBREVS
#define vbl_basic_optional vbl_bRo
#endif

//: Default parameter for optional arguments of a templated type.
// vbl_basic_optional is templated and specialized to support
// templated classes which have optional arguments.  The traits
// class defined here enables compile time optimization of such
// templates so that only the appropriate pieces of code are
// included in each instantiation.
//
// \deprecated Because no-one knows what it does.
class vbl_basic_optional
{
 public:
  vbl_basic_optional() {}
  vbl_basic_optional(const vbl_basic_optional&) {}
  inline static void Error(const char* message)
  { vcl_cerr << message << vcl_endl; vcl_abort(); }
};

// Here is the general case for the traits class used to determine
// whether or not a given class is vbl_basic_optional.  Uses an enum because
// it is more widely supported than a static const bool.
// vbl_basic_optional_traits<T>::IsOptional == false.
//
// \deprecated Because no-one knows what it does.
template <class T>
class vbl_basic_optional_traits
{
 public:
  enum Enum { IsOptional = false };
};

// Here is the special case for the vbl_basic_optional class, for which
// vbl_basic_optional_traits<vbl_basic_optional>::IsOptional == true.
class vbl_basic_optional_traits<vbl_basic_optional>
{
 public:
  enum Enum { IsOptional = true };
};

inline vcl_ostream& operator<<(vcl_ostream& str, const vbl_basic_optional&)
{
  return str;
}

inline bool operator==(const vbl_basic_optional&, const vbl_basic_optional&)
{
  return true;
}

inline bool operator!=(const vbl_basic_optional&, const vbl_basic_optional&)
{
  return false;
}

#include <vcl_hash.h>
struct vcl_hash<vbl_basic_optional>
{
  vcl_size_t operator()(const vbl_basic_optional& /*s*/) const { return 0; }
};

#endif // vbl_basic_optional_h
