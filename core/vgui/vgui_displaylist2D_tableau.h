// This is core/vgui/vgui_displaylist2D_tableau.h
#ifndef vgui_displaylist2D_tableau_h_
#define vgui_displaylist2D_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Displaying two-dimensional geometric objects.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   14 Sep 99
//
// \verbatim
//  Modifications
//   01-SEP-2000 Marko Bacic,Oxford RRG - Added displaying of object information
//   01-OCT-2002 K.Y.McGaul - Moved displaylist2D to displaylist2D_tableau.
//   26-JUN-2003 Mark Johnson - Fixed overlay highlighting.
// \endverbatim

#include "vgui_displaybase_tableau.h"
#include <vgui/vgui_displaylist2D_tableau_sptr.h>

//: Displaying two-dimensional geometric objects.
class vgui_displaylist2D_tableau : public vgui_displaybase_tableau
{
 public:
  vgui_displaylist2D_tableau() {}

  virtual bool handle(const vgui_event& e);
  bool motion(int x, int y);
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

 protected:
  ~vgui_displaylist2D_tableau() {}

 private:
  void get_hits(float x, float y, vcl_vector<unsigned>& my_hits);
  unsigned find_closest(float x, float y, vcl_vector<unsigned> const& hits);
};

//: Create a smart-pointer to a vgui_displaylist2D_tableau.
struct vgui_displaylist2D_tableau_new : public vgui_displaylist2D_tableau_sptr
{
  vgui_displaylist2D_tableau_new()
    : vgui_displaylist2D_tableau_sptr(new vgui_displaylist2D_tableau) {}
};

#endif // vgui_displaylist2D_tableau_h_
