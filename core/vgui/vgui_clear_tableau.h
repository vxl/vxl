#ifndef vgui_clear_tableau_h_
#define vgui_clear_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_clear_tableau - tableau performing OpenGL clearing
// .LIBRARY vgui
// .INCLUDE vgui/vgui_clear_tableau.h
// .FILE vgui_clear_tableau.cxx
//
// .SECTION Description:
//
// vgui_clear_tableau is a tableau that performs OpenGL clearing upon 
// receipt of a vgui_DRAW event. It passes all events to its single child. 
//
// .SECTION Author:
//              Philip C. Pritchett, 11 Nov 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include "vgui_clear_tableau_ref.h"
#include <vgui/vgui_gl.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_slot.h>
class vgui_menu;

class vgui_clear_tableau : public vgui_tableau {
public:
  vgui_clear_tableau();

  vcl_string type_name() const;

  void set_colour(float r, float g, float b, float a=1);
  void set_mask(GLbitfield);

  void toggle_clearing();

  void add_popup(vgui_menu &menu);
  void config_dialog();

protected:
  virtual ~vgui_clear_tableau();
  virtual bool handle(const vgui_event&);
  
private:
  GLbitfield mask;
  
  float colour[4]; // rgba
  float accum[4];  // rgba
  GLclampd depth;
  GLint stencil;

  bool clearing_;
};

struct vgui_clear_tableau_new : public vgui_clear_tableau_ref {
  typedef vgui_clear_tableau_ref base;
  vgui_clear_tableau_new() : base(new vgui_clear_tableau()) { }
};

#endif // vgui_clear_tableau_h_
