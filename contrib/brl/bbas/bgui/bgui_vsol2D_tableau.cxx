#include "bgui_vsol2D_tableau.h"
//:
// \file
#include <bgui/bgui_vsol_soview2D.h>
#include <vgui/vgui.h>
#include <vgui/vgui_style.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vcl_cassert.h>

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
  //define default soview styles
  //these can be overridden by later set_*_syle commands prior to drawing.
  //
  point_style_                = vgui_style::new_style(0.0f, 1.0f, 0.0f, 3.0f, 1.0f);
  line_style_                 = vgui_style::new_style(0.8f, 0.2f, 0.9f, 1.0f, 3.0f);
  polyline_style_             = vgui_style::new_style(0.8f, 0.2f, 0.9f, 1.0f, 3.0f);
  digital_curve_style_        = vgui_style::new_style(0.8f, 0.0f, 0.8f, 1.0f, 3.0f);
  dotted_digital_curve_style_ = vgui_style::new_style(0.8f, 0.0f, 0.8f, 3.0f, 3.0f);
  edgel_curve_style_          = vgui_style::new_style(0.0f, 0.5f, 0.8f, 1.0f, 3.0f);
  dotted_edgel_curve_style_   = vgui_style::new_style(0.0f, 0.5f, 0.8f, 3.0f, 3.0f);
}


bgui_vsol_soview2D_point*
bgui_vsol2D_tableau::add_vsol_point_2d(vsol_point_2d_sptr const& p,
                                              const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_point* obj =
      new bgui_vsol_soview2D_point(p);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( point_style_ );
  return obj;
}


bgui_vsol_soview2D_line_seg*
bgui_vsol2D_tableau::add_vsol_line_2d(vsol_line_2d_sptr const& line,
                                             const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_line_seg* obj =
      new bgui_vsol_soview2D_line_seg(line);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( line_style_ );
  return obj;
}


bgui_vsol_soview2D_polyline*
bgui_vsol2D_tableau::add_vsol_polyline_2d(vsol_polyline_2d_sptr const& pline,
                                                 const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_polyline* obj =
      new bgui_vsol_soview2D_polyline(pline);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( polyline_style_ );
  return obj;
}


bgui_vsol_soview2D_polygon*
bgui_vsol2D_tableau::add_vsol_polygon_2d(vsol_polygon_2d_sptr const& pline,
                                                const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_polygon* obj =
      new bgui_vsol_soview2D_polygon(pline);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( polyline_style_ );
  return obj;
}


bgui_vsol_soview2D_digital_curve*
bgui_vsol2D_tableau::add_digital_curve(vsol_digital_curve_2d_sptr const& dc,
                                       const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_digital_curve* obj =
    new bgui_vsol_soview2D_digital_curve(dc);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( digital_curve_style_ );
  return obj;
}


bgui_vsol_soview2D_digital_curve*
bgui_vsol2D_tableau::add_dotted_digital_curve(vsol_digital_curve_2d_sptr const& dc,
                                              const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_digital_curve* obj =
      new bgui_vsol_soview2D_digital_curve(dc, true);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( dotted_digital_curve_style_ );
  return obj;
}


bgui_vsol_soview2D_edgel_curve*
bgui_vsol2D_tableau::add_edgel_curve(vdgl_digital_curve_sptr const& dc,
                                              const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_edgel_curve* obj =
    new bgui_vsol_soview2D_edgel_curve(dc);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( edgel_curve_style_ );
  return obj;
}


bgui_vsol_soview2D_edgel_curve*
bgui_vsol2D_tableau::add_dotted_edgel_curve(vdgl_digital_curve_sptr const& dc,
                                            const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_edgel_curve* obj =
      new bgui_vsol_soview2D_edgel_curve(dc, true);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( dotted_edgel_curve_style_ );
  return obj;
}


void bgui_vsol2D_tableau::
add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos,
                    const vgui_style_sptr& style)
{
  for (vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator sit = sos.begin();
       sit != sos.end(); sit++)
  {
    add_spatial_object( (*sit) , style );
  }
}


