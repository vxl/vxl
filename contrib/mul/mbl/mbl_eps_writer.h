#ifndef mbl_eps_writer_h_
#define mbl_eps_writer_h_
//:
// \file
// \brief Class to generate simple EPS files containing images and lines
// \author Tim Cootes

#include <string>
#include <iostream>
#include <fstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>

#include <vxl_config.h>

//: Class to generate simple EPS files containing images and lines
//  Note that Postscript uses units of points=1/72 inch. Scaling may
//  be required.  Also, by default origin is in bottom left.  This
//  class treats the origin at top right, to comply with usual VXL
//  standards.  The drawing area is a box (nx,ny), based at the origin.
//
// The output eps file should be suitable for use in LaTeX documents.
//
// Example usage:
// \code
// mbl_eps_writer eps_writer("example.eps",100,100);
// eps_writer.set_colour("blue");
// eps_writer.draw_circle(centre,radius);
// eps_writer.close();
// \endcode

class mbl_eps_writer
{
 private:
  //: Stream to eps file
  std::ofstream ofs_;

  //: Bounding box (in points)
  double nx_,ny_;

  //: Current scaling
  double sx_,sy_;

  //: Creates a greyscale image  at (tx,ty) with given pixel widths
  //  Size in points given by sx(),sy() * given values.
  //  Image must be no bigger than 255x255.
  //  Some postscript can't cope with bigger blocks.
  void draw_grey_image_block(const vil_image_view<vxl_byte>& image,
                             double tx, double ty,
                             double pixel_width_x, double pixel_width_y);

  //: Creates a colour image with given pixel widths
  //  Image assumed to be a 3 plane image.
  //  Image must be no bigger than 255x255.
  //  Some postscript can't cope with bigger blocks.
  void draw_rgb_image_block(const vil_image_view<vxl_byte>& image,
                            double tx, double ty,
                            double pixel_width_x, double pixel_width_y);

  //: Creates a grey or colour image with given pixel widths
  //  Image assumed to have either 1 or 3 planes.
  //  Image must be no bigger than 255x255.
  //  Some postscript can't cope with bigger blocks.
  void draw_image_block(const vil_image_view<vxl_byte>& image,
                        double tx, double ty,
                        double pixel_width_x, double pixel_width_y);

 public:

  //: Dflt ctor
  mbl_eps_writer();

  //: Open file and write header, given bounding box
  // Bounding box specified in "points" (72 points=1 inch)
  mbl_eps_writer(const char* path, double nx, double ny);


  //: Destructor
  virtual ~mbl_eps_writer();

  //: Current stream used to write EPS file.
  std::ofstream& ofs() { return ofs_; }

  //: Define shade of subsequent graphics [0,1] = black-white
  void set_grey_shade(double shade);

  //: Define colour of subsequent graphics r,g,b in [0,1]
  void set_rgb(double r, double g, double b);

  //: Draws filled box covering whole region
  //  Should be called before anything else.
  void draw_background_box();

  //: Set colour of subsequent graphics using a named colour
  //  Valid options include black,grey25,grey,grey75,white red,green,blue,
  //  cyan,yellow.  Note: Probably need a tidy map thing to
  //  do this properly.  Sets to grey if colour not known.
  void set_colour(const std::string& colour_name);

  //: Define scaling factor
  void set_scaling(double s);

  //: Define scaling factor
  void set_scaling(double sx, double sy);

  //: Open file and write header, given bounding box
  // Bounding box specified in "points" (72 points=1 inch)
  bool open(const char* path, double nx, double ny);

  //: Creates file and draws image, setting bounding box to that of image
  //  Sets scaling to pixel widths, so that subsequent points are
  //  interpreted in pixel units.
  bool open(const char* path, const vil_image_view<vxl_byte>& image,
            double pixel_width_x, double pixel_width_y);

  //: Write tail and close
  void close();

  //: Draws disk of radius r around p, with current colour
  void draw_disk(const vgl_point_2d<double>& p, double r);

  //: Draws circle of radius r around p, with current colour
  void draw_circle(const vgl_point_2d<double>& p, double r);

  //: Draws rectangle of given width/height, corner at (x,y)
  void draw_box(double x, double y,
                double width, double height, bool filled=false);

  //: Draws line segment from p1 to p2
  void draw_line(const vgl_point_2d<double>& p1,
                 const vgl_point_2d<double>& p2);

  //: Draws polygon connecting points.
  //  If closed, then adds line joining last to first point.
  //  If filled, then fills with current colour/greyshade.
  void draw_polygon(const std::vector<vgl_point_2d<double> >& pts,
                    bool closed=true, bool filled=false);

  //: Define line width.
  void set_line_width(double w);

  //: Writes first plane of image in hex format to os
  void write_image_data(std::ostream& os,
                        const vil_image_view<vxl_byte>& image);

  //: Creates an image  at (tx,ty) with given pixel widths
  //  Size in points given by sx(),sy() * given values.
  void draw_image(const vil_image_view<vxl_byte>& image,
                  double tx, double ty,
                  double pixel_width_x, double pixel_width_y);
};

#endif // mbl_eps_writer_h_
