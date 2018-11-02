// This is core/vgui/vgui_easy2D_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief  See vgui_easy2D_tableau.h for a description of this file.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   24 Sep 1999
//
// \verbatim
//  Modifications
//   24-SEP-1999 P.Pritchett - Initial version.
// \endverbatim

#include <vector>
#include <cstdlib>
#include "vgui_easy2D_tableau.h"

#include <cassert>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vxl_config.h> // for vxl_byte

#include <vul/vul_psfile.h>
#include <vnl/vnl_math.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_pixel.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>

#include <vgui/vgui_event.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_displaylist2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_style.h>

#ifdef DEBUG
#include <vil/vil_save.h>
#endif

static bool debug = false;

vgui_easy2D_tableau::vgui_easy2D_tableau(const char* n) : image_slot(this), name_(n), style_(vgui_style::new_style())
{
  style_->rgba[0] = 1.0f;
  style_->rgba[1] = 1.0f;
  style_->rgba[2] = 0.0f;

  style_->line_width = 1;
  style_->point_size = 3;
}


vgui_easy2D_tableau::vgui_easy2D_tableau(vgui_image_tableau_sptr const& i, const char* n)
  : image_slot(this,i), image_image(i), name_(n), style_(vgui_style::new_style())
{
  style_->rgba[0] = 1.0f;
  style_->rgba[1] = 1.0f;
  style_->rgba[2] = 0.0f;

  style_->line_width = 1;
  style_->point_size = 3;
}


vgui_easy2D_tableau::vgui_easy2D_tableau(vgui_tableau_sptr const& i, const char* n)
  : image_slot(this,i), name_(n), style_(vgui_style::new_style())
{
  style_->rgba[0] = 1.0f;
  style_->rgba[1] = 1.0f;
  style_->rgba[2] = 0.0f;

  style_->line_width = 1;
  style_->point_size = 3;
}


bool vgui_easy2D_tableau::handle(vgui_event const& e)
{
#if 0 //event tracker
  if(e.type != vgui_MOTION)
    std::cout << "easy2D" << e << '\n' << std::flush;
#endif
  if (image_slot && (e.type != vgui_DRAW || gl_mode != GL_SELECT))
    image_slot.handle(e);

  return vgui_displaylist2D_tableau::handle(e);
}

std::string vgui_easy2D_tableau::file_name() const
{
  if (image_slot)
    return type_name() + "[" + name_ + ",i=" + image_slot->file_name() + "]";
  else
    return name_;
}


std::string vgui_easy2D_tableau::pretty_name() const
{
  if (image_slot)
    return type_name() + "[" + name_ + ",i=" + image_slot->file_name() + "]";
  else
    return type_name() + "[" + name_ + ",i=null]";
}

std::string vgui_easy2D_tableau::type_name() const
{
  return "vgui_easy2D_tableau";
}

//: Set the child tableau to be the given image_tableau.
void vgui_easy2D_tableau::set_image(std::string const& fn)
{
  image_image->set_image(fn.c_str());
}

//: Set the child tableau to be the given tableau.
void vgui_easy2D_tableau::set_child(vgui_tableau_sptr const& i)
{
  if (i && i->type_name() != "vgui_image_tableau" &&
      i->type_name() != "xcv_image_tableau")
    vgui_macro_warning << "assigning what seems like a non-image to my child : i = " << i << std::endl;
  image_slot.assign(i);
}

//: Set the colour of objects to the given red, green, blue, alpha values.
void vgui_easy2D_tableau::set_foreground(float r, float g, float b, float a)
{
  // create a new style object so that already added objects don't
  // suddenly change
  style_ = vgui_style::new_style( style_ );
  style_->rgba[0] = r;
  style_->rgba[1] = g;
  style_->rgba[2] = b;
  style_->rgba[3] = a;
}

//: Set the width of lines to the given width.
void vgui_easy2D_tableau::set_line_width(float w)
{
  // create a new style object so that already added objects don't
  // suddenly change
  style_ = vgui_style::new_style( style_ );
  style_->line_width = w;
}

//: Set the radius of points to the given radius.
void vgui_easy2D_tableau::set_point_radius(float r)
{
  // create a new style object so that already added objects don't
  // suddenly change
  style_ = vgui_style::new_style( style_ );
  style_->point_size = r;
}

