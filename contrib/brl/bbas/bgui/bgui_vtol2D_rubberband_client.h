// This is brl/bbas/bgui/bgui_vtol2D_rubberband_client.h
#ifndef bgui_vtol2D_rubberband_client_h_
#define bgui_vtol2D_rubberband_client_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A client for rubberbanding stuff
// \author  J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy November 28, 2002    Initial version.
//   Peter Vanroose August 2004      Moved into separate file
// \endverbatim
//-----------------------------------------------------------------------------
#include <vgui/vgui_rubberband_tableau.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>


class bgui_vtol2D_rubberband_client : public vgui_rubberband_client
{
 public:
  bgui_vtol2D_tableau_sptr vtol2D_;

  //:constructor - takes a pointer to a vtol2D tableau
  bgui_vtol2D_rubberband_client(bgui_vtol2D_tableau_sptr const& vtol2D)
  : vtol2D_(vtol2D) {}

  //: Called by vgui_rubberband_tableau when the user has selected a point.
  virtual void add_point(float, float);

  //: Called by vgui_rubberband_tableau when the user has selected a finite line.
  virtual void add_line(float,float,float,float);

  //: Called by vgui_rubberband_tableau when user has selected an infinite line.
  virtual void add_infinite_line(float,float,float);

  //: Called by vgui_rubberband_tableau when the user has selected a circle.
  virtual void add_circle(float,float,float);

  //: Called by vgui_rubberband_tableau when the user has selectd a linestrip.
  virtual void add_linestrip(int n,float const *,float const *);

  //: Called by vgui_rubberband_tableau when the user has selected a polygon.
  virtual void add_polygon(int n,float const*,float const*);

  //: Called by vgui_rubberband_tableau when user has selected a rectangular box
  virtual void add_box(float,float,float,float);

  //: Called by vgui_rubberband_tableau whenever mouse motion is captured.
  //  This turns off the highlighting of geometry objects to eliminate
  //  flickering highlights while drawing temporary objects.
  void clear_highlight();
};

#endif // bgui_vtol2D_rubberband_client_h_
