#include "bgui_vsol2D_tableau.h"
//:
// \file
#include <bgui/bgui_vsol_soview2D.h>
#include <vgui/vgui.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_style.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_triangle_2d.h>
#include <vsol/vsol_group_2d.h>

#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>


bgui_vsol2D_tableau::bgui_vsol2D_tableau(const char* n) :
  vgui_easy2D_tableau(n) { this->init(); }

bgui_vsol2D_tableau::bgui_vsol2D_tableau(vgui_image_tableau_sptr const& it,
                                         const char* n) :
  vgui_easy2D_tableau(it, n) { this->init(); }

bgui_vsol2D_tableau::bgui_vsol2D_tableau(vgui_tableau_sptr const& t,
                                         const char* n) :
  vgui_easy2D_tableau(t, n) { this->init(); }

bgui_vsol2D_tableau::~bgui_vsol2D_tableau()
{
}

void bgui_vsol2D_tableau::init()
{
  old_id_ = 0;
  highlight_ = true;

  //set highlight display style parameters
  highlight_style_ = new bgui_style(0.0f, 0.0f, 1.0f, 5.0f, 5.0f);

  //define default soview styles
  //these can be overridden by later set_*_syle commands prior to drawing.
  //
   this->set_vsol_point_2d_style(0.0f, 1.0f, 0.0f, 3.0f);
   this->set_vsol_line_2d_style(0.8f, 0.2f, 0.9f, 3.0f);
   this->set_vsol_polyline_2d_style(0.8f, 0.2f, 0.9f, 3.0f);
   this->set_digital_curve_style(0.8f, 0.0f, 0.8f, 3.0f);
   this->set_dotted_digital_curve_style(0.8f, 0.0f, 0.8f, 3.0f, 3.0f);
}

//:
// Provide roaming highlighting for soviews in the tableau.
// As the mouse moves the soview closest to the mouse is
// changed to the highlighted style.

// vgui_displaylist2D_tableau::motion(..) has a mechanism for
// highlighting the nearest object but it doesn't work.

bool bgui_vsol2D_tableau::handle(vgui_event const &e)
{
  if (e.type == vgui_MOTION && highlight_)
    {
      //retrive the previously highlighted soview and
      //restore it to its default style

      vgui_soview* old_so = vgui_soview::id_to_object(old_id_);
      if (old_so)
        {
          bgui_style_sptr default_sty = style_map_[old_so->type_name()];
          bgui_style* bs = (bgui_style*)default_sty.ptr();
          vgui_style* s = (vgui_style*)bs;
          old_so->set_style(s);
        }

      //get the soview that is closest to the mouse
      vgui_soview2D* high_so = (vgui_soview2D*)get_highlighted_soview();
      
      if (high_so && high_so->get_style())
        {
          //replace the old soview with the currently closest view
          int id = high_so->get_id();
          old_id_ = id;
          //set soview style to the highlight color and weight
          bgui_style* bsh = (bgui_style*)highlight_style_.ptr();
          vgui_style* sh = (vgui_style*)bsh;
          high_so->set_style(sh);
          this->post_redraw();
        }
    }
  // We aren't interested in other events so pass them to the base class.
  return vgui_easy2D_tableau::handle(e);
}

bgui_vsol_soview2D_point*
bgui_vsol2D_tableau::add_vsol_point_2d(vsol_point_2d_sptr const& p)
{
  bgui_vsol_soview2D_point* obj =
    new bgui_vsol_soview2D_point();
  obj->x = p->x();
  obj->y = p->y();
  add(obj);
   bgui_style_sptr s = style_map_[obj->type_name()];
   obj->set_style(s.ptr());
  return obj;
}

bgui_vsol_soview2D_line_seg*
bgui_vsol2D_tableau::add_vsol_line_2d(vsol_line_2d_sptr const& line)
{
  bgui_vsol_soview2D_line_seg* obj =
    new bgui_vsol_soview2D_line_seg(line);
  add(obj);
  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  obj->set_style(sty.ptr());
  return obj;
}

