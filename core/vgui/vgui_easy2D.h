#ifndef vgui_easy2D_h_
#define vgui_easy2D_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
//
// \author
//              Philip C. Pritchett, 24 Sep 99
//              Robotics Research Group, University of Oxford
//
// \verbatim
// Modifications
//    20-JUL-2000   Marko Bacic, Oxford RRG -- Provided support for printing linestrips
//    25-APR-2002   domi@vision.ee.ethz.ch - make print_psfile work without image tableau
// \endverbatim
//-----------------------------------------------------------------------------

#include <vgui/vgui_displaylist2D.h>
#include <vgui/vgui_slot.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_sptr.h>

class vgui_soview2D;
class vgui_soview2D_point;
class vgui_soview2D_lineseg;
class vgui_soview2D_infinite_line;
class vgui_soview2D_circle;
class vgui_soview2D_linestrip;
class vgui_soview2D_polygon;

class vgui_easy2D : public vgui_displaylist2D
{
public:
  vgui_easy2D(const char* n="unnamed");
  vgui_easy2D(vgui_image_tableau_sptr const&, const char* n="unnamed");
  vgui_easy2D(vgui_tableau_sptr const&, const char* n="unnamed");

  bool handle(const vgui_event& e);

  vcl_string name() const { return name_; };
  vcl_string file_name() const;
  vcl_string pretty_name() const;
  vcl_string type_name() const;

  void set_image(vcl_string const& image);
  void set_child(vgui_tableau_sptr const&);

  void set_foreground(float r, float g, float b) { fg[0] = r; fg[1] = g; fg[2] = b; }
  void set_line_width(float w) { line_width = w; }
  void set_point_radius(float r) { point_size = r; }

  void add(vgui_soview2D*);

  vgui_soview2D_point* add_point(float x, float y);
  vgui_soview2D_lineseg* add_line(float x0, float y0, float x1, float y1);
  vgui_soview2D_infinite_line* add_infinite_line(float a, float b, float c); // ax + by + c = 0
  vgui_soview2D_circle* add_circle(float x, float y, float r);

  vgui_soview2D_point* add_point_3dv(double const p[3]);
  vgui_soview2D_lineseg* add_line_3dv_3dv(double const p1[3], double const p2[3]); // line segment
  vgui_soview2D_infinite_line* add_infinite_line_3dv(double const l[3]);
  vgui_soview2D_circle* add_circle_3dv(double const point[3], float r);

  vgui_soview2D_linestrip* add_linestrip(unsigned n, float const *x, float const *y);
  vgui_soview2D_polygon* add_polygon(unsigned n, float const *x, float const *y);

  vgui_image_tableau_sptr get_image_tableau() { return image_image; }
  //: screen dump to postscript file.
  //  Specify the optional arguments in case this tableau does not contain
  //  an image tableau, or if you want a smaller part of the image printed.
  //  If wd or ht are 0, no image is printed at all.
  void print_psfile(vcl_string filename, int reduction_factor,
                    bool print_geom_objs, int wd=-1, int ht=-1);

protected:
  ~vgui_easy2D() { }

  vgui_slot image_slot;
  vgui_image_tableau_sptr image_image;

  vcl_string name_;
  float fg[3];
  float line_width;
  float point_size;
};

struct vgui_easy2D_new : public vgui_easy2D_sptr {
  vgui_easy2D_new(char const *n="unnamed") :
    vgui_easy2D_sptr(new vgui_easy2D(n)) { }

  vgui_easy2D_new(vgui_image_tableau_sptr const& i, char const* n="unnamed") :
    vgui_easy2D_sptr(new vgui_easy2D(i, n)) { }

  vgui_easy2D_new(vgui_tableau_sptr const& i, char const* n="unnamed") :
    vgui_easy2D_sptr(new vgui_easy2D(i, n)) { }
};

#endif // vgui_easy2D_h_
