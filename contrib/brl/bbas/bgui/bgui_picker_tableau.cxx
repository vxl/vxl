// This is brl/bbas/bgui/bgui_picker_tableau.cxx
#include "bgui_picker_tableau.h"
//:
// \file
// \author K.Y.McGaul
//  See bgui_picker_tableau.h for a description of this file.
//
// \verbatim
//  Modifications
//   K.Y.McGaul - 26-APR-2001 - Initial version.
// \endverbatim

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_projection_inspector.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>

bgui_picker_tableau::object_type bgui_picker_tableau::obj_type = none_enum;

//========================================================================
//: Constructor, takes a child tableau.
bgui_picker_tableau::bgui_picker_tableau(vgui_tableau_sptr const& t)
  :child_tab(this, t)
{
  r = 1.0f;
  g = 1.0f;
  b = 1.0f;
  w = 2.0f;
  use_event_ = false;
  pointx1 = pointy1 = pointx2 = pointy2 = 0;
  FIRSTPOINT = true;
  pointx = 0; pointy = 0;
  point_ret = true;
  anchor_x = 0;
  anchor_y = 0;
  //for polygon draw
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1 = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  gesture2 = vgui_event_condition(vgui_END, vgui_MODIFIER_NULL, true);
  last_x = 0;
  last_y = 0;
  active = false;
}

//========================================================================
//: Destructor.
bgui_picker_tableau::~bgui_picker_tableau()
{
}

//========================================================================
//: Gets a user selected point.
//  This function grabs the event loop and will not return until a
//  mouse down event occurs.
//  Returns true if this is done with the left mouse button, otherwise false.
//  The coordinates of the point are returned in the parameters.
bool bgui_picker_tableau::pick_point(float* x, float* y)
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

void bgui_picker_tableau::pick_box(float* x1, float* y1, float *x2, float* y2)
{
  obj_type = box_enum;
  picking_completed = false;
  point_ret = true;
  vgui::flush();  // handle any pending events before we grab the event loop.

  // Grab event loop until picking is completed:
  while (!picking_completed)
    next();

  *x1 = pointx1;
  *y1 = pointy1;
  *x2 = pointx2;
  *y2 = pointy2;

  //reset everything for the next pick
  FIRSTPOINT=true;
  post_redraw();
  obj_type = none_enum;
}//========================================================================

//: Draw a line to help the user pick it.
void bgui_picker_tableau::draw_line()
{
  if (!FIRSTPOINT)  // there is no point in drawing till we have a first point
    {
      glLineWidth(w);
      glColor3f(r,g,b);

      glBegin(GL_LINES);
      glVertex2f(pointx1, pointy1);
      glVertex2f(pointx2, pointy2);
      glEnd();
    }
}

//: Draw a box to help the user pick it.
void bgui_picker_tableau::draw_box()
{
  if (!FIRSTPOINT)  // there is no point in drawing till we have a first point
    {
      glLineWidth(w);
      glColor3f(r,g,b);

      glBegin(GL_LINE_LOOP);
      glVertex2f(pointx1, pointy1);
      glVertex2f(pointx2, pointy1);
      glVertex2f(pointx2, pointy2);
      glVertex2f(pointx1, pointy2);
      glEnd();
    }
}


//========================================================================
//: Draw a line to help the user pick it.
void bgui_picker_tableau::draw_anchor_line()
{
  glLineWidth(w);
  glColor3f(r,g,b);
  glBegin(GL_LINES);
  glVertex2f(anchor_x, anchor_y);
  glVertex2f(pointx1, pointy1);
  glEnd();
}

//========================================================================
//: Gets a user selected line.
//  This function grabs the event loop and will not return until two mouse
//  down events occur.
//  The parameters return the two points defining the line.
void bgui_picker_tableau::pick_line(float* x1, float* y1, float* x2, float* y2)
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
  post_overlay_redraw();
  obj_type = none_enum;
}

//========================================================================
//: Gets a user selected point (x, y)
//  This function grabs the event loop and will not return until a
//  mouse button down event occurs.
void bgui_picker_tableau::anchored_pick_point(const float anch_x,
                                              const float anch_y,
                                              float* x, float* y)
{
  anchor_x = anch_x;
  anchor_y = anch_y;

  obj_type = anchor_enum;
  picking_completed = false;
  vgui::flush();  // handle any pending events before we grab the event loop.
  // Grab event loop until picking is completed:
  while (!picking_completed)
    next();

  *x = pointx1;
  *y = pointy1;
  post_redraw();
  obj_type = none_enum;
}

