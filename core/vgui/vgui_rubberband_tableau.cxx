// This is core/vgui/vgui_rubberband_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author K.Y.McGaul
// \date   31-MAR-2000
// \brief  See vgui_rubberband_tableau.h for a description of this file.

#include "vgui_rubberband_tableau.h"

#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgl/vgl_clip.h>
#include <vnl/vnl_math.h>

#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_projection_inspector.h>

//static bool debug = false;
vgui_rubberband_tableau::object_type vgui_rubberband_tableau::obj_type = none_enum;

#ifdef __GNUC__ /* __FUNCTION__ is a GNU extension */
# define function_macro { vcl_cerr << __FUNCTION__ << " not yet implemented\n"; }
#else
# define function_macro { vcl_cerr << __FILE__ ":" << __LINE__ << " not yet implemented\n"; } 
#endif
void vgui_rubberband_client::add_point(float, float) function_macro
void vgui_rubberband_client::add_line(float,float,float,float) function_macro
void vgui_rubberband_client::add_infinite_line(float,float,float) function_macro
void vgui_rubberband_client::add_circle(float,float,float) function_macro
void vgui_rubberband_client::add_linestrip(int /*n*/,float const *,float const *) function_macro
void vgui_rubberband_client::add_polygon(int /*n*/,float const*,float const*) function_macro
void vgui_rubberband_client::add_box(float,float,float,float) function_macro
void vgui_rubberband_client::clear_highlight() function_macro
#undef function_macro


//---------------------------------------------------------------------------
//                                              vgui_rubberband_easy2D_client

vgui_rubberband_easy2D_client::
vgui_rubberband_easy2D_client(vgui_easy2D_tableau_sptr const& e)
  : easy(e)
{
}

void
vgui_rubberband_easy2D_client::
add_point(float x, float y)
{
  easy->add_point(x,y);
}

void
vgui_rubberband_easy2D_client::
add_line(float x0, float y0, float x1, float y1)
{
  easy->add_line(x0, y0, x1, y1);
}

void
vgui_rubberband_easy2D_client::
add_infinite_line(float a, float b, float c)
{
  easy->add_infinite_line(a, b, c);
}

void
vgui_rubberband_easy2D_client::
add_circle(float x, float y, float r)
{
  easy->add_circle(x, y, r);
}

void
vgui_rubberband_easy2D_client::
add_linestrip(int n, float const* x, float const* y)
{
  easy->add_linestrip(n, x, y);
}

void
vgui_rubberband_easy2D_client::
add_polygon(int n, float const* x, float const* y)
{
  easy->add_polygon(n, x, y);
}


void
vgui_rubberband_easy2D_client::
add_box(float x0, float y0, float x1, float y1)
{
  float x[4] = {x0, x1, x1, x0};
  float y[4] = {y0, y0, y1, y1};
  this->add_polygon( 4, x, y );
}


void
vgui_rubberband_easy2D_client::
clear_highlight()
{
  easy->highlight(0);
}

//---------------------------------------------------------------------------
//                                                    vgui_rubberband_tableau

void vgui_rubberband_tableau::init(vgui_rubberband_client* the_client)
{
  client_ = the_client;
  active = false;
  obj_type = none_enum;
#ifdef HAS_MFC
  // Until somebody implements overlays for mfc, this
  // is the default for windows.
  use_overlays = true;
#else
  // This is the default on non-windows platforms.
  // Please keep it that way or document clearly why
  // it must be changed. It is the default because
  // Mesa (or glx) is too slow otherwise.
  use_overlays = true;
#endif
}

vgui_rubberband_tableau::vgui_rubberband_tableau(vgui_rubberband_client *client)
{
  init(client);
}

#if 0
vgui_rubberband_tableau::vgui_rubberband_tableau(vgui_easy2D_tableau_sptr const& easy)
{
  init(new vgui_rubberband_tableau_easy2D_client(easy));
}
#endif

void vgui_rubberband_tableau::set_client(vgui_rubberband_client *client)
{
  client_ = client;
}

void vgui_rubberband_tableau::rubberband_point()
{
  obj_type = point_enum;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true ); // press
  gesture1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, false); // release
}

void vgui_rubberband_tableau::rubberband_line()
{
  obj_type = line_enum;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

void vgui_rubberband_tableau::rubberband_infinite_line()
{
  obj_type = infinite_line_enum;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

void vgui_rubberband_tableau::rubberband_circle()
{
  obj_type = circle_enum;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  gesture1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
}

void vgui_rubberband_tableau::rubberband_polygon()
{
  obj_type = polygon_enum;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true); // begin
  gesture1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true); // new point
  // vgui_RIGHT will conflict with popup menus in many cases. add a
  // gesture3 field if you really need to respond to both.
  gesture2 = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true); // last point
}

void vgui_rubberband_tableau::rubberband_linestrip()
{
  obj_type = linestrip_enum;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true); // begin
  gesture1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true); // new point
  // see above comment.
  gesture2 = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true); // last point
}

void vgui_rubberband_tableau::rubberband_box()
{
  obj_type = box_enum;
  gesture0 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true); // begin
  gesture1 = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true); // end
}

