#ifndef vul_psfile_h_
#define vul_psfile_h_

// This is vxl/vul/vul_psfile.h

//:
// \file

#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>

//: Write a PostScript file
class vul_psfile: public vcl_ofstream
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
  ~vul_psfile();
  operator bool () { return ((void *)output_filestream!=0); }

  void set_min_max_xy(float x, float y);
  void set_min_max_xy(int x, int y);
  bool set_parameters(int sizex, int sizey);
  void set_paper_type(vul_psfile::paper_type type){printer_paper_type = type;}
  void set_paper_layout(vul_psfile::paper_layout layout) {printer_paper_layout = layout;}
  void set_paper_orientation(vul_psfile::paper_orientation o) {printer_paper_orientation = o;}
  void set_reduction_factor(int rf) {reduction_factor = rf;}
  void set_scale_x(int sx) {scale_x = sx;}
  void set_scale_y(int sy) {scale_x = sy;}
  void set_fg_color(float r, float g, float b) {fg_r = r; fg_g = g; fg_b = b;}
  void set_bg_color(float r, float g, float b) {bg_r = r; bg_g = g; bg_b = b;}
  void set_line_width(float f_width) {line_width = f_width;}

  typedef unsigned char byte;
  void print_greyscale_image(byte* data, int sizex, int sizey);
  void print_color_image(byte* data, int sizex, int sizey);

  void postscript_header();
  void graphic_header();
  void print_graphics_prolog();
  void sobj_rgb_params(char const* str, bool filled);

  void line(float x1, float y1, float x2, float y2);
  void point(float x, float y, float point_size = 0);
  void ellipse(float x, float y, float a_axis, float b_axis, int angle = 0);
  void circle(float x, float y, float radius);

private:
  void compute_bounding_box();
  void reset_bounding_box();
  void reset_postscript_header();
  void image_translate_and_scale();
  void object_translate_and_scale();
  void done();

  vcl_ofstream output_filestream;

  float fg_r, fg_g, fg_b;
  float bg_r, bg_g, bg_b;
  float line_width;
  int scale_x, scale_y;
  int ox, oy, iw, ih;
  double iwf, ihf;
  double psizex, psizey;   /* current paper size, in inches */
  double pos_inx, pos_iny; /* top-left offset of image, in inches */
  int width, height;       /* image width and height */
  vcl_string filename;     /* postscript path/filename */
  paper_type printer_paper_type;
  paper_orientation printer_paper_orientation;
  paper_layout printer_paper_layout;
  int reduction_factor;
  bool doneps;
  int min_x, min_y;
  int max_x, max_y;
  int box_width, box_height;

private: /*even more*/

  vcl_streampos translate_pos;
  vcl_streampos sobj_t_pos;
  vcl_streampos header_pos;

  bool graphics_prolog_exists;
  bool exist_image;
  bool exist_objs;
};

#endif // vul_psfile_h_
