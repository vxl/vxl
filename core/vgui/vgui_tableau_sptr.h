// This is oxl/vgui/vgui_tableau_sptr.h
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

template <class T, class Base = vgui_tableau_sptr>
struct vgui_tableau_sptr_t : public Base {
  vgui_tableau_sptr_t(): Base() {}
  vgui_tableau_sptr_t(T* p): Base(p) {}
  vgui_tableau_sptr_t(vgui_tableau_sptr_t<T> const& r): Base(r) {}
  void operator=(vgui_tableau_sptr_t<T> const& r) { Base::operator=(r); }
  T* operator->() const { return (T*)as_pointer(); }
  //  typedef vgui_tableau_sptr_t<T> self_or_base;
};

#endif // vgui_tableau_sptr_h_
