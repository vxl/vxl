// This is core/vbl/vbl_basic_relation_type.h
#ifndef vbl_basic_relation_type_h
#define vbl_basic_relation_type_h
//:
// \file
// \author Rupert Curwen, GE Corporate Research and Development
// \date   Aug 28th, 1998
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// Peter Vanroose - Jan 2002 - Removed DEX support
// IMS (Manchester) 21/10/2003: Deprecated - Decision at Providence 2002 Meeting.
//                              Can be deleted after VXL-1.1
// \endverbatim
//

#include <vcl_deprecated_header.h>


//: A base class for all vbl_basic_relations; provides type-safe downcasting
//
// \deprecated Because no-one knows what it does.
class vbl_basic_relation_type
{
 public:
  virtual ~vbl_basic_relation_type() {}

  // This virtual function returns the address of a static variable
  // which is unique for each vbl_basic_relation templated type.
  virtual void* GetType() { return NULL; }
  virtual void* This() { return (void*)this; }
};

#endif // vbl_basic_relation_type_h
