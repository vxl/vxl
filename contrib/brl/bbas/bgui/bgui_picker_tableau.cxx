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
//   J. Green - 01 Oct 2007 - Added pick point set + some extra comments
// \endverbatim

#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_projection_inspector.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

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
  gesture1 = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);
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
  while (picking_completed == false)
    next();

  *x = pointx;
  *y = pointy;
  obj_type = none_enum;
  return point_ret;
}

//========================================================================
void bgui_picker_tableau::pick_box(float* x1, float* y1, float *x2, float* y2)
{
  obj_type = box_enum;
  picking_completed = false;
  point_ret = true;
  vgui::flush();  // handle any pending events before we grab the event loop.

  // Grab event loop until picking is completed:
  while (picking_completed == false)
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

//========================================================================
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
  while (picking_completed == false)
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
  while (picking_completed == false)
    next();

  *x = pointx1;
  *y = pointy1;
  post_redraw();
  obj_type = none_enum;
}

//========================================================================
void bgui_picker_tableau::pick_polygon(vsol_polygon_2d_sptr& poly)
{
  point_list.clear();
  obj_type = poly_enum;
  picking_completed = false;
  vgui::flush();  // handle any pending events before we grab the event loop.
  // Grab event loop until picking is completed:
  while (picking_completed == false)
    next();
  if(point_list.size() >=3)
    poly =  new vsol_polygon_2d( point_list );
  obj_type = none_enum;   
}

//========================================================================
// add max as argument to this method
bool bgui_picker_tableau::pick_point_set(vcl_vector< vsol_point_2d_sptr >& ps_list, int max)
{
  obj_type = point_set_enum;
  picking_completed = false;
  vgui::flush();  // handle any pending events before we grab the event loop.

  // start with empty lists
  point_set_list.clear();
  ps_list.clear();

  // Grab event loop until picking is completed:
  while (picking_completed == false) {
      next();
  }

  if(point_set_list.size() <= max)
  {
	  // copy points into return vector ps_list
	  ps_list.clear();
	  for (int i=0; i < point_set_list.size(); i++)
			  ps_list.push_back(point_set_list[i]);
	  obj_type = none_enum;
	  return true;
  }
  else 			// too many points picked?
  {
	  ps_list.clear();					// error, return empty list
	  vcl_cout << "Error, " << point_set_list.size() 
	  		   << " points picked (more than " << max << ")!!  Try again." << vcl_endl;
	  vcl_cout.flush();
      return false;
  }
}

