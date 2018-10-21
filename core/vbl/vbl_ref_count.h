// This is core/vbl/vbl_ref_count.h
#ifndef vbl_ref_count_h
#define vbl_ref_count_h
//:
// \file
//
// \verbatim
//  Modifications
//   21/03/2001 PDA (Manchester) Tidied up the documentation
//   10/09/2004 Peter Vanroose   Inlined all 1-line methods in class decl
// \endverbatim

#include <vcl_atomic_count.h>

class vbl_ref_count
{
  vcl_atomic_count ref_count_;
 public:
  vbl_ref_count() : ref_count_(0) { }
  // Copying an object should not copy the ref count.
  vbl_ref_count(vbl_ref_count const&) : ref_count_(0) { }

  vbl_ref_count&
  operator=(vbl_ref_count const& /*rhs*/)
  { /* should not copy the ref count */ return *this; }

  virtual ~vbl_ref_count() = default;

  void ref() { ++ref_count_; }
  void unref() { /*assert(ref_count_>0);*/ if (--ref_count_ == 0) delete this; }
  long get_references() const { return ref_count_; }
  bool is_referenced() const { return ref_count_ > 0; }
};

#endif // vbl_ref_count_h
