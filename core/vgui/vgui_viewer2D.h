#ifndef vgui_viewer2D_h_
#define vgui_viewer2D_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_viewer2D - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_viewer2D.h
// .FILE vgui_viewer2D.cxx
//
// .SECTION Description
//
// vgui_viewer2D is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 14 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//		23-AUG-2000 Marko Bacic, Oxford RRG -- Added support for scroll bars
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_drag_mixin.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_viewer2D_ref.h>

#include "dll.h"

class vgui_viewer2D : public vgui_wrapper_tableau, public vgui_drag_mixin {
public:
  vgui_viewer2D(vgui_tableau_ref const&);

  // vgui_tableau methods
  virtual bool handle(const vgui_event& event);
  virtual vcl_string type_name() const;
  
  void setup_gl_matrices();

  void zoomin(float zoom_factor, int x, int y);
  void zoomout(float zoom_factor, int x, int y);
  void center_image(int w, int h);
  void center_event();

  // drag mixin methods
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);
  bool mouse_drag(int x, int y,  vgui_button button, vgui_modifier modifier);
  bool mouse_up(int x, int y,  vgui_button button, vgui_modifier modifier);
  bool help();
  bool key_press(int x, int y, vgui_key key, vgui_modifier);
  
  // data
  struct token_t {
    float scaleX;
    float scaleY;
    float offsetX;
    float offsetY;
    token_t() : scaleX(1.0f), scaleY(1.0f), offsetX(0.0f), offsetY(0.0f) { }
  } token;
  
  // static data
  static vgui_DLLDATA const void * const CENTER_EVENT;


  bool nice_points;
  bool nice_lines;

  enum {normal_zoom, smooth_zoom} zoom_type;

  bool panning;
  bool smooth_zooming;
  bool sweep_zooming;
  bool sweep_next;
  float prev_x,prev_y;
  float zoom_x,zoom_y;
  float new_x,new_y;
  float zoom_factor;

protected:
 ~vgui_viewer2D();
};

struct vgui_viewer2D_new : public vgui_viewer2D_ref {
  vgui_viewer2D_new(vgui_tableau_ref const& that) : 
    vgui_viewer2D_ref(new vgui_viewer2D(that)) { }
};

#endif // vgui_viewer2D_h_
