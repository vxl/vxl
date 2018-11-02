#include <iostream>
#include <algorithm>
#include "mbl_eps_writer.h"
//:
// \file
// \brief Class to generate simple EPS files containing images and lines
// \author Tim Cootes

#include <vil/vil_plane.h>
#include <vil/vil_crop.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//=======================================================================
// Dflt ctor
//=======================================================================

mbl_eps_writer::mbl_eps_writer()
  : nx_(100.0),ny_(100.0),sx_(1.0),sy_(1.0)
{
}

//: Open file and write header, given bounding box
// Bounding box specified in "points" (72 points=1 inch)
mbl_eps_writer::mbl_eps_writer(const char* path, double nx, double ny)
  : sx_(1.0),sy_(1.0)
{
   open(path,nx,ny);
}

//=======================================================================
// Destructor
//=======================================================================

mbl_eps_writer::~mbl_eps_writer() = default;

//: Define shade of subsequent lines [0,1] = black-white
void mbl_eps_writer::set_grey_shade(double shade)
{
  if (shade<0.0) shade=0.0;
  if (shade>1.0) shade=1.0;
  ofs_<<shade<<" setgray\n";
}

//: Define colour of subsequent lines r,g,b in [0,1]
void mbl_eps_writer::set_rgb(double r, double g, double b)
{
  ofs_<<r<<' '<<g<<' '<<b<<" setrgbcolor\n";
}

//: Set colour of subsequent graphics using a named colour
//  Valid options include black,white,grey,red,green,blue,
//  cyan,yellow.  Note: Probably need a tidy map thing to
//  do this properly.  Sets to grey if colour not known.
void mbl_eps_writer::set_colour(const std::string& colour_name)
{
  if (colour_name=="black") { set_grey_shade(0.0); return; }
  if (colour_name=="white") { set_grey_shade(1.0); return; }
  if (colour_name=="grey") { set_grey_shade(0.5); return; }
  if (colour_name=="grey25") { set_grey_shade(0.25); return; }
  if (colour_name=="grey75") { set_grey_shade(0.75); return; }
  if (colour_name=="red") { set_rgb(1,0,0); return; }
  if (colour_name=="green") { set_rgb(0,1,0); return; }
  if (colour_name=="blue") { set_rgb(0,0,1); return; }
  if (colour_name=="cyan") { set_rgb(0,1,1); return; }
  if (colour_name=="yellow") { set_rgb(1,1,0); return; }

  // Colour not recognised, so set to grey.
  set_grey_shade(0.5);
}

//: Define scaling factor
void mbl_eps_writer::set_scaling(double s)
{
  sx_=s; sy_=s;
}

//: Define scaling factor
void mbl_eps_writer::set_scaling(double sx, double sy)
{
  sx_=sx; sy_=sy;
}


//: Open file and write header
bool mbl_eps_writer::open(const char* path, double nx, double ny)
{
  ofs_.open(path,std::ios::out);
  if (!ofs_) return false;

  ofs_<<"%!PS-Adobe-1.0\n"
      <<"%%Creator: mbl_eps_writer\n"
  //  <<"%%Title: shapes\n"
      <<"%%BoundingBox: 0 0 "<<nx<<' '<<ny<<'\n'

      <<"gsave\n"
      <<"% Define some simple macros to save space\n"
      <<"/M {moveto} def\n"
      <<"/L {lineto} def\n\n"
      <<"/CP {closepath} def\n\n";

  nx_ = nx; ny_=ny;

  return true;
}

//: Creates file and draws image, setting bounding box to that of image
//  Sets scaling to pixel widths, so that subsequent points are
//  interpreted in pixel units.
bool mbl_eps_writer::open(const char* path,
                          const vil_image_view<vxl_byte>& image,
                          double pixel_width_x, double pixel_width_y)
{
  if (!open(path,image.ni()*pixel_width_x,image.nj()*pixel_width_y))
    return false;
  set_scaling(pixel_width_x,pixel_width_y);

  draw_image(image,0,0,1,1);
  return true;
}


