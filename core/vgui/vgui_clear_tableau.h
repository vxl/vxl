// This is ./oxl/vgui/vgui_clear_tableau.h

//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Nov 99
// \brief  Tableau performing OpenGL clearing.
//
// \verbatim
//  Modifications:
//    11-NOV-1999 P.Pritchett - Initial version.
//    26-APR-2002 K.Y.McGaul - Added some doxygen style comments.
// \endverbatim

#ifndef vgui_clear_tableau_h_
#define vgui_clear_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif

#include "vgui_clear_tableau_sptr.h"
#include <vgui/vgui_gl.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_slot.h>
class vgui_menu;

//: Tableau performing OpenGL clearing.
//
// vgui_clear_tableau is a tableau that performs OpenGL clearing upon
// receipt of a vgui_DRAW event. It passes all events to its single child.
class vgui_clear_tableau : public vgui_tableau {
public:
  vgui_clear_tableau();

  vcl_string type_name() const;

  //: Set colour of clear_tableau to the given red, green, blue values.
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

struct vgui_clear_tableau_new : public vgui_clear_tableau_sptr {
  typedef vgui_clear_tableau_sptr base;
  vgui_clear_tableau_new() : base(new vgui_clear_tableau()) { }
};

#endif // vgui_clear_tableau_h_
