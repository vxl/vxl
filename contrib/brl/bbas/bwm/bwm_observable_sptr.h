#ifndef bwm_observable_sptr_h_
#define bwm_observable_sptr_h_
//:
// \file
// \brief  Smart-pointer to a bwm_observable
class bwm_observable;
#include <vbl/vbl_smart_ptr.h>

//typedef vbl_smart_ptr<bwm_observable> bwm_observable_sptr;

struct bwm_observable_sptr : public vbl_smart_ptr<bwm_observable> {
  typedef vbl_smart_ptr<class bwm_observable> base;

  bwm_observable_sptr() {}
  bwm_observable_sptr(bwm_observable* p): base(p) {}
  void vertical_cast(bwm_observable_sptr const& that) { *this = that; }
  void vertical_cast(bwm_observable* t) { *this = t; }
};

// Stop doxygen documenting the B class
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class T, class B = bwm_observable_sptr>
struct bwm_observable_sptr_t : public B {
  bwm_observable_sptr_t(): B() {}
  bwm_observable_sptr_t(T* p): B(p) {}
  bwm_observable_sptr_t(bwm_observable_sptr_t<T> const& r): B(r) {}
  void operator=(bwm_observable_sptr_t<T> const& r) { B::operator=(r); }
  T* operator->() const { return (T*)this->as_pointer(); }
  T& operator*() const { return *((T*)this->as_pointer()); }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // bwm_observable_sptr_h_