//: Write tail and close
void mbl_eps_writer::close()
{
  ofs_<<"grestore\n";
  ofs_.close();
}

//: Define line width.
void mbl_eps_writer::set_line_width(double w)
{
  ofs_<<w<<" setlinewidth\n";
}


//: Draws circle of radius r around p
void mbl_eps_writer::draw_circle(const vgl_point_2d<double>& p, double r)
{
  ofs_<<"newpath\n\n"
      <<sx_*p.x()<<' '<<ny_-sy_*p.y()<<' '<<sx_*r<<" 0 360 arc CP\n\n"
      <<"stroke"<<std::endl;
}

//: Draws disk of radius r around p
void mbl_eps_writer::draw_disk(const vgl_point_2d<double>& p, double r)
{
  ofs_<<"newpath\n"
      <<sx_*p.x()<<' '<<ny_-sy_*p.y()<<' '<<sx_*r<<" 0 360 arc CP fill\n"
      <<"stroke"<<std::endl;
}

//: Draws line segment from p1 to p2
void mbl_eps_writer::draw_line(const vgl_point_2d<double>& p1, const vgl_point_2d<double>& p2)
{
  ofs_<<"newpath\n"
      <<sx_*p1.x()<<' '<<ny_-sy_*p1.y()<<" M "
      <<sx_*p2.x()<<' '<<ny_-sy_*p2.y()<<" L\n"
      <<"stroke\n";
}

//: Draws filled box covering whole region
//  Should be called before anything else.
void mbl_eps_writer::draw_background_box()
{
  ofs_<<"newpath\n"
      <<"0 0 M "
      <<nx_<<" 0 L "
      <<nx_<<' '<<ny_<<" L "
      <<"0 "<<ny_<<" L fill stroke\n";
}


//: Draws rectangle of given width/height, corner at (x,y)
void mbl_eps_writer::draw_box(double x, double y, double w, double h, bool filled)
{
  ofs_<<"newpath\n"
      <<sx_*x<<' '<<ny_-sy_*y<<" M "
      <<sx_*(x+w)<<' '<<ny_-sy_*y<<" L "
      <<sx_*(x+w)<<' '<<ny_-sy_*(y+h)<<" L "
      <<sx_*x<<' '<<ny_-sy_*(y+h)<<" L ";
  if (filled) ofs_<<"fill ";
  ofs_<<"stroke\n";
}


//: Draws polygon connecting points.
//  If closed, then adds line joining last to first point.
//  If filled, then fills with current colour/greyshade.
void mbl_eps_writer::draw_polygon(const std::vector<vgl_point_2d<double> >& pts,
                                  bool closed, bool filled)
{
  if (pts.size()<2) return;
  ofs_<<"newpath\n"
      <<sx_*pts[0].x()<<' '<<ny_-sy_*pts[0].y()<<" M ";
  for (unsigned i=1;i<pts.size();++i)
    ofs_<<sx_*pts[i].x()<<' '<<ny_-sy_*pts[i].y()<<" L\n";
  if (closed)
  {
    ofs_<<sx_*pts[0].x()<<' '<<ny_-sy_*pts[0].y()<<" L CP\n";
  }
  if (filled) ofs_<<"fill ";
  ofs_<<"stroke\n";
}


//: Writes first plane of image in hex format to os
void mbl_eps_writer::write_image_data(std::ostream& os,
                                      const vil_image_view<vxl_byte>& image)
{
  unsigned ni=image.ni(),nj=image.nj();
  os<<"{<";
  for (unsigned j=0;j<nj;++j)
  {
    // Write each row
    for (unsigned i=0;i<ni;++i)
    {
      os<<std::hex<<int(image(i,j))/16<<int(image(i,j))%16;
    }
    os<<std::endl;
  }
  os<<">}\n"
    <<std::dec;  // Ensure returns to decimal
}

