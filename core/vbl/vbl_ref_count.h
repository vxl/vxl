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

class vbl_ref_count
{
  int ref_count_;
 public:
  vbl_ref_count() : ref_count_(0) { }
  virtual ~vbl_ref_count() {}
  void ref() { ++ref_count_; }
  void unref() { if (--ref_count_ <= 0) delete this; }
  int get_references() const { return ref_count_; }
  bool is_referenced() const { return ref_count_ > 0; }
};

#endif // vbl_ref_count_h
