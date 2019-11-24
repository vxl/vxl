// This is oxl/xcv/xcv_picker_tableau.cxx
#include "xcv_picker_tableau.h"
//:
//  \file
// \author K.Y.McGaul
//  See xcv_picker_tableau.h for a description of this file.
//
// \verbatim
//  Modifications:
//    K.Y.McGaul 26-APR-2001   Initial version.
// \endverbatim

#include "vgui/vgui.h"
#include "vgui/vgui_gl.h"
#include "vgui/vgui_projection_inspector.h"

xcv_picker_tableau::object_type xcv_picker_tableau::obj_type = none_enum;

//========================================================================
//: Constructor, takes a child tableau.
xcv_picker_tableau::xcv_picker_tableau(vgui_tableau_sptr const& t)
  :child_tab(this, t)
{
  use_event_ = false;
  pointx1 = pointy1 = pointx2 = pointy2 = 0;
  FIRSTPOINT = true;
  pointx = 0; pointy = 0;
  point_ret = true;
}

//========================================================================
//: Destructor.
xcv_picker_tableau::~xcv_picker_tableau()
{
}

//========================================================================
//: Gets a user selected point.
//  This function grabs the event loop and will not return until a
//  mouse down event occurs.
//  Returns true if this is done with the left mouse button, otherwise false.
//  The coordinates of the point are returned in the parameters.
bool xcv_picker_tableau::pick_point(float* x, float* y)
{
  obj_type = point_enum;
  picking_completed = false;
  point_ret = true;
  vgui::flush();  // handle any pending events before we grab the event loop.

  // Grab event loop until picking is completed:
  while (!picking_completed)
    next();

  *x = pointx;
  *y = pointy;
  obj_type = none_enum;
  return point_ret;
}

//========================================================================
//: Draw a line to help the user pick it.
void xcv_picker_tableau::draw_line()
{
  if (!FIRSTPOINT)  // there is no point in drawing till we have a first point
  {
    glLineWidth(2);
    glColor3f(1,1,1);

    glBegin(GL_LINES);
    glVertex2f(pointx1, pointy1);
    glVertex2f(pointx2, pointy2);
    glEnd();
  }
}

//========================================================================
//: Gets a user selected line.
//  This function grabs the event loop and will not return until two mouse
//  down events occur.
//  The parameters return the two points defining the line.
void xcv_picker_tableau::pick_line(float* x1, float* y1, float* x2,
  float* y2)
{
  obj_type = line_enum;
  picking_completed = false;

  vgui::flush();  // handle any pending events before we grab the event loop.

  // Grab event loop until picking is completed:
  while (!picking_completed)
    next();

  *x1 = pointx1;
  *y1 = pointy1;
  *x2 = pointx2;
  *y2 = pointy2;

  // Reset everything ready for the next pick:
  FIRSTPOINT=true;
  post_redraw();
  obj_type = none_enum;
}

//========================================================================
//: Handles all events for this tableau.
//  We grab events in this way rather than using a vgui_event_server because
//  if we look at events outside the handle function then the gl state
//  associated with those events will have changed.  This means for a
//  draw_overlay event we would end up drawing into the wrong buffer.
//  For a mouse event we would not be able to get the position in the image
//  using the projection_inspector (if e.g. the image was zoomed) since all
//  the gl matrices would have been reset.
bool xcv_picker_tableau::handle(const vgui_event& event)
{
  // Pass events on down to the child tableaux:
  child_tab->handle(event);

  use_event_ = true;

  //---- Object type is point -----
  if (obj_type == point_enum)
  {
    if (event.type == vgui_BUTTON_DOWN)
    {
      vgui_projection_inspector p_insp;
      p_insp.window_to_image_coordinates(event.wx, event.wy, pointx, pointy);

      if (event.button != vgui_LEFT)
        point_ret= false;
      picking_completed = true;
    }
  }
  // ---- Object type is line ----
  if (obj_type == line_enum)
  {
    if (event.type == vgui_DRAW)
      draw_line();
    else if (event.type == vgui_MOTION)
    {
      vgui_projection_inspector p_insp;
      p_insp.window_to_image_coordinates(event.wx, event.wy, pointx2, pointy2);
      post_redraw();
    }
    else if (event.type == vgui_BUTTON_DOWN)
    {
      if (FIRSTPOINT)
      {
        vgui_projection_inspector p_insp;
        p_insp.window_to_image_coordinates(event.wx, event.wy,
          pointx1, pointy1);
        pointx2 = pointx1;
        pointy2 = pointy1;
        FIRSTPOINT=false;
      }
      else
      {
        picking_completed = true;
      }
    }
  }
  return true;
}

//========================================================================
//: Get next event in event loop
bool xcv_picker_tableau::next()
{
  use_event_ = false;
  while (!use_event_)
    vgui::run_one_event();
  return true;
}