bgui_vsol_soview2D_polyline* 
bgui_vsol2D_tableau::add_vsol_polyline_2d(vsol_polyline_2d_sptr const& pline)
{
  bgui_vsol_soview2D_polyline* obj =
    new bgui_vsol_soview2D_polyline(pline);
  add(obj);
  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  obj->set_style(sty.ptr());
  return obj;
}

bgui_vsol_soview2D_digital_curve*
bgui_vsol2D_tableau::add_digital_curve(vdgl_digital_curve_sptr const& dc)
{
  bgui_vsol_soview2D_digital_curve* obj =
    new bgui_vsol_soview2D_digital_curve(dc);

  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);

  return obj;
}

bgui_vsol_soview2D_dotted_digital_curve*
bgui_vsol2D_tableau::add_dotted_digital_curve(vdgl_digital_curve_sptr const& dc)
{
  bgui_vsol_soview2D_dotted_digital_curve* obj =
    new bgui_vsol_soview2D_dotted_digital_curve(dc);
  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  return obj;
}

void bgui_vsol2D_tableau::
add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos)
{
  for (vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator sit = sos.begin();
       sit != sos.end(); sit++)
    {
      add_spatial_object( (*sit) );
    }
}

void bgui_vsol2D_tableau::
add_spatial_object(vsol_spatial_object_2d_sptr const& sos)
{
  if (sos->cast_to_point()) {
    {
      vsol_point_2d_sptr p = sos->cast_to_point();
      this->add_vsol_point_2d(p);
    }
  } 

  if (sos->cast_to_curve()) {
    if (sos->cast_to_curve()->cast_to_digital_curve())
      {
        vdgl_digital_curve_sptr dc =
          sos->cast_to_curve()->cast_to_digital_curve();
        this->add_digital_curve(dc);
      }
    if (sos->cast_to_curve()->cast_to_line_2d())
      {
        vsol_line_2d_sptr line =
          sos->cast_to_curve()->cast_to_line_2d();
        this->add_vsol_line_2d(line);
     }
    if (sos->cast_to_curve()->cast_to_polyline_2d())
      {
        vsol_polyline_2d_sptr pline =
          sos->cast_to_curve()->cast_to_polyline_2d();
        this->add_vsol_polyline_2d(pline);
     }
  }
  return;
}

void bgui_vsol2D_tableau::clear_all()
{
  bool temp = highlight_;
  highlight_ = false;//in case of event interrupts during the clear
  vgui_easy2D_tableau::clear();
  old_id_ = 0;
  highlight_ = temp;
  this->post_redraw();
}

void bgui_vsol2D_tableau::set_vsol_spatial_object_2d_style(vsol_spatial_object_2d_sptr sos,
                                                           const float r, const float g, const float b,
                                                           const float line_width,
                                                           const float point_radius)
{
  if (sos->cast_to_point()) {
    set_vsol_point_2d_style(r,g,b, point_radius);
  } else if (sos->cast_to_curve()) {
    set_digital_curve_style(r,g,b, line_width);
  }
}

void bgui_vsol2D_tableau::set_vsol_point_2d_style(const float r, const float g,
                                                  const float b,
                                                  const float point_radius)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, point_radius, 0.0f);
  bgui_vsol_soview2D_point p;
  style_map_[p.type_name()]=sty;
}

void bgui_vsol2D_tableau::set_vsol_line_2d_style(const float r, const float g,
                                                 const float b,
                                                 const float line_width)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vsol_soview2D_line_seg seg;
  style_map_[seg.type_name()]=sty;
}

void bgui_vsol2D_tableau::set_vsol_polyline_2d_style(const float r, const float g, 
					             const float b, const float line_width)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vsol_soview2D_polyline pline;
  style_map_[pline.type_name()]=sty;
}

void bgui_vsol2D_tableau::set_digital_curve_style(const float r, const float g,
                                                  const float b,
                                                  const float line_width)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vsol_soview2D_digital_curve dc;
  style_map_[dc.type_name()]=sty;
}

void bgui_vsol2D_tableau::set_dotted_digital_curve_style(const float r, const float g,
                                                         const float b,
                                                         const float line_width, const float point_radius)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, point_radius, line_width);
  bgui_vsol_soview2D_dotted_digital_curve dc;
  style_map_[dc.type_name()]=sty;
}
