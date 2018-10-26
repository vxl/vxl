// This is brl/bseg/bapl/bapl_keypoint_sptr.h
#ifndef bapl_keypoint_sptr_h_
#define bapl_keypoint_sptr_h_
//:
// \file
// \brief Smart pointer to a keypoint
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sat Nov 8 2003
//

class bapl_keypoint;
class bapl_keypoint_set;

#include <vbl/vbl_smart_ptr.h>

struct bapl_keypoint_sptr : public vbl_smart_ptr<bapl_keypoint> {
  typedef vbl_smart_ptr<bapl_keypoint> base;

  bapl_keypoint_sptr() = default;
  bapl_keypoint_sptr(bapl_keypoint* p): base(p) {}
  void vertical_cast(bapl_keypoint_sptr const& that) { *this = that; }
  void vertical_cast(bapl_keypoint* t) { *this = t; }
};

// Stop doxygen documenting the B class
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class T, class B = bapl_keypoint_sptr>
struct bapl_keypoint_sptr_t : public B {
  bapl_keypoint_sptr_t(): B() {}
  bapl_keypoint_sptr_t(T* p): B(p) {}
  bapl_keypoint_sptr_t(bapl_keypoint_sptr_t<T> const& r): B(r) {}
  void operator=(bapl_keypoint_sptr_t<T> const& r) { B::operator=(r); }
  T* operator->() const { return (T*)B::as_pointer(); }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // bapl_keypoint_sptr_h_
