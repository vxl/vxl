#ifndef vgui_easy2D_h_
#define vgui_easy2D_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_easy2D - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_easy2D.h
// .FILE vgui_easy2D.cxx
//
// .SECTION Description
// vgui_easy2D is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 24 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//    20-JUL-2000   Marko Bacic, Oxford RRG -- Provided support for printing linestrips
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_displaylist2D.h>
#include <vgui/vgui_slot.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_ref.h>

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
  vgui_easy2D(vgui_image_tableau_ref const&, const char* n="unnamed");

  bool handle(const vgui_event& e);

  vcl_string file_name() const;
  vcl_string pretty_name() const;
  vcl_string type_name() const;

  void set_image(vcl_string const& image);
  void set_child(vgui_tableau_ref const&);
  
  void set_foreground(float, float, float);
  void set_line_width(float);
  void set_point_radius(float);
  
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

  vgui_image_tableau_ref get_image_tableau();
  void print_psfile(vcl_string name, int reduction_factor, bool print_goem_objs);

protected:
  ~vgui_easy2D() { }

  vgui_slot image_slot;
  vgui_image_tableau_ref image_image;

  vcl_string name_;
  float fg[3];
  float line_width;
  float point_size;
};

struct vgui_easy2D_new : public vgui_easy2D_ref {
  vgui_easy2D_new(char const *n="unnamed") : 
    vgui_easy2D_ref(new vgui_easy2D(n)) { }
  
  vgui_easy2D_new(vgui_image_tableau_ref const& i, char const* n="unnamed") :
    vgui_easy2D_ref(new vgui_easy2D(i, n)) { }
};

#endif // vgui_easy2D_h_
