// This is core/vgui/vgui_displaylist3D_tableau.h
#ifndef vgui_displaylist3D_tableau_h_
#define vgui_displaylist3D_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Displaying of three-dimensional geometric objects.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   14 Sep 99
//
// \verbatim
//  Modifications
//   14-SEP-1999 P.Pritchett - Initial version.
//   01-OCT-2002 K.Y.McGaul - Moved displaylist3D to displaylist3D_tableau.
//   02-OCT-2002 K.Y.McGaul - Added vgui_displaylist3D_tableau_new.
// \endverbatim

#include <vgui/vgui_displaybase_tableau.h>
#include <vgui/vgui_displaylist3D_tableau_sptr.h>
#include <vgui/vgui_drag_mixin.h>

class vgui_soview3D;

//: Displaying of three-dimensional geometric objects
class vgui_displaylist3D_tableau : public vgui_displaybase_tableau, public vgui_drag_mixin
{
 public:
  //: Constructor - don't use this, use vgui_displaylist3D_tableau_new.
  vgui_displaylist3D_tableau() {}

  vcl_string type_name() const { return "vgui_displaylist3D_tableau"; }

  bool handle(const vgui_event& e);
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

 protected:
  //: Destructor - called by vgui_displaylist3D_tableau_sptr.
  ~vgui_displaylist3D_tableau() {}

 private:
  void get_hits(float x, float y, vcl_vector<unsigned>& my_hits);
};

//: Creates a smart-pointer to a vgui_displaylist3D_tableau.
struct vgui_displaylist3D_tableau_new : public vgui_displaylist3D_tableau_sptr
{
  //: Constructor - create a default vgui_displaylist3D_tableau.
  vgui_displaylist3D_tableau_new( )
    : vgui_displaylist3D_tableau_sptr(new vgui_displaylist3D_tableau) {}
};

#endif // vgui_displaylist3D_tableau_h_
