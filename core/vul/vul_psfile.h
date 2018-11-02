// This is core/vul/vul_psfile.h
#ifndef vul_psfile_h_
#define vul_psfile_h_
//:
// \file
// \brief write out images, points, lines, circles and/or ellipses to PostScript
// \author Alan S. Liu
//
// \verbatim
// Modifications
//  7 Jan 2003 - Peter Vanroose - bug fix in image output: complete rewrite of
//                                print_greyscale_image() & print_color_image()
// \endverbatim

#include <string>
#include <fstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Write a PostScript file
class vul_psfile: public std::ofstream
{
 public:
  enum paper_type {
    US_NORMAL,
    A4,
    B5,
    A3,
    US_LEGAL,
    ELEVEN_BY_SEVENTEEN,
    FOUR_BY_FIVE,
    THIRTY_FIVE_mm};
  enum paper_orientation{
    PORTRAIT,
    LANDSCAPE };
  enum paper_layout{
    CENTER,
    MAX };

  vul_psfile(char const* filename, bool debug_output=false);
  ~vul_psfile() override;
  operator bool() { return static_cast<bool>(output_filestream); }

  void set_paper_type(vul_psfile::paper_type type){printer_paper_type = type;}
  void set_paper_layout(vul_psfile::paper_layout layout) {printer_paper_layout = layout;}
  void set_paper_orientation(vul_psfile::paper_orientation o) {printer_paper_orientation = o;}
  void set_reduction_factor(int rf) {reduction_factor = rf;}
  //: set the horizontal scaling (in percent); no scaling is 100.
  void set_scale_x(float sx) {scale_x = sx * .01f;}
  //: set the vertical scaling (in percent); no scaling is 100.
  void set_scale_y(float sy) {scale_y = sy * .01f;}
  void set_fg_color(float r, float g, float b) {fg_r = r; fg_g = g; fg_b = b;}
  void set_bg_color(float r, float g, float b) {bg_r = r; bg_g = g; bg_b = b;}
  void set_line_width(float f_width) {line_width_ = f_width;}
  float line_width() const { return line_width_; }

  //: Write 8 bit grey scale image.
  void print_greyscale_image(const unsigned char* data, int sizex, int sizey);
  //: Write 24 bit colour image.
  void print_color_image(const unsigned char* data, int sizex, int sizey);

  //:  Add a line between the given points to the Postscript file.
  void line(float x1, float y1, float x2, float y2);
  //: Add a point at the given coordinates to the Postscript file.
  void point(float x, float y, float point_size = 0);
  //: Add an ellipse to the Postscript file.
  void ellipse(float x, float y, float a_axis, float b_axis, int angle = 0);
  //: Add a circle with the given centre point and radius to the Postscript file.
  void circle(float x, float y, float radius);

  void reset_bounding_box();

 protected:
  void set_min_max_xy(float x, float y);
  void set_min_max_xy(int x, int y);
  bool set_parameters(int sizex, int sizey);

  //: PostScript file header.  Automatically called by the constructor.
  void postscript_header();

  //: Set graphic coordinate (translate and rotate to local coordinate).
  void graphic_header();
  //: Utility program used in point(), line(), ellipse() and circle()
  void sobj_rgb_params(char const* str, bool filled);
  //: the defined procedure for PostScript script use.
  void print_graphics_prolog();

 private:
  void compute_bounding_box();

  void reset_postscript_header();
  void image_translate_and_scale();
  void object_translate_and_scale();
  void done();

  std::ofstream output_filestream;

  float fg_r, fg_g, fg_b;
  float bg_r, bg_g, bg_b;
  float line_width_;
  float scale_x, scale_y;
  int ox, oy, iw, ih;
  double iwf, ihf;
  double psizex, psizey;   /* current paper size, in inches */
  double pos_inx, pos_iny; /* top-left offset of image, in inches */
  int width, height;       /* image width and height */
  std::string filename;     /* postscript path/filename */
  paper_type printer_paper_type;
  paper_orientation printer_paper_orientation;
  paper_layout printer_paper_layout;
  int reduction_factor;
  bool doneps;
  int min_x, min_y;
  int max_x, max_y;
  int box_width, box_height;

 private: /*even more*/

  std::streampos translate_pos;
  std::streampos sobj_t_pos;
  std::streampos header_pos;

  bool graphics_prolog_exists;
  bool exist_image;
  bool exist_objs;
};

#endif // vul_psfile_h_
