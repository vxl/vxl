// This is core/vgui/vgui_vil_image_tableau_sptr.h
#ifndef vgui_vil_image_tableau_sptr_h
#define vgui_vil_image_tableau_sptr_h
//:
// \file
// \brief  Smart-pointer to a vgui_vil_image_tableau.

#include <vgui/vgui_tableau_sptr.h>

class vgui_vil_image_tableau_base;

template <class T>
class vgui_vil_image_tableau;

typedef vgui_tableau_sptr_t<vgui_vil_image_tableau_base> vgui_vil_image_tableau_base_sptr;

template <class T>
struct vgui_vil_image_tableau_sptr : public vgui_vil_image_tableau_base_sptr {
  typedef vgui_vil_image_tableau_base_sptr B; // base
  vgui_vil_image_tableau_sptr(): B() {}
  vgui_vil_image_tableau_sptr( vgui_vil_image_tableau<T>* p): B(p) {}
  vgui_vil_image_tableau_sptr( vgui_vil_image_tableau_sptr<T> const& r): B(r) {}
  void operator=(vgui_vil_image_tableau_sptr<T> const& r) { B::operator=(r); }
  vgui_vil_image_tableau<T>* operator->() const { return (vgui_vil_image_tableau<T>*)as_pointer(); }
};


#endif // vgui_vil_image_tableau_sptr_h
