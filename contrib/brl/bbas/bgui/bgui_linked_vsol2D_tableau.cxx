#include "bgui_linked_vsol2D_tableau.h"
//:
// \file
#include <bgui/bgui_vsol_soview2D.h>
#include <bgui/bgui_linked_vsol_soview2D.h>
#include <vgui/vgui.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_style_factory.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_region_2d.h>

#include <vdgl/vdgl_digital_curve.h>

bgui_linked_vsol2D_tableau::bgui_linked_vsol2D_tableau(const char* n) :
  vgui_easy2D_tableau(n) { this->init(); }

bgui_linked_vsol2D_tableau::bgui_linked_vsol2D_tableau(vgui_image_tableau_sptr const& it,
                                                       const char* n) :
  vgui_easy2D_tableau(it, n) { this->init(); }

bgui_linked_vsol2D_tableau::bgui_linked_vsol2D_tableau(vgui_tableau_sptr const& t,
                                                       const char* n) :
  vgui_easy2D_tableau(t, n) { this->init(); }

bgui_linked_vsol2D_tableau::~bgui_linked_vsol2D_tableau()
{
}

void bgui_linked_vsol2D_tableau::init()
{
  //old_id_ = 0;
  //highlight_ = true;

  //set highlight display style parameters
  //highlight_style_ = new bgui_style(0.0f, 0.0f, 1.0f, 5.0f, 5.0f);

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
//
// vgui_displaylist2D_tableau::motion(..) has a mechanism for
// highlighting the nearest object but it doesn't work.
//
// ***********************************************************
// since June 13, 2003 - mrj
// ***********************************************************
//
// switched to bgui_displaylist2D and bgui_displaybase, which
// have been altered to make highlighting objects work like
// selecting objects, hopefully rendering the below highlighting
// technique unnecessary

bool bgui_linked_vsol2D_tableau::handle(vgui_event const &e)
{
#if 0 // commented out
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
    } */
#endif // 0
  // We aren't interested in other events so pass them to the base class.
  return vgui_easy2D_tableau::handle(e);
}

bgui_vsol_soview2D_point*
bgui_linked_vsol2D_tableau::add_vsol_point_2d(vsol_point_2d_sptr const& p,
                                              const float r,
                                              const float g,
                                              const float b,
                                              const float point_radius)
{
  bgui_vsol_soview2D_point* obj =
    new bgui_vsol_soview2D_point();
  obj->x = p->x();
  obj->y = p->y();
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r , g , b , point_radius , 1.0f ) );
  return obj;
}

bgui_vsol_soview2D_point*
bgui_linked_vsol2D_tableau::add_vsol_point_2d(vsol_point_2d_sptr const& p)
{
  return add_vsol_point_2d( p ,  point_style_.r ,
                                 point_style_.g ,
                                 point_style_.b ,
                                 point_style_.point_radius);
}

bgui_linked_vsol_soview2D_line_seg*
bgui_linked_vsol2D_tableau::add_vsol_line_2d(vsol_line_2d_sptr const& line,
                                             const float r,
                                             const float g,
                                             const float b,
                                             const float line_width)
{
  bgui_linked_vsol_soview2D_line_seg* obj =
    new bgui_linked_vsol_soview2D_line_seg(line);
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r , g , b , 1.0f , line_width ) );
  return obj;
}

bgui_linked_vsol_soview2D_line_seg*
bgui_linked_vsol2D_tableau::add_vsol_line_2d(vsol_line_2d_sptr const& line)
{
  return add_vsol_line_2d( line , line_style_.r ,
                                  line_style_.g ,
                                  line_style_.b ,
                                  line_style_.line_width );
}

bgui_linked_vsol_soview2D_polyline*
bgui_linked_vsol2D_tableau::add_vsol_polyline_2d(vsol_polyline_2d_sptr const& pline,
                                                 const float r,
                                                 const float g,
                                                 const float b,
                                                 const float line_width)
{
  bgui_linked_vsol_soview2D_polyline* obj =
    new bgui_linked_vsol_soview2D_polyline(pline);
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r , g , b , 1.0f , line_width ) );
  return obj;
}