void vgui_rubberband_tableau::rubberband_none()
{
  active = false;
  obj_type = none_enum;
}

//: Render the given point.
void vgui_rubberband_tableau::draw_point(float x, float y)
{
  glColor3f(1,1,1);
  glBegin(GL_POINTS);
  // vcl_cerr << "vgui_rubberband_tableau::draw_point(" << x << "," << y << ")\n";
  glVertex2f(x,y);
  glEnd();
}

//: Render the finite line between the given points.
void vgui_rubberband_tableau::draw_line(float x0, float y0, float x1, float y1)
{
  glLineWidth(1);
  glColor3f(1,1,1);

  glBegin(GL_LINES);
  glVertex2f(x0, y0);
  glVertex2f(x1, y1);
  glEnd();
}

//: Render the infinite line ax + by + c = 0.
void vgui_rubberband_tableau::draw_infinite_line(float a, float b, float c)
{
  // Compute the bounding box.
  // This will be the viewport but in the coordinates of the image.
  vgui_projection_inspector pi;
  float x0, y0, x1, y1;
  pi.image_viewport(x0,y0,x1,y1);

  float x3, y3;
  float x4, y4;
  bool f = vgl_clip_line_to_box(a,b,c, x0, y0, x1, y1, x3, y3, x4,y4);
  if (!f)
    return; // no intersection

  glBegin(GL_LINES);
  glVertex2f(x3,y3);
  glVertex2f(x4,y4);
  glEnd();
}

//: Render the rubber circle.
void vgui_rubberband_tableau::draw_circle(float x, float y, float r)
{
  glLineWidth(1);
  glColor3f(1,1,1);
  glBegin(GL_LINE_LOOP);


  for (int i=0;i<100;i++) {
    double angle = (2*vnl_math::pi/100.0)*i;
    glVertex2d(vcl_cos(angle)*r + x, vcl_sin(angle)*r + y);
  }
  glEnd();
}

//: Render the rubber polygon.
void vgui_rubberband_tableau::draw_polygon(float px, float py)
{
  unsigned n=x_coords.size(); assert(n==y_coords.size());
  if (n==0)
    return;

  vgui_matrix_state M; M.save();

  glLineWidth(1);
  glColor3f(1,1,1);
  if (n==1)
    glBegin(GL_LINES);
  else
    glBegin(GL_LINE_LOOP);

  for (unsigned i=0; i<n; ++i)
    glVertex2f(x_coords[i], y_coords[i]);
  glVertex2f(px, py);
  glEnd();
}

//: Render rubber box
void vgui_rubberband_tableau::draw_box(float x0,float y0,float x1,float y1)
{
  float sx = x0>x1 ? x1:x0;
  float sy = y0>y1 ? y1:y0;
  float ex = x0>x1 ? x0:x1;
  float ey = y0>y1 ? y0:y1;

  glLineWidth(1);
  glColor3f(1,1,1);
  glBegin(GL_LINE_LOOP);

  glVertex2f(sx,sy);
  glVertex2f(ex,sy);
  glVertex2f(ex,ey);
  glVertex2f(sx,ey);

  glEnd();
}

//: Render the rubber line strip
void vgui_rubberband_tableau::draw_linestrip(float px,float py)
{
  unsigned n=x_coords.size(); assert(n==y_coords.size());
  if (n==0)
    return;

  vgui_matrix_state M; M.save();

  glLineWidth(1);
  glColor3f(1,1,1);
  glBegin(GL_LINE_STRIP);
  for (unsigned i=0; i<n; ++i)
    glVertex2f(x_coords[i], y_coords[i]);
  glVertex2f(px,py);
  glEnd();
}


bool vgui_rubberband_tableau::handle_point(vgui_event const &e, float ix, float iy)
{
  if ((use_overlays && e.type == vgui_DRAW_OVERLAY) || (!use_overlays && e.type == vgui_DRAW))
  {
    draw_point(lastx, lasty);
    return true;
  }
  if (gesture1(e))
  {
    // Point is completed, add to client:
    client_->add_point(ix,iy);
    active = false;
    obj_type = none_enum;
    post_redraw();
    return true;
  }
  return false;
}

bool vgui_rubberband_tableau::handle_line(vgui_event const &e, float ix, float iy)
{
  if ((use_overlays && e.type == vgui_DRAW_OVERLAY) || (!use_overlays && e.type == vgui_DRAW))
  {
    draw_line(x_coords[0], y_coords[0], lastx, lasty);
    return true;
  }
  if (gesture1(e))
  {
    // Line is completed, add to client
    client_->add_line(x_coords[0],y_coords[0],ix,iy);
    active = false;
    obj_type = none_enum;
    post_redraw();
    return true;
  }
  return false;
}