//: Add the given two-dimensional object to the display.
void vgui_easy2D_tableau::add(vgui_soview2D* object)
{
  object->set_style(style_);
  vgui_displaylist2D_tableau::add(object);
}

//: Add a point at the given position to the display.
vgui_soview2D_point* vgui_easy2D_tableau::add_point(float x, float y)
{
  vgui_soview2D_point *obj = new vgui_soview2D_point;

  obj->x = x;
  obj->y = y;
  add(obj);
  return obj;
}

//: Add a finite line with the given start and end points to the display.
//  Note that this will be added as a vgui_lineseg (not vgui_line - which doesn't exist).
vgui_soview2D_lineseg* vgui_easy2D_tableau::add_line(float x0, float y0, float x1, float y1)
{
  vgui_soview2D_lineseg *obj = new vgui_soview2D_lineseg;

  obj->x0 = x0;
  obj->y0 = y0;
  obj->x1 = x1;
  obj->y1 = y1;

  add(obj);
  return obj;
}

//: Add an infinite line (ax + by +c = 0) to the display.
vgui_soview2D_infinite_line* vgui_easy2D_tableau::add_infinite_line(float a, float b, float c)
{
  vgui_soview2D_infinite_line *obj = new vgui_soview2D_infinite_line;

  obj->a = a;
  obj->b = b;
  obj->c = c;

  add(obj);
  return obj;
}

//: Add a circle with the given centre and radius to the display.
vgui_soview2D_circle* vgui_easy2D_tableau::add_circle(float x, float y, float r)
{
  vgui_soview2D_circle *obj = new vgui_soview2D_circle;

  obj->x = x;
  obj->y = y;
  obj->r = r;

  add(obj);
  return obj;
}

vgui_soview2D_ellipse* vgui_easy2D_tableau::add_ellipse(float x, float y, float w, float h, float phi)
{
  vgui_soview2D_ellipse *obj = new vgui_soview2D_ellipse;

  obj->x = x;
  obj->y = y;
  obj->w = w;
  obj->h = h;
  obj->phi = phi;

  add(obj);
  return obj;
}

//: Add a point with the given projective coordinates.
vgui_soview2D_point* vgui_easy2D_tableau::add_point_3dv(double const p[3])
{
  return add_point(float(p[0]/p[2]), float(p[1]/p[2]));
}

//: Add a line with the given projective start and end points.
vgui_soview2D_lineseg* vgui_easy2D_tableau::add_line_3dv_3dv(double const p[3], double const q[3])
{
  return add_line(float(p[0]/p[2]), float(p[1]/p[2]),
                  float(q[0]/q[2]), float(q[1]/q[2]));
}

//: Add an infinite line with the given projective coordinates.
vgui_soview2D_infinite_line* vgui_easy2D_tableau::add_infinite_line_3dv(double const l[3])
{
  return add_infinite_line(float(l[0]), float(l[1]), float(l[2]));
}

//: Add a circle with the given centre (in projective coords) and radius to the display.
vgui_soview2D_circle* vgui_easy2D_tableau::add_circle_3dv(double const point[3], float r)
{
  return add_circle(float(point[0]/point[2]), float(point[1]/point[2]), r);
}

//: Add a linestrip with the given n vertices to the display.
vgui_soview2D_linestrip* vgui_easy2D_tableau::add_linestrip(unsigned n, float const *x, float const *y)
{
  vgui_soview2D_linestrip *obj = new vgui_soview2D_linestrip(n, x, y);

  add(obj);
  return obj;
}

//: Add  polygon with the given n vertices to the display.
vgui_soview2D_polygon* vgui_easy2D_tableau::add_polygon(unsigned n, float const *x, float const *y, bool filled)
{
  vgui_soview2D_polygon *obj = new vgui_soview2D_polygon(n, x, y, filled);
  add(obj);
  return obj;
}