bgui_linked_vsol_soview2D_polyline*
bgui_linked_vsol2D_tableau::add_vsol_polyline_2d(vsol_polyline_2d_sptr const& pline)
{
  return add_vsol_polyline_2d( pline , polyline_style_.r ,
                                       polyline_style_.g ,
                                       polyline_style_.b ,
                                       polyline_style_.line_width );
}

bgui_linked_vsol_soview2D_polygon*
bgui_linked_vsol2D_tableau::add_vsol_polygon_2d(vsol_polygon_2d_sptr const& pline,
                                                 const float r,
                                                 const float g,
                                                 const float b,
                                                 const float line_width)
{
  bgui_linked_vsol_soview2D_polygon* obj =
    new bgui_linked_vsol_soview2D_polygon(pline);
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r , g , b , 1.0f , line_width ) );
  return obj;
}

bgui_linked_vsol_soview2D_polygon*
bgui_linked_vsol2D_tableau::add_vsol_polygon_2d(vsol_polygon_2d_sptr const& pline)
{
  return add_vsol_polygon_2d( pline , polyline_style_.r ,
                                       polyline_style_.g ,
                                       polyline_style_.b ,
                                       polyline_style_.line_width );
}


bgui_linked_vsol_soview2D_digital_curve*
bgui_linked_vsol2D_tableau::add_digital_curve(vdgl_digital_curve_sptr const& dc,
                                              const float r,
                                              const float g,
                                              const float b,
                                              const float line_width)
{
  bgui_linked_vsol_soview2D_digital_curve* obj =
    new bgui_linked_vsol_soview2D_digital_curve(dc);
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r , g , b , 1.0f , line_width ) );
  return obj;
}

bgui_linked_vsol_soview2D_digital_curve*
bgui_linked_vsol2D_tableau::add_digital_curve(vdgl_digital_curve_sptr const& dc)
{
  return add_digital_curve( dc , digital_curve_style_.r ,
                                 digital_curve_style_.g ,
                                 digital_curve_style_.b ,
                                 digital_curve_style_.line_width );
}

bgui_vsol_soview2D_dotted_digital_curve*
bgui_linked_vsol2D_tableau::add_dotted_digital_curve(vdgl_digital_curve_sptr const& dc,
                                                     const float r,
                                                     const float g,
                                                     const float b,
                                                     const float line_width,
                                                     const float point_radius)
{
  bgui_vsol_soview2D_dotted_digital_curve* obj =
    new bgui_vsol_soview2D_dotted_digital_curve(dc);
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r , g , b , point_radius , line_width ) );
  return obj;
}

bgui_vsol_soview2D_dotted_digital_curve*
bgui_linked_vsol2D_tableau::add_dotted_digital_curve(vdgl_digital_curve_sptr const& dc)
{
  return add_dotted_digital_curve( dc , dotted_digital_curve_style_.r ,
                                        dotted_digital_curve_style_.g ,
                                        dotted_digital_curve_style_.b ,
                                        dotted_digital_curve_style_.point_radius ,
                                        dotted_digital_curve_style_.line_width );
}

void bgui_linked_vsol2D_tableau::
add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos,
                    const float r,
                    const float g,
                    const float b,
                    const float line_width,
                    const float point_radius)
{
  for (vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator sit = sos.begin();
       sit != sos.end(); sit++)
    {
      add_spatial_object( (*sit) , r , g , b , line_width, point_radius );
    }
}

void bgui_linked_vsol2D_tableau::
add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos)
{
  for (vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator sit = sos.begin();
       sit != sos.end(); sit++)
    {
      add_spatial_object( (*sit) );
    }
}

