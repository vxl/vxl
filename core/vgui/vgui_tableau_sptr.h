// This is core/vgui/vgui_tableau_sptr.h
#ifndef vgui_tableau_sptr_h_
#define vgui_tableau_sptr_h_
//:
// \file
// \brief  Smart-pointer to a vgui_tableau

class vgui_tableau;
#include <vbl/vbl_smart_ptr.h>

struct vgui_tableau_sptr : public vbl_smart_ptr<vgui_tableau> {
  typedef vbl_smart_ptr<vgui_tableau> base;

  vgui_tableau_sptr() {}
  vgui_tableau_sptr(vgui_tableau* p): base(p) {}
  void vertical_cast(vgui_tableau_sptr const& that) { *this = that; }
  void vertical_cast(vgui_tableau* t) { *this = t; }
};

// Stop doxygen documenting the B class
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class T, class B = vgui_tableau_sptr>
struct vgui_tableau_sptr_t : public B {
  vgui_tableau_sptr_t(): B() {}
  vgui_tableau_sptr_t(T* p): B(p) {}
  vgui_tableau_sptr_t(vgui_tableau_sptr_t<T> const& r): B(r) {}
  void operator=(vgui_tableau_sptr_t<T> const& r) { B::operator=(r); }
  T* operator->() const { return (T*)this->as_pointer(); }
  T& operator*() const { return *((T*)this->as_pointer()); }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // vgui_tableau_sptr_h_