void bgui_picker_tableau::pick_polyline(vsol_polyline_2d_sptr& poly)
{
  point_list.clear();
  obj_type = polyline_enum;
  picking_completed = false;
  active = true;
  vgui::flush();  // handle any pending events before we grab the event loop.
  // Grab event loop until picking is completed:
  while (picking_completed == false)
    next();
  if(point_list.size() >=2)
    poly =  new vsol_polyline_2d( point_list );
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

bool bgui_picker_tableau::handle(const vgui_event& event)
{
  //USE BUTTON_UP TO INITIALIZE, COMPETITION FOR BUTTON_DOWN on other tableaux
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
      return true;
    }
  // ---- Object type is line ----
  if (obj_type == line_enum)
    {
      if (event.type == vgui_DRAW_OVERLAY)
        draw_line();
      else if (event.type == vgui_MOTION)
        {
          vgui_projection_inspector p_insp;
          if(!FIRSTPOINT)
            p_insp.window_to_image_coordinates(event.wx, event.wy, pointx2, pointy2);
          else
            p_insp.window_to_image_coordinates(event.wx, event.wy, pointx1, pointy1);
          post_overlay_redraw();
        }
      else if (event.type == vgui_BUTTON_UP)
        {
          if (FIRSTPOINT)
            {
              vgui_projection_inspector p_insp;
              p_insp.window_to_image_coordinates(event.wx,event.wy, pointx1,pointy1);
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
      if (event.type == vgui_DRAW_OVERLAY)
        draw_box();
      else if (event.type == vgui_MOTION)
        {
          vgui_projection_inspector p_insp;
          p_insp.window_to_image_coordinates(event.wx, event.wy, pointx2, pointy2);
          post_overlay_redraw();
        }
      else if (event.type == vgui_BUTTON_DOWN)
        {
          if (FIRSTPOINT)
            {
              vgui_projection_inspector p_insp;
              p_insp.window_to_image_coordinates(event.wx,event.wy, pointx1,pointy1);
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
      if (event.type == vgui_DRAW_OVERLAY)
        draw_anchor_line();
      else if (event.type == vgui_MOTION)
        {
          vgui_projection_inspector p_insp;
          p_insp.window_to_image_coordinates(event.wx, event.wy, pointx1, pointy1);
          post_overlay_redraw();
        }
      else if (event.type == vgui_BUTTON_UP)
        picking_completed = true;
      return true;
    }

  // ---- Object type is polygon ----
  if (obj_type == poly_enum)
    {
      float ix, iy;
      vgui_projection_inspector().window_to_image_coordinates(event.wx,
                                                              event.wy,
                                                              ix, iy);

      if(active&&event.type == vgui_MOTION)
        {
          last_x = ix; last_y = iy;
          post_overlay_redraw();
        }
      if( !active && event.type == vgui_BUTTON_UP ) {
        active = true;
        point_list.push_back( vsol_point_2d_sptr(new vsol_point_2d( ix , iy)));
        return true;
      } 
      else if( active ) {

        if( gesture0(event) ) {
          point_list.push_back(vsol_point_2d_sptr( new vsol_point_2d( ix , iy )));
        }

        if( event.type == vgui_OVERLAY_DRAW ) {

          glLineWidth(w);
          glColor3f(r,g,b);

          glBegin(GL_LINE_LOOP);
          for (unsigned i=0; i<point_list.size(); ++i)
            glVertex2f(point_list[i]->x(), point_list[i]->y() );
          glVertex2f(last_x,last_y);
          glEnd();
        }

        if( gesture1(event)||gesture2(event) ) {
          if(gesture1(event))
            point_list.push_back( vsol_point_2d_sptr( new vsol_point_2d( ix , iy ) ) );
     
          active = false;
          picking_completed = true;
        }
      }
      return true;
    }

  // ---- Object type is point_set_enum ----
  //  -- similar to polygon except that we don't need to draw anything between points --
  if (obj_type == point_set_enum)
    {
      float ix, iy;
      vgui_projection_inspector().window_to_image_coordinates(event.wx,
                                                              event.wy,
                                                              ix, iy);

		// gesture0 = left mouse click, just add point to list
        if( gesture0(event) ) {
          point_set_list.push_back(vsol_point_2d_sptr( new vsol_point_2d( ix, iy )));
		  vcl_cout << "Left click returned " << ix << ",  " << iy  << vcl_endl;
		  vcl_cout.flush();
        }

		// gesture1 = middle mouse click or gesture2 = END key press
        if( gesture1(event)||gesture2(event) ) {

		  // if middle mouse, add point to list and end, if END key just end
		  if(gesture1(event)) {
            point_set_list.push_back( vsol_point_2d_sptr( new vsol_point_2d( ix, iy )));
		    vcl_cout << "Middle click returned " << ix << ",  " << iy << vcl_endl;
		    vcl_cout.flush();
		  }
		  
		  // either way, end of picking
          active = false;
          picking_completed = true;
		  vcl_cout << "Detected either middle or END key" << vcl_endl;
		  vcl_cout.flush();
        }
      }
      return true;


}

//========================================================================
//: Get next event in event loop
bool bgui_picker_tableau::next()
{
  use_event_ = false;
  while (!use_event_)
    vgui::run_one_event();
  return true;
}
