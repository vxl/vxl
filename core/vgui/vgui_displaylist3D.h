// This is oxl/vgui/vgui_displaylist3D.h

//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   14 Sep 99
// \brief  Displaying of three-dimensional geometric objects.

#ifndef vgui_displaylist3D_h_
#define vgui_displaylist3D_h_
#ifdef __GNUC__
#pragma interface
#endif

#include "vgui_displaybase.h"

#include <vgui/vgui_drag_mixin.h>

class vgui_soview3D;

//: Displaying of three-dimensional geometric objects
//              
// \verbatim
//  Modifications:
//    14-SEP-1999 P.Pritchett - Initial version.
// \endverbatim
class vgui_displaylist3D : public vgui_displaybase, public vgui_drag_mixin { 
public:
  vgui_displaylist3D();

  vcl_string type_name() const { return "vgui_displaylist3D"; }

  bool handle(const vgui_event& e);
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

protected:
 ~vgui_displaylist3D();

private:
  void get_hits(float x, float y, vcl_vector<unsigned>& my_hits);
};

#endif // vgui_displaylist3D_h_