void bgui_vsol2D_tableau::
add_spatial_object(vsol_spatial_object_2d_sptr const& sos,
                   const vgui_style_sptr& style)
{
  if (sos->cast_to_point()) {
  {
    vsol_point_2d_sptr p = sos->cast_to_point();
    this->add_vsol_point_2d(p , style );
  }
  }

  if (sos->cast_to_curve()) {
    if (sos->cast_to_curve()->cast_to_digital_curve())
    {
      vsol_digital_curve_2d_sptr dc =
        sos->cast_to_curve()->cast_to_digital_curve();
      this->add_digital_curve(dc , style);
    }
    else if (sos->cast_to_curve()->cast_to_vdgl_digital_curve())
    {
      vdgl_digital_curve_sptr dc =
        sos->cast_to_curve()->cast_to_vdgl_digital_curve();
      this->add_edgel_curve(dc , style);
    }
    else if (sos->cast_to_curve()->cast_to_line())
    {
      vsol_line_2d_sptr line =
        sos->cast_to_curve()->cast_to_line();
      this->add_vsol_line_2d(line, style);
    }
    else if (sos->cast_to_curve()->cast_to_polyline())
    {
      vsol_polyline_2d_sptr pline =
        sos->cast_to_curve()->cast_to_polyline();
      this->add_vsol_polyline_2d(pline , style);
    }
    else
      assert(!"unknown curve type in bgui_vsol2D_tableau::add_spatial_object()");
  }
  else if (sos->cast_to_region()) {
    if (sos->cast_to_region()->cast_to_polygon())
    {
      vsol_polygon_2d_sptr pline =
        sos->cast_to_region()->cast_to_polygon();
      this->add_vsol_polygon_2d(pline, style);
    }
    else
      assert(!"unknown region type in bgui_vsol2D_tableau::add_spatial_object()");
  }
  else
    assert(!"unknown spatial object type in bgui_vsol2D_tableau::add_spatial_object()");
  return;
}


void bgui_vsol2D_tableau::set_vsol_spatial_object_2d_style(vsol_spatial_object_2d_sptr sos,
                                                           const vgui_style_sptr& style)
{
  if (sos->cast_to_point()) {
    set_vsol_point_2d_style(style);
  } else if (sos->cast_to_curve()) {
    set_digital_curve_style(style);
  }
}


void bgui_vsol2D_tableau::set_vsol_point_2d_style(const vgui_style_sptr& style)
{
  point_style_->rgba[0] = style->rgba[0];
  point_style_->rgba[1] = style->rgba[1];
  point_style_->rgba[2] = style->rgba[2];
  point_style_->point_size = style->point_size;
  point_style_->line_width = style->line_width;
}


void bgui_vsol2D_tableau::set_vsol_line_2d_style(const vgui_style_sptr& style)
{
  line_style_->rgba[0] = style->rgba[0];
  line_style_->rgba[1] = style->rgba[1];
  line_style_->rgba[2] = style->rgba[2];
  line_style_->point_size = style->point_size;
  line_style_->line_width = style->line_width;
}


void bgui_vsol2D_tableau::set_vsol_polyline_2d_style(const vgui_style_sptr& style)
{
  polyline_style_->rgba[0] = style->rgba[0];
  polyline_style_->rgba[1] = style->rgba[1];
  polyline_style_->rgba[2] = style->rgba[2];
  polyline_style_->point_size = style->point_size;
  polyline_style_->line_width = style->line_width;
}


void bgui_vsol2D_tableau::set_digital_curve_style(const vgui_style_sptr& style)
{
  digital_curve_style_->rgba[0] = style->rgba[0];
  digital_curve_style_->rgba[1] = style->rgba[1];
  digital_curve_style_->rgba[2] = style->rgba[2];
  digital_curve_style_->point_size = style->point_size;
  digital_curve_style_->line_width = style->line_width;
}


void bgui_vsol2D_tableau::set_dotted_digital_curve_style(const vgui_style_sptr& style)
{
  dotted_digital_curve_style_->rgba[0] = style->rgba[0];
  dotted_digital_curve_style_->rgba[1] = style->rgba[1];
  dotted_digital_curve_style_->rgba[2] = style->rgba[2];
  dotted_digital_curve_style_->point_size = style->point_size;
  dotted_digital_curve_style_->line_width = style->line_width;
}


void bgui_vsol2D_tableau::set_edgel_curve_style(const vgui_style_sptr& style)
{
  edgel_curve_style_->rgba[0] = style->rgba[0];
  edgel_curve_style_->rgba[1] = style->rgba[1];
  edgel_curve_style_->rgba[2] = style->rgba[2];
  edgel_curve_style_->point_size = style->point_size;
  edgel_curve_style_->line_width = style->line_width;
}


void bgui_vsol2D_tableau::set_dotted_edgel_curve_style(const vgui_style_sptr& style)
{
  dotted_edgel_curve_style_->rgba[0] = style->rgba[0];
  dotted_edgel_curve_style_->rgba[1] = style->rgba[1];
  dotted_edgel_curve_style_->rgba[2] = style->rgba[2];
  dotted_edgel_curve_style_->point_size = style->point_size;
  dotted_edgel_curve_style_->line_width = style->line_width;
}
