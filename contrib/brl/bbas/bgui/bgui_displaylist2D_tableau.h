// This is core/vgui/bgui_displaylist2D_tableau.h
#ifndef bgui_displaylist2D_tableau_h_
#define bgui_displaylist2D_tableau_h_
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
//   13-JUN-2003 M.R. Johnson - Not using this to display highlights anymore.
// \endverbatim

#include "bgui_displaybase_tableau.h"
#include <bgui/bgui_displaylist2D_tableau_sptr.h>

//: Displaying two-dimensional geometric objects.
class bgui_displaylist2D_tableau : public bgui_displaybase_tableau
{
 public:
  bgui_displaylist2D_tableau();

  virtual bool handle(const vgui_event& e);
  bool motion(int x, int y);
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

 protected:
 ~bgui_displaylist2D_tableau();

 private:
  void get_hits(float x, float y, vcl_vector<unsigned>& my_hits);
  unsigned find_closest(float x, float y, vcl_vector<unsigned>& hits);

  bool posted_redraw_;
};

//: Create a smart-pointer to a bgui_displaylist2D_tableau.
struct bgui_displaylist2D_tableau_new : public bgui_displaylist2D_tableau_sptr
{
  bgui_displaylist2D_tableau_new() :
    bgui_displaylist2D_tableau_sptr(new bgui_displaylist2D_tableau) { }
};

#endif // bgui_displaylist2D_tableau_h_
