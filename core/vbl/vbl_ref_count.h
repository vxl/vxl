// This is vxl/vbl/vbl_ref_count.h
#ifndef vbl_ref_count_h
#define vbl_ref_count_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim

#include "dll.h"

class vbl_ref_count
{
 public:
  vbl_ref_count() : ref_count(0) { }
  virtual ~vbl_ref_count();

  void ref() { ++ref_count; }

  void unref() { if (--ref_count <= 0) delete this; }

  int get_references() const { return ref_count; }

  bool is_referenced() const { return ref_count > 0; }

 protected:
  // this should be private.
  int ref_count;

 private:
  static VBL_DLL_DATA int verbosity_;
};

#endif // vbl_ref_count_h
