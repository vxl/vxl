//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_easy2D
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 24 Sep 99
//
//-----------------------------------------------------------------------------

#include "vgui_easy2D.h"

#include <vcl_vector.h>

#include <vbl/vbl_psfile.h>

#include <vil/vil_image.h>
#include <vil/vil_pixel.h>

#include <vgui/vgui_event.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_displaylist2D.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_style_factory.h>
#include <vgui/vgui_style.h>

static bool debug = false;

vgui_easy2D::vgui_easy2D(char const* n):
  image_slot(this),
  name_(n)
{
  fg[0] = 1.0f;
  fg[1] = 1.0f;
  fg[2] = 0.0f;
  
  line_width = 1;
  point_size = 3;
}


vgui_easy2D::vgui_easy2D(vgui_image_tableau_ref const& i, char const* n) :
  image_slot(this,i),
  image_image(i),
  name_(n)
{
  fg[0] = 1.0f;
  fg[1] = 1.0f;
  fg[2] = 0.0f;
  
  line_width = 1;
  point_size = 3;
}


bool vgui_easy2D::handle(vgui_event const& e) {
  if (image_slot) {
    if (e.type == vgui_DRAW && gl_mode == GL_SELECT) {
      // do nothing
    }
    else {
      image_slot.handle(e);
    }
  }

  return vgui_displaylist2D::handle(e);

}

vcl_string vgui_easy2D::file_name() const {
  if (image_slot) 
    return type_name() + "[" + name_ + ",i=" + image_slot->file_name() + "]";
  else
    return name_ ;
}


vcl_string vgui_easy2D::pretty_name() const {
  if (image_slot) 
    return type_name() + "[" + name_ + ",i=" + image_slot->file_name() + "]";
  else
    return type_name() + "[" + name_ + ",i=null]";
}

vcl_string vgui_easy2D::type_name() const {
  return "vgui_easy2D";
}

void vgui_easy2D::set_image(vcl_string const& fn)
{
  image_image->set_image(fn.c_str());
}

void vgui_easy2D::set_child(vgui_tableau_ref const& i) {
  if (i->type_name() != "vgui_image_tableau" &&
      i->type_name() != "xcv_image_tableau")
    vgui_macro_warning << "assigning what seems like a non-image to my child : i = " << i << vcl_endl;
  image_slot.assign(i);
}

void vgui_easy2D::add(vgui_soview2D* object) {

  vgui_style *style = vgui_style_factory::instance()->get_style(fg[0], fg[1], fg[2], point_size, line_width);
  object->set_style(style);
  
  vgui_displaylist2D::add(object);
}



vgui_soview2D_point* vgui_easy2D::add_point(float x, float y)
{
  vgui_soview2D_point *obj = new vgui_soview2D_point;

  obj->x = x;
  obj->y = y;
  add(obj);
  return obj;
}


vgui_soview2D_lineseg* vgui_easy2D::add_line(float x0, float y0, float x1, float y1) {
  vgui_soview2D_lineseg *obj = new vgui_soview2D_lineseg;

  obj->x0 = x0;
  obj->y0 = y0;
  obj->x1 = x1;
  obj->y1 = y1;

  add(obj);
  return obj;
}

vgui_soview2D_infinite_line* vgui_easy2D::add_infinite_line(float a, float b, float c) {
  vgui_soview2D_infinite_line *obj = new vgui_soview2D_infinite_line;

  obj->a = a;
  obj->b = b;
  obj->c = c;

  add(obj);
  return obj;
}

vgui_soview2D_circle* vgui_easy2D::add_circle(float x, float y, float r) {
  vgui_soview2D_circle *obj = new vgui_soview2D_circle;

  obj->x = x;
  obj->y = y;
  obj->r = r;

  add(obj);
  return obj;
}


void vgui_easy2D::set_foreground(float r, float g, float b)
{
  fg[0] = r;
  fg[1] = g;
  fg[2] = b;
}

void vgui_easy2D::set_line_width(float w)
{
  line_width = w;
}

void vgui_easy2D::set_point_radius(float r)
{
  point_size = r;
}

vgui_soview2D_point* vgui_easy2D::add_point_3dv(double const p[3]) {
  return add_point(p[0]/p[2], p[1]/p[2]);
}

vgui_soview2D_lineseg* vgui_easy2D::add_line_3dv_3dv(double const p[3], double const q[3]) {
  return add_line(p[0]/p[2], p[1]/p[2], 
		  q[0]/q[2], q[1]/q[2]);
}

vgui_soview2D_infinite_line* vgui_easy2D::add_infinite_line_3dv(double const l[3]) {
  return add_infinite_line(l[0], l[1], l[2]);
}

vgui_soview2D_circle* vgui_easy2D::add_circle_3dv(double const point[3], float r) {
  return add_circle(point[0]/point[2], point[1]/point[2], r);
}

vgui_soview2D_linestrip* vgui_easy2D::add_linestrip(unsigned n, float const *x, float const *y) {
  vgui_soview2D_linestrip *obj = new vgui_soview2D_linestrip(n, x, y);

  add(obj);
  return obj;
}