void bgui_linked_vsol2D_tableau::
add_spatial_object(vsol_spatial_object_2d_sptr const& sos,
                   const float r,
                   const float g,
                   const float b,
                   const float line_width,
                   const float point_radius)
{
  if (sos->cast_to_point()) {
    {
      vsol_point_2d_sptr p = sos->cast_to_point();
      this->add_vsol_point_2d(p , r , g , b , point_radius );
    }
  }

  if (sos->cast_to_curve()) {
    if (sos->cast_to_curve()->cast_to_digital_curve())
      {
        vdgl_digital_curve_sptr dc =
          sos->cast_to_curve()->cast_to_digital_curve();
        this->add_digital_curve(dc , r , g , b , line_width);
      }
    if (sos->cast_to_curve()->cast_to_line_2d())
      {
        vsol_line_2d_sptr line =
          sos->cast_to_curve()->cast_to_line_2d();
        this->add_vsol_line_2d(line, r , g , b , line_width);
     }
    if (sos->cast_to_curve()->cast_to_polyline_2d())
      {
        vsol_polyline_2d_sptr pline =
          sos->cast_to_curve()->cast_to_polyline_2d();
        this->add_vsol_polyline_2d(pline , r , g , b , line_width);
     }
  }
  return;
}

void bgui_linked_vsol2D_tableau::
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
  if (sos->cast_to_region()) {    
	  if (sos->cast_to_region()->cast_to_polygon_2d()) {
        vsol_polygon_2d_sptr pline =
          sos->cast_to_region()->cast_to_polygon_2d();
        this->add_vsol_polygon_2d(pline);
     }
  }
  return;
}

void bgui_linked_vsol2D_tableau::clear_all()
{
//  bool temp = highlight_;
//  highlight_ = false;//in case of event interrupts during the clear
  vgui_easy2D_tableau::clear();
//  old_id_ = 0;
//  highlight_ = temp;
  this->post_redraw();
}

void bgui_linked_vsol2D_tableau::set_vsol_spatial_object_2d_style(vsol_spatial_object_2d_sptr sos,
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

void bgui_linked_vsol2D_tableau::set_vsol_point_2d_style(const float r, const float g,
                                                         const float b,
                                                         const float point_radius)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, point_radius, 0.0f);
  bgui_vsol_soview2D_point p;
  style_map_[p.type_name()]=sty;
#endif // 0
  point_style_.r = r;
  point_style_.g = g;
  point_style_.b = b;
  point_style_.point_radius = point_radius;
}

void bgui_linked_vsol2D_tableau::set_vsol_line_2d_style(const float r, const float g,
                                                        const float b,
                                                        const float line_width)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vsol_soview2D_line_seg seg;
  style_map_[seg.type_name()]=sty;
#endif // 0
  line_style_.r = r;
  line_style_.g = g;
  line_style_.b = b;
  line_style_.line_width = line_width;
}

void bgui_linked_vsol2D_tableau::set_vsol_polyline_2d_style(const float r, const float g,
                                                            const float b, const float line_width)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vsol_soview2D_polyline pline;
  style_map_[pline.type_name()]=sty;
#endif // 0
  polyline_style_.r = r;
  polyline_style_.g = g;
  polyline_style_.b = b;
  polyline_style_.line_width = line_width;
}

void bgui_linked_vsol2D_tableau::set_digital_curve_style(const float r, const float g,
                                                         const float b,
                                                         const float line_width)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vsol_soview2D_digital_curve dc;
  style_map_[dc.type_name()]=sty;
#endif // 0
  digital_curve_style_.r = r;
  digital_curve_style_.g = g;
  digital_curve_style_.b = b;
  digital_curve_style_.line_width = line_width;
}

void bgui_linked_vsol2D_tableau::set_dotted_digital_curve_style(const float r, const float g,
                                                                const float b,
                                                                const float line_width, const float point_radius)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, point_radius, line_width);
  bgui_vsol_soview2D_dotted_digital_curve dc;
  style_map_[dc.type_name()]=sty;
#endif // 0
  dotted_digital_curve_style_.r = r;
  dotted_digital_curve_style_.g = g;
  dotted_digital_curve_style_.b = b;
  dotted_digital_curve_style_.line_width = line_width;
  dotted_digital_curve_style_.point_radius = point_radius;
}
