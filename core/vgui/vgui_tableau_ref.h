#ifndef vgui_tableau_ref_h_
#define vgui_tableau_ref_h_

class vgui_tableau;
#include <vbl/vbl_smart_ptr.h>

struct vgui_tableau_ref : public vbl_smart_ptr<vgui_tableau> {
  typedef vbl_smart_ptr<vgui_tableau> base;

  vgui_tableau_ref() {}
  vgui_tableau_ref(vgui_tableau* p): base(p) {}
  void vertical_cast(vgui_tableau_ref const& that) { *this = that; }
  void vertical_cast(vgui_tableau* t) { *this = t; }
};

template <class T, class Base = vgui_tableau_ref>
struct vgui_tableau_ref_t : public Base {
  vgui_tableau_ref_t(): Base() {}
  vgui_tableau_ref_t(T* p): Base(p) {}
  vgui_tableau_ref_t(vgui_tableau_ref_t<T> const& r): Base(r) {}
  void operator=(vgui_tableau_ref_t<T> const& r) { Base::operator=(r); }
  T* operator->() const { return (T*)as_pointer(); }
  //  typedef vgui_tableau_ref_t<T> self_or_base;
};

#endif // vgui_tableau_ref_h_