vgui_soview2D_polygon* vgui_easy2D::add_polygon(unsigned n, float const *x, float const *y) {
  vgui_soview2D_polygon *obj = new vgui_soview2D_polygon(n, x, y);

  add(obj);
  return obj;
}

vgui_image_tableau_ref vgui_easy2D::get_image_tableau() {
  return image_image;
}

void vgui_easy2D::print_psfile(vcl_string filename, int reduction_factor, bool print_geom_objs){
  vil_image img = get_image_tableau()->get_image();
  typedef vbl_psfile::byte byte;
  byte* data = new byte[img.width() * img.height()];
  img.get_section(data, 0, 0, img.width(), img.height());
 
  vbl_psfile psfile(filename.c_str());
  psfile.set_parameters(img.width(), img.height());
  psfile.set_reduction_factor(reduction_factor);
  psfile.postscript_header(); 
  if (vil_pixel_format(img) == VIL_BYTE)
  {
    if (debug) vcl_cerr << "vgui_easy2D::print_psfile printing greyscale image to" 
      <<  filename.c_str() << vcl_endl;
    psfile.print_greyscale_image(data, img.width(), img.height());  
  }
  else if (vil_pixel_format(img) == VIL_RGB_BYTE)
  {
    if (debug) vcl_cerr << "vgui_easy2D::print_psfile printing color image to " 
      << filename.c_str() << vcl_endl;
    psfile.print_color_image(data, img.width(), img.height());
  }
  else
    // urgh
    vgui_macro_warning << "failed to print image : " << img << vcl_endl;

  if (print_geom_objs)
  {
    if (debug) vcl_cerr << "vgui_easy2D: Printing geometric objects" << vcl_endl;
    vcl_vector<vgui_soview*> all_objs = get_all();
    vgui_style* style = 0;
    double style_point_size = 0;
    for (vcl_vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); i++)
    {
      vgui_soview* sv = (vgui_soview*)(*i);
      if (sv == NULL)
      {
         vgui_macro_warning << "Object in soview list is null" << vcl_endl;
         return;
      }
      // Send style info if it has changed.
      vgui_style* svstyle = sv->get_style();
      if (svstyle != style) {
	// rgba, line_width, point_size
	style = svstyle;
	psfile.set_line_width(style->line_width);
	style_point_size = style->point_size;
 	psfile.set_fg_color(style->rgba[0],style->rgba[1],style->rgba[2]);
      }
      
      if (sv->type_name() == "vgui_soview2D_point")
      {
        vgui_soview2D_point* pt = (vgui_soview2D_point*)sv;
        psfile.point(pt->x, pt->y, style_point_size);
        if (debug)
	  vcl_cerr << "  vgui_easy2D: Adding a point at " << pt->x << ", " << pt->y << vcl_endl;
      }

      else if (sv->type_name() == "vgui_soview2D_circle")
      {
        vgui_soview2D_circle* circ = (vgui_soview2D_circle*)sv;
        psfile.circle(circ->x, circ->y, circ->r);
        if (debug) vcl_cerr << "  vgui_easy2D: Adding circle, center " << circ->x << ", " 
          << circ->y << " radius " << circ->r << vcl_endl;
      }
      else if (sv->type_name() == "vgui_soview2D_lineseg")
      {
        vgui_soview2D_lineseg* line = (vgui_soview2D_lineseg*)sv;
        psfile.line(line->x0, line->y0, line->x1, line->y1);
        if (debug) vcl_cerr << " vgui_easy2D: Adding line between " << line->x0 << ", " 
          << line->y0 << " and " << line->x1 << ", " << line->y1 << vcl_endl;
      }
      else if(sv->type_name() == "vgui_soview2D_linestrip")
      {
	vgui_soview2D_linestrip *linestrip = (vgui_soview2D_linestrip *)sv;
	for(int ii = 1; ii<linestrip->n; ++ii)
	  psfile.line(linestrip->x[ii-1],linestrip->y[ii-1],
		       linestrip->x[ii  ],linestrip->y[ii  ]);
	if(debug) vcl_cerr<< " vgui_easy2D: Adding linestrip " <<vcl_endl;
      }
      else if(sv->type_name() == "vgui_soview2D_polygon")
      {
	vgui_soview2D_polygon *polygon = (vgui_soview2D_polygon *)sv;
	for(int ii = 1; ii<polygon->n; ++ii)
	  psfile.line(polygon->x[ii-1],polygon->y[ii-1],
		       polygon->x[ii  ],polygon->y[ii  ]);
	psfile.line(polygon->x[polygon->n - 1],polygon->y[polygon->n - 1], polygon->x[0], polygon->y[0]);
	if(debug) vcl_cerr<< " vgui_easy2D: Adding polygon " <<vcl_endl;
      }
      else
        vgui_macro_warning << "unknown soview typename = " << sv->type_name() << vcl_endl;
    }
  }
}
