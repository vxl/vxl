// This is core/vgui/vgui_function_tableau.h
#ifndef vgui_function_tableau_h_
#define vgui_function_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Tableau where functions passed in are called for various events.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   25 Nov 99
//
//  Contains class  vgui_function_tableau
//
// \verbatim
//  Modifications
//   25-NOV-1999 P.Pritchett - Initial version.
//   02-OCT-2002 K.Y.McGaul - Added vgui_function_tableau_new.
//                          - Added comments for the functions.
//   14-OCT-2003 P.Vanroose - Added implementation for 5 missing tableau virtuals
// \endverbatim

#include <vgui/vgui_tableau.h>
#include "vgui_function_tableau_sptr.h"
#include "dll.h"

//:  Tableau where functions passed in are called for various events.
//
//   For example, you can pass in my_draw_function() to draw() and
//   it will be called every time a draw event occurs in this tableau.
class vgui_function_tableau : public vgui_tableau
{
 public:
  typedef bool (*function)(const vgui_event&);

  //: Constructor - don't use this, use vgui_function_tableau_new.
  //  Creates a default vgui_function tableau.
  vgui_function_tableau();

  //: Returns the type of this tableau ('vgui_function_tableau').
  vcl_string type_name() const { return "vgui_function_tableau"; }

  //: Call the given function when a draw event occurs.
  void draw(function f) {draw_ = f;}
  virtual bool draw();

  //: Call the given function when a mouse up event occurs.
  void mouse_up(function f) {mouse_up_ = f;}
  virtual bool mouse_up(int x, int y, vgui_button, vgui_modifier);

  //: Call the given function when a mouse down event occurs.
  void mouse_down(function f) {mouse_down_ = f;}
  virtual bool mouse_down(int x, int y, vgui_button, vgui_modifier);

  //: Call the given function when a mouse motion event occurs.
  void motion(function f) {motion_ = f;}
  virtual bool motion(int x, int y);

  //: Call the given function when a key is pressed by the user.
  void key_press(function f) {key_press_ = f;}
  virtual bool key_press(int x, int y, vgui_key, vgui_modifier);

  //: Call the given function when the '?' or 'help' key is pressed by the user.
  void help(function f) {help_ = f;}
  virtual bool help();

  static vgui_DLLDATA bool redraw;

 protected:
  //: Destructor - called by vgui_function_tableau_sptr.
 ~vgui_function_tableau();

  //: Handle all events by passing them to the appropriate functions.
  bool handle(const vgui_event&);

  function draw_;
  function mouse_up_;
  function mouse_down_;
  function motion_;
  function key_press_;
  function help_;
};

//: Creates a smart-pointer to a vgui_function_tableau.
struct vgui_function_tableau_new : public vgui_function_tableau_sptr
{
  //: Constructor - create a default vgui_function_tableau.
  vgui_function_tableau_new( )
    : vgui_function_tableau_sptr(new vgui_function_tableau) { }
};

#endif // vgui_function_tableau_h_
