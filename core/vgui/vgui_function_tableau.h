#ifndef vgui_function_tableau_h_
#define vgui_function_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_function_tableau - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_function_tableau.h
// .FILE vgui_function_tableau.cxx
//
// .SECTION Description
//
// vgui_function_tableau is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 25 Nov 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_tableau.h>
#include "dll.h"

class vgui_function_tableau : public vgui_tableau {
public:
  typedef bool (*function)(const vgui_event&);
  
  vgui_function_tableau();

  vcl_string type_name() const { return "vgui_function_tableau"; }
  
  void draw(function f) {draw_ = f;} 
  void mouse_up(function f) {mouse_up_ = f;} 
  void mouse_down(function f) {mouse_down_ = f;};
  void motion(function f) {motion_ = f;}
  void key_press(function f) {key_press_ = f;}
  void help(function f) {help_ = f;}
  

  static vgui_DLLDATA bool redraw;

protected: 
 ~vgui_function_tableau();
  bool handle(const vgui_event&);

  function draw_;
  function mouse_up_;
  function mouse_down_;
  function motion_;
  function key_press_;
  function help_;
};

#endif // vgui_function_tableau_h_
