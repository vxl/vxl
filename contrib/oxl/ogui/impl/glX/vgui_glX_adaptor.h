// This is oxl/vgui/impl/glX/vgui_glX_adaptor.h
#ifndef vgui_glX_adaptor_h_
#define vgui_glX_adaptor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Undocumented class FIXME
//
// \author
//              Philip C. Pritchett, 11 Sep 99
//              Robotics Research Group, University of Oxford
//-----------------------------------------------------------------------------

#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>

class vgui_glX_window;

class vgui_glX_adaptor : public vgui_adaptor, public vgui_adaptor_mixin
{
 public:
  typedef vgui_adaptor_mixin mixin;

  vgui_glX_adaptor();
  ~vgui_glX_adaptor();

  // vgui_adaptor methods
  void swap_buffers();
  void make_current();
  void post_redraw() { draw(); }
  void post_overlay_redraw() { draw(); }

  unsigned get_width() const  {return mixin::width;}
  unsigned get_height() const {return mixin::height;}
  void get_popup_bindings(vgui_modifier &m, vgui_button &b) const
    { m = mixin::popup_modifier; b = mixin::popup_button; }

  //
  void draw();
  vgui_glX_window *window;

 protected:
  bool use_popups;
};

#endif // vgui_glX_adaptor_h_