void mbl_eps_writer::draw_image(const vil_image_view<vxl_byte>& image,
                                double tx, double ty,
                                double pixel_width_x, double pixel_width_y)
{
  unsigned ni=image.ni(),nj=image.nj();

  if (ni<256 && nj<256)
  {
    draw_image_block(image,tx,ty,pixel_width_x,pixel_width_y);
    return;
  }

  // Split into smaller blocks
  unsigned max_w = 200;
  unsigned ni_blocks = 1+(ni-1)/max_w;
  unsigned nj_blocks = 1+(nj-1)/max_w;
  for (unsigned j=0;j<nj_blocks;++j)
    for (unsigned i=0;i<ni_blocks;++i)
    {
      unsigned wi=std::min(max_w,ni-(i*max_w));
      unsigned wj=std::min(max_w,nj-(j*max_w));
      vil_image_view<vxl_byte> cropped=vil_crop(image,i*max_w,wi,j*max_w,wj);
      double tx1=tx+i*max_w*pixel_width_x;
      double ty1=ty+j*max_w*pixel_width_y;
      draw_image_block(cropped,tx1,ty1,pixel_width_x,pixel_width_y);
    }
}

//: Creates a greyscale image  at (tx,ty) with given pixel widths
//  Size in points given by sx(),sy() * given values.
//  Image must be no bigger than 255x255
//  Some postscript can't cope with bigger blocks.
void mbl_eps_writer::draw_grey_image_block(
                    const vil_image_view<vxl_byte>& image,
                    double tx, double ty,
                    double pixel_width_x, double pixel_width_y)
{
  assert(image.ni()<256);
  assert(image.nj()<256);
  ofs_<<"gsave\n"
      <<sx_*tx<<' '<<ny_-sy_*ty<<" translate\n";
  unsigned ni=image.ni(),nj=image.nj();
  ofs_<<sx_*ni*pixel_width_x<<" -"<<sy_*nj*pixel_width_y<<" scale\n"
      <<ni<<' '<<nj<<" % Image size\n"
      <<"8 % Bits per pixel\n"
      <<"[ "<<ni<<" 0 0 "<<nj<<" 0 0]\n";  // Transformation (unit sqr to pixels)
  // Now draw the image data
  write_image_data(ofs_,image);

  ofs_<<"image\n"
      <<"grestore\n";
}

//: Creates a colour image with given pixel widths
//  Image assumed to be a 3 plane image.
void mbl_eps_writer::draw_rgb_image_block(
                       const vil_image_view<vxl_byte>& image,
                       double tx, double ty,
                       double pixel_width_x, double pixel_width_y)
{
  assert(image.ni()<256);
  assert(image.nj()<256);

  ofs_<<"gsave\n"
      <<sx_*tx<<' '<<ny_-sy_*ty<<" translate\n";
  unsigned ni=image.ni(),nj=image.nj();
  ofs_<<sx_*ni*pixel_width_x<<" -"<<sy_*nj*pixel_width_y<<" scale\n"
      <<ni<<' '<<nj<<" % Image size\n"
      <<"8 % Bits per pixel\n"
      <<"[ "<<ni<<" 0 0 "<<nj<<" 0 0]\n";  // Transformation (unit sqr to pixels)
  // Now draw the image data
  write_image_data(ofs_,vil_plane(image,0));  // Red
  write_image_data(ofs_,vil_plane(image,1));  // Green
  write_image_data(ofs_,vil_plane(image,2));  // Blue

  ofs_<<"true 3 colorimage\n"
      <<"grestore\n";
}

//: Creates an image  at (tx,ty) with given pixel widths
//  Size in points given by sx(),sy() * given values.
void mbl_eps_writer::draw_image_block(const vil_image_view<vxl_byte>& image,
                                      double tx, double ty,
                                      double pixel_width_x, double pixel_width_y)
{
  if (image.nplanes()==1)
    draw_grey_image_block(image,tx,ty,pixel_width_x,pixel_width_y);
  else
  if (image.nplanes()==3)
    draw_rgb_image_block(image,tx,ty,pixel_width_x,pixel_width_y);
  else
    std::cerr<<"mbl_eps_writer::draw_image_block() Can't cope with image."<<std::endl;
}
