// This is core/vgui/vgui_tableau_sptr.h
#ifndef vgui_tableau_sptr_h_
#define vgui_tableau_sptr_h_
//:
// \file
// \brief  Smart-pointer to a vgui_tableau

class vgui_tableau;
#include <vbl/vbl_smart_ptr.h>

struct vgui_tableau_sptr : public vbl_smart_ptr<vgui_tableau>
{
  typedef vbl_smart_ptr<vgui_tableau> base;

  vgui_tableau_sptr() {}
  vgui_tableau_sptr(vgui_tableau * p)
    : base(p)
  {}
  void
  vertical_cast(const vgui_tableau_sptr & that)
  {
    *this = that;
  }
  void
  vertical_cast(vgui_tableau * t)
  {
    *this = t;
  }
};

// Stop doxygen documenting the B class
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class T, class B = vgui_tableau_sptr>
struct vgui_tableau_sptr_t : public B
{
  vgui_tableau_sptr_t()
    : B()
  {}
  vgui_tableau_sptr_t(T * p)
    : B(p)
  {}
  vgui_tableau_sptr_t(const vgui_tableau_sptr_t<T> & r)
    : B(r)
  {}
  vgui_tableau_sptr_t<T> &
  operator=(const vgui_tableau_sptr_t<T> & r)
  {
    B::operator=(r);
    return *this;
  }
  T *
  operator->() const
  {
    return (T *)this->as_pointer();
  }
  T &
  operator*() const
  {
    return *((T *)this->as_pointer());
  }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // vgui_tableau_sptr_h_