//: Screen dump to postscript file.
//  Specify the optional arguments in case this tableau does not contain
//  an image tableau, or if you want a smaller part of the image printed.
//  If wd or ht are 0, no image is printed at all.
void vgui_easy2D_tableau::print_psfile(std::string filename, int reduction_factor, bool print_geom_objs, int wd, int ht)
{
  // Set wd and ht from the image tableau, if not specified as parameters
  if (wd < 0 || ht < 0)
  {
    assert(get_image_tableau());
    if (get_image_tableau()->get_image_resource())
    {
      vil_image_resource_sptr img_sptr = get_image_tableau()->get_image_resource();
      if (wd < 0) wd = img_sptr->ni();
      if (ht < 0) ht = img_sptr->nj();
    }
    else
    {
      vil1_image img = get_image_tableau()->get_image();
      if (wd < 0) wd = img.width();
      if (ht < 0) ht = img.height();
      assert (wd <= img.width());
      assert (ht <= img.height());
    }
  }

  // Write PostScript header
  vul_psfile psfile(filename.c_str(), false);
  psfile.set_paper_layout(vul_psfile::MAX);
  psfile.set_reduction_factor(reduction_factor);
  // psfile.set_parameters(wd, ht); // no longer needed - vul_psfile does this

  // Write image, if present
  if (get_image_tableau() && wd*ht > 0)
  {
    if (get_image_tableau()->get_image_resource())
    {
      vil_image_resource_sptr img_sptr = get_image_tableau()->get_image_resource();
      vil_image_view<vxl_byte> img= img_sptr->get_view();
      if (!img)
      {
        // invalid pixel type
        vgui_macro_warning<< "failed to print image of unsupported pixel format: "
                          << img << std::endl;
      }
      if (img.nplanes()==1) // greyscale image
      {
        if (img.istep() != 1)
          vgui_macro_warning<< "The istep of this image view is not 1: "
                            << img << std::endl;
        else {
          if (debug)
            std::cerr << "vgui_easy2D_tableau::print_psfile printing greyscale image to"
                     << filename.c_str() << '\n';
          psfile.print_greyscale_image(img.top_left_ptr(), img.ni(), img.nj());
        }
      }
      else if (img.nplanes() == 3) // color image
      {}
      else
        // urgh
        vgui_macro_warning<< "Don't know how to handle image with "
                          << img.nplanes() << " planes" << std::endl;
    }
    else
    {
      vil1_image img = get_image_tableau()->get_image();
      unsigned char* data = new unsigned char[img.get_size_bytes()];
      img.get_section(data, 0, 0, wd, ht);
      if (vil1_pixel_format(img) == VIL1_BYTE)
      {
        if (debug)
          std::cerr << "vgui_easy2D_tableau::print_psfile printing greyscale image to"
                   << filename.c_str() << '\n';
        psfile.print_greyscale_image(data, wd, ht);
      }
      else if (vil1_pixel_format(img) == VIL1_RGB_BYTE)
      {
        if (debug)
          std::cerr << "vgui_easy2D_tableau::print_psfile printing color image to "
                   << filename.c_str() << '\n';
        psfile.print_color_image(data, wd, ht);
      }
      else
        // urgh
        vgui_macro_warning<< "failed to print image of unsupported pixel format: "
                          << img << std::endl;
      delete[] data;
    }
  }

  // Skip the rest of this function if no geometry is wanted
  if (!print_geom_objs) return;
  if (debug)
    std::cerr << "vgui_easy2D_tableau: Printing geometric objects\n";

  std::vector<vgui_soview*> all_objs = get_all();
  vgui_style_sptr style = nullptr;
  float style_point_size = 0;
  for (std::vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); ++i)
  {
    vgui_soview* sv = *i;
    if (sv == nullptr) {
       vgui_macro_warning << "An object in soview list is null\n";
       continue;
    }
    // Send style info if it has changed.
    vgui_style_sptr svstyle = sv->get_style();
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
        std::cerr << "  vgui_easy2D_tableau: Adding a point at "
                 << pt->x << ", " << pt->y << '\n';
    }
    else if (sv->type_name() == "vgui_soview2D_circle")
    {
      vgui_soview2D_circle* circ = (vgui_soview2D_circle*)sv;
      psfile.circle(circ->x, circ->y, circ->r);
      if (debug)
        std::cerr << "  vgui_easy2D_tableau: Adding circle, center " << circ->x << ", "
                 << circ->y << " radius " << circ->r << '\n';
    }
    else if (sv->type_name() == "vgui_soview2D_lineseg")
    {
      vgui_soview2D_lineseg* line = (vgui_soview2D_lineseg*)sv;
      psfile.line(line->x0, line->y0, line->x1, line->y1);
      if (debug)
        std::cerr << " vgui_easy2D_tableau: Adding line between " << line->x0 << ", "
                 << line->y0 << " and " << line->x1 << ", " << line->y1 << '\n';
    }
    else if (sv->type_name() == "vgui_soview2D_linestrip")
    {
      vgui_soview2D_linestrip *linestrip = (vgui_soview2D_linestrip *)sv;
      for (unsigned int ii = 1; ii<linestrip->n; ++ii)
        psfile.line(linestrip->x[ii-1],linestrip->y[ii-1],
                    linestrip->x[ii  ],linestrip->y[ii  ]);
      if (debug)
        std::cerr<< " vgui_easy2D_tableau: Adding linestrip\n";
    }
    else if (sv->type_name() == "vgui_soview2D_polygon")
    {
      vgui_soview2D_polygon *polygon = (vgui_soview2D_polygon *)sv;
      for (unsigned int ii = 1; ii<polygon->n; ++ii)
        psfile.line(polygon->x[ii-1],polygon->y[ii-1],
                    polygon->x[ii  ],polygon->y[ii  ]);
      psfile.line(polygon->x[polygon->n - 1],
                  polygon->y[polygon->n - 1],
                  polygon->x[0], polygon->y[0]);
      if (debug)
        std::cerr<< " vgui_easy2D_tableau: Adding polygon\n";
    }
    else if (sv->type_name() == "vgui_soview2D_ellipse")
    {
      vgui_soview2D_ellipse* ellip = (vgui_soview2D_ellipse*)sv;
      psfile.ellipse(ellip->x, ellip->y, ellip->w, ellip->h, int(vnl_math::deg_per_rad * ellip->phi + 0.5)); // convert radians to degrees
      if (debug)
        std::cerr << "  vgui_easy2D_tableau: Adding ellipse, center " << ellip->x << ", "
                 << ellip->y << " width " << ellip->w << " height " << ellip->h
                 << " angle " << ellip->phi << " (" << int(vnl_math::deg_per_rad * ellip->phi + 0.5) << " deg)\n";
    }
    else
      vgui_macro_warning << "unknown soview typename = " << sv->type_name() << std::endl;
  }
}

