// This is oxl/ogui/examples/geoff/vgui_testbed/vgui_point_selector.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vgui_point_selector.h"

#include <vcl_iostream.h>

#include <vgui/vgui_event.h>
#include <vgui/vgui_projection_inspector.h>

// Default ctor
example_point_selector::example_point_selector( vgui_tableau_sptr child)
  : vgui_wrapper_tableau( child)
{
}


bool example_point_selector::handle( vgui_event const &e)
{
  // just display any time the mouse button is pressed
  if (e.type== vgui_BUTTON_DOWN)
    {
      // undo the window transformation
      vgui_projection_inspector pi;
      float ix, iy;
      pi.window_to_image_coordinates( e.wx, e.wy, ix, iy);

      if (e.button== vgui_LEFT)
        vcl_cerr << "Left mouse press at : " << ix << " " << iy << vcl_endl;
      else if (e.button== vgui_MIDDLE)
        vcl_cerr << "Middle mouse press at : " << ix << " " << iy << vcl_endl;
      else if (e.button== vgui_RIGHT)
        vcl_cerr << "Right mouse press at : " << ix << " " << iy << vcl_endl;
    }

  return child && child->handle( e);
}