void bgui_picker_tableau::pick_polygon(vsol_polygon_2d_sptr& poly)
{
  point_list.clear();
  obj_type = poly_enum;
  picking_completed = false;
  active = true;
  vgui::flush();  // handle any pending events before we grab the event loop.
  // Grab event loop until picking is completed:
  while (!picking_completed)
    next();
  if (point_list.size() >=3)
    poly =  new vsol_polygon_2d( point_list );
  obj_type = none_enum;
}

void bgui_picker_tableau::pick_polyline(vsol_polyline_2d_sptr& poly)
{
  point_list.clear();
  obj_type = polyline_enum;
  picking_completed = false;
  active = true;
  vgui::flush();  // handle any pending events before we grab the event loop.
  // Grab event loop until picking is completed:
  while (!picking_completed)
    next();
  if (point_list.size() >=2)
    poly =  new vsol_polyline_2d( point_list );
  obj_type = none_enum;
}

//========================================================================
// add max as argument to this method
bool bgui_picker_tableau::pick_point_set(std::vector< vsol_point_2d_sptr >& ps_list,
                                         unsigned max)
{
  obj_type = point_set_enum;
  picking_completed = false;
  vgui::flush();  // handle any pending events before we grab the event loop.

  // start with empty lists
  point_set_list.clear();
  ps_list.clear();

  // Grab event loop until picking is completed:
  while (!picking_completed)
    next();

  if (point_set_list.size() <= max)
    {
      // copy points into return vector ps_list
      ps_list.clear();
      for (unsigned i=0; i < point_set_list.size(); i++)
  ps_list.push_back(point_set_list[i]);
      obj_type = none_enum;
      return true;
    }
  else       // too many points picked?
    {
      ps_list.clear();          // error, return empty list
      std::cout << "Error, " << point_set_list.size()
         << " points picked (more than " << max << ")!!  Try again.\n";
      std::cout.flush();
      return false;
    }
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
bool bgui_picker_tableau::handle(const vgui_event& e)
{
  //USE BUTTON_UP TO INITIALIZE, COMPETITION FOR BUTTON_DOWN on other tableaux

  //ENDER/LEAVB events cause trouble when moving across grid tableau panes
  //image/viewer rendering is screwed up by this tableau issuing
  //DRAW_OVERLAY events to those tableaux
  if(e.type == vgui_ENTER || e.type == vgui_LEAVE)
    return false;
  // Pass events except DRAW_OVERLAY on down to the child tableaux:
  bool child_handled = false;
  if(e.type != vgui_DRAW_OVERLAY)
    child_handled = child_tab->handle(e);
#ifdef DEBUG
  if(e.type != vgui_MOTION)
    std::cout << "picker " << e << '\n' << std::flush;
#endif
  use_event_ = true;
  //---- Object type is point -----
  if (obj_type == point_enum)
    {
      if (e.type == vgui_BUTTON_DOWN)
  {
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx, pointy);

    if (e.button != vgui_LEFT)
      point_ret= false;
    picking_completed = true;
  }
      return true;
    }
  // ---- Object type is line ----
  if (obj_type == line_enum)
    {
      if (e.type == vgui_DRAW_OVERLAY)
  draw_line();
      else if (e.type == vgui_MOTION)
  {
    vgui_projection_inspector p_insp;
    if (!FIRSTPOINT){
      p_insp.window_to_image_coordinates(e.wx, e.wy, pointx2, pointy2);
      post_overlay_redraw();
    }
    else
      p_insp.window_to_image_coordinates(e.wx, e.wy, pointx1, pointy1);

  }
      else if (e.type == vgui_BUTTON_UP)
  {
    if (FIRSTPOINT)
      {
        vgui_projection_inspector p_insp;
        p_insp.window_to_image_coordinates(e.wx,e.wy, pointx1,pointy1);
        pointx2 = pointx1;
        pointy2 = pointy1;
        FIRSTPOINT=false;
      }
    else
      {
        picking_completed = true;
      }
  }
      return true;
    }

  // ---- Object type is box ----
  if (obj_type == box_enum)
    {
      if (e.type == vgui_DRAW_OVERLAY)
  draw_box();
      else if (e.type == vgui_MOTION)
  {
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx2, pointy2);
    post_overlay_redraw();
  }
      else if (e.type == vgui_BUTTON_DOWN)
  {
    if (FIRSTPOINT)
      {
        vgui_projection_inspector p_insp;
        p_insp.window_to_image_coordinates(e.wx,e.wy, pointx1,pointy1);
        pointx2 = pointx1;
        pointy2 = pointy1;
        FIRSTPOINT=false;
      }
    else
      {
        picking_completed = true;
      }
  }
      return true;
    }

  // ---- Object type is anchor line ----
  if (obj_type == anchor_enum)
    {
      if (e.type == vgui_DRAW_OVERLAY)
  draw_anchor_line();
      else if (e.type == vgui_MOTION)
  {
    vgui_projection_inspector p_insp;
    p_insp.window_to_image_coordinates(e.wx, e.wy, pointx1, pointy1);
    post_overlay_redraw();
  }
      else if (e.type == vgui_BUTTON_UP)
  picking_completed = true;
      return true;
    }

  // ---- Object type is polygon or polyline ----
  if ((obj_type == poly_enum) || (obj_type == polyline_enum))
    {
      if (active && e.type == vgui_OVERLAY_DRAW )
  {
    glLineWidth(w);
    glColor3f(r,g,b);

    if (obj_type == poly_enum)
      {
        glBegin(GL_LINE_LOOP);
        for (unsigned i=0; i<point_list.size(); ++i)
    glVertex2f(static_cast<float>(point_list[i]->x()), static_cast<float>(point_list[i]->y()));
        glVertex2f(last_x,last_y);
        glEnd();
        return true;
      }
    else if (obj_type == polyline_enum)
      {
        unsigned n = point_list.size();
        for (unsigned i=1; i<n; ++i){
    glBegin(GL_LINES);
    glVertex2f(static_cast<float>(point_list[i-1]->x()), static_cast<float>(point_list[i-1]->y()));
    glVertex2f(static_cast<float>(point_list[i]->x()), static_cast<float>(point_list[i]->y()));
    glEnd();
        }
        if (n > 0) {
    glBegin(GL_LINES);
    glVertex2f(last_x,last_y);
    glVertex2f(static_cast<float>(point_list[n-1]->x()), static_cast<float>(point_list[n-1]->y()));
    glEnd();
        }
        return true;
      }
  }

      float ix, iy;

      vgui_projection_inspector pi;
      pi.window_to_image_coordinates(e.wx,e.wy,ix, iy);

      if (active) {
  if (e.type == vgui_MOTION) {
    last_x = ix; last_y = iy;
    post_overlay_redraw();
  }
  else if ( gesture0(e) ) {
    point_list.push_back(vsol_point_2d_sptr( new vsol_point_2d( ix , iy )));
    last_x = ix; last_y = iy;
  }
  else if ( gesture1(e) || gesture2(e) ) {
    if (gesture1(e))
      point_list.push_back( vsol_point_2d_sptr( new vsol_point_2d(ix,iy) ));
    active = false;
    picking_completed = true;
    return true;
  }
      }
    }
  // ---- Object type is point_set_enum ----
  //  -- similar to polygon except that we don't need to draw anything between points --
  if (obj_type == point_set_enum)
    {
      if (e.type == vgui_OVERLAY_DRAW ) {
  unsigned n = point_set_list.size();
  glPointSize(3);
  glColor3f(r,g,b);
  glBegin(GL_POINTS);
  for (unsigned i=0; i<n; ++i){
    glVertex2f(static_cast<float>(point_set_list[i]->x()), static_cast<float>(point_set_list[i]->y()));
  }
  glEnd();
      }

      float ix, iy;
      vgui_projection_inspector().window_to_image_coordinates(e.wx,
                    e.wy,
                    ix, iy);

      // gesture0 = left mouse click, just add point to list
      if ( gesture0(e) ) {
  point_set_list.push_back(vsol_point_2d_sptr( new vsol_point_2d(ix,iy) ));
  post_overlay_redraw();
  return true;
#ifdef DEBUG
  std::cout << "Left click returned " << ix << ",  " << iy  << std::endl;
  std::cout.flush();
#endif
      }

      // gesture1 = shift left mouse click or gesture2 = END key press
      if ( gesture1(e)||gesture2(e) )
  {
    // if middle mouse, add point to list and end, if END key just end
    if (gesture1(e)) {
      point_set_list.push_back(vsol_point_2d_sptr(new vsol_point_2d(ix,iy)));
      post_overlay_redraw();
#ifdef DEBUG
      std::cout << "Shift left returned " << ix << ",  " << iy << std::endl;
      std::cout.flush();
#endif
    }

    // either way, end of picking
    active = false;
    picking_completed = true;
#ifdef DEBUG
    std::cout << "Detected either middle or END key\n";
    std::cout.flush();
#endif
    return true;
  }
    }
  return child_handled;
}

//========================================================================
//: Get next event in event loop
bool bgui_picker_tableau::next()
{
  use_event_ = false;
  while (!use_event_) {
    vgui::run_one_event();
  }
  return true;
}