//: Add an image at the given position to the display.
vgui_soview2D_image* vgui_easy2D_tableau::add_image( float x, float y,
                                                     vil1_image const& img )
{
  // Assume alpha blending is necessary iff there are four components
  // in the image
  bool blend = false;
  if ( img.components() == 4 )
    blend = true;
  vgui_soview2D_image *obj = new vgui_soview2D_image( x, y, img, blend );
  add(obj);
  return obj;
}

//: Add an image at the given position to the display.
vgui_soview2D_image* vgui_easy2D_tableau::add_image( float x, float y,
                                                     vil_image_view_base const& img )
{
  // Assume alpha blending is necessary iff there are four components
  // in the image
  bool blend = false;
  if ( img.nplanes() == 4 )
    blend = true;
  vgui_soview2D_image *obj = new vgui_soview2D_image( x, y, img, blend );
  add(obj);
  return obj;
}
//: access the child on the parent-child link
vgui_tableau_sptr vgui_easy2D_tableau::child() const{
  return image_slot.child();
}

#if 0 // deprecated method
vgui_soview2D_image* vgui_easy2D_tableau::add_image( float x, float y,
                                                     float width, float height,
                                                     char *data,
                                                     unsigned int format, unsigned int type )
{
  if ( type != GL_UNSIGNED_BYTE ) {
    std::cerr << "Don't know how to add non-byte sprites using old interface\n";
    std::abort();
  }
  unsigned w = unsigned(width);
  unsigned h = unsigned(height);
  vil_image_view<vxl_byte> img;
  if ( format == GL_RGB ) {
    img = vil_image_view<vxl_byte>( reinterpret_cast<vxl_byte*>(data), w, h, 3, 3, w*3, 1 );
#ifdef DEBUG
    vil_save(img, "tmp_image_GL_RGB.jpeg", "jpeg");
#endif
  }
  else if ( format == GL_RGBA ) {
    img = vil_image_view<vxl_byte>( reinterpret_cast<vxl_byte*>(data), w, h, 4, 4, w*4, 1 );
#ifdef DEBUG // inserted debugging jr Oct 24, 2003
    vil_save(img, "tmp_image_GL_RGBA.jpeg", "jpeg");
#endif
  }
  else {
    std::cerr << "Don't know how to handle format " << format << " with old interface\n";
    std::abort();
  }
  return this->add_image( x, y, img );
}
#endif // 0