bool vgui_rubberband_tableau::handle_infinite_line(vgui_event const &e, float ix, float iy)
{
  if ((use_overlays && e.type == vgui_DRAW_OVERLAY) || (!use_overlays && e.type == vgui_DRAW))
  {
    draw_infinite_line(-lastx+x_coords[0],y_coords[0] - lasty,
                       -(-lastx+x_coords[0])*lastx-lasty*(y_coords[0]-lasty));
    return true;
  }
  if (gesture1(e))
  {
    // Infinite line is completed, add to rubberband_client
    client_->add_infinite_line(-ix+x_coords[0],y_coords[0] - iy,
                               -(-ix+x_coords[0])*lastx-iy*(y_coords[0]-lasty));
    active = false;
    obj_type = none_enum;
    post_redraw();
    return true;
  }
  return false; // ?
}

bool vgui_rubberband_tableau::handle_circle(vgui_event const &e, float ix, float iy)
{
  if ((use_overlays && e.type == vgui_DRAW_OVERLAY) || (!use_overlays && e.type == vgui_DRAW))
  {
    // hypot(x_coords[0] - lastx, y_coords[0]-lasty);
    float dx = x_coords[0] - lastx;
    float dy = y_coords[0] - lasty;
    float radi = vcl_sqrt(dx*dx + dy*dy);
    draw_circle(x_coords[0], y_coords[0], radi);
    return true;
  }
  if (gesture1(e))
  {
    // Circle is completed, add to the client:
    float dx = x_coords[0] - ix;
    float dy = y_coords[0] - iy;
    float radi = vcl_sqrt(dx*dx + dy*dy);
    client_->add_circle(x_coords[0],y_coords[0],radi);
    active = false;
    obj_type = none_enum;
    post_redraw();
    return true;
  }
  return false;
}

bool vgui_rubberband_tableau::handle_polygon(vgui_event const &e, float ix, float iy)
{
  unsigned n = x_coords.size();
  assert(n == y_coords.size());
  assert(n>0);

  if ((use_overlays && e.type == vgui_DRAW_OVERLAY) || (!use_overlays && e.type == vgui_DRAW))
  {
    draw_polygon(lastx, lasty);
    return true;
  }
  if (gesture1(e))
  {
    // drop a new point
    x_coords.push_back(ix);
    y_coords.push_back(iy);
    return true;
  }

  if (gesture2(e))
  {
    // last selected point - add to the list of coordinates:
    x_coords.push_back(ix);
    y_coords.push_back(iy);
    n++;

    // Add the first point in as also the last point:
    x_coords.push_back(x_coords[0]);
    y_coords.push_back(y_coords[0]);
    n++;

    client_->add_polygon(n, &x_coords[0], &y_coords[0]); //vector<>::iterator

    post_redraw();
    active = false;
    obj_type = none_enum;
    return true;
  }
  return false;
}

bool vgui_rubberband_tableau::handle_box(vgui_event const &e, float ix,float iy)
{
  if ((use_overlays && e.type == vgui_DRAW_OVERLAY) || (!use_overlays && e.type == vgui_DRAW))
  {
    draw_box(x_coords[0], y_coords[0], lastx, lasty);
    return true;
  }
  if (gesture1(e))
  {
    client_->add_box(x_coords[0],y_coords[0],ix,iy);
    active = false;
    obj_type = none_enum;
    post_redraw();
    return true;
  }
  return false;
}

bool vgui_rubberband_tableau::handle_linestrip(vgui_event const &e, float ix, float iy)
{
  unsigned n = x_coords.size();
  assert(n == y_coords.size());
  assert(n>0);

  if ((use_overlays && e.type == vgui_DRAW_OVERLAY) || (!use_overlays && e.type == vgui_DRAW))
  {
    draw_linestrip(lastx, lasty);
    return true;
  }

  if (gesture1(e))
  {
    // drop a new point
    x_coords.push_back(ix);
    y_coords.push_back(iy);
    return true;
  }

  if (gesture2(e))
  {
    client_->add_linestrip(n,&x_coords[0],&y_coords[0]); //vector<>::iterator
    post_redraw();
    active = false;
    obj_type = none_enum;
    return true;
  }
  return false;
}

bool vgui_rubberband_tableau::handle(vgui_event const &e)
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);

  if (!active) {
    if (obj_type != none_enum && gesture0(e))
    {
      active = true;
      if (use_overlays) post_overlay_redraw(); else post_redraw();
      client_->clear_highlight();

      lastx = ix;
      lasty = iy;
      x_coords.clear();
      y_coords.clear();
      x_coords.push_back(ix);
      y_coords.push_back(iy);

      return true;
    }
    else
      return false;
  }

  // active :
  if (e.type == vgui_MOTION){
    lastx = ix;
    lasty = iy;
    if (use_overlays) post_overlay_redraw(); else post_redraw();
    client_->clear_highlight();

    return true;
  }

  if (obj_type == point_enum)
    return handle_point(e, ix, iy);

  if (obj_type == line_enum)
    return handle_line(e, ix, iy);

  if (obj_type == infinite_line_enum)
    return handle_infinite_line(e, ix, iy);

  if (obj_type == circle_enum)
    return handle_circle(e, ix, iy);

  if (obj_type == polygon_enum)
    return handle_polygon(e, ix, iy);

  if (obj_type == linestrip_enum)
    return handle_linestrip(e,ix,iy);

  if (obj_type == box_enum)
    return handle_box(e,ix,iy);

  return false;
}
