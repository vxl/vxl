// This is brl/bbas/bgui/bgui_linked_vsol_soview2D.h
#ifndef bgui_linked_vsol_soview2D_h_
#define bgui_linked_vsol_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A collection of vgui_soview2D objects
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy June 2, 2003      Initial version.
//   Amir Tamrakar June 06, 2003  Added soview for vsol_polyline_2d.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_iosfwd.h>

#include <vdgl/vdgl_digital_curve_sptr.h>
#if 0 // currently not needed
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <vsol/vsol_triangle_2d_sptr.h>
#include <vsol/vsol_group_2d_sptr.h>
#endif

#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_style.h>

//: This view is essentially the same as a point, the only difference is style.
// Probably move to a geometry tableau later
/*class bgui_linked_vsol_soview2D_point : public vgui_soview2D_point
{
 public:
  //: Constructor - creates a default vsol_point_2d view
  bgui_linked_vsol_soview2D_point(){}

  //: Print details about this vtol_point to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_linked_vsol_soview2D_point').
  vcl_string type_name() const { return "bgui_linked_vsol_soview2D_point"; }
};

//: vsol_line_2d
class bgui_linked_vsol_soview2D_line_seg : public vgui_soview2D_lineseg
{
 public:
  //: Constructor - creates a default edge_2d view
  bgui_linked_vsol_soview2D_line_seg() {}

  //: Constructor - creates a view of a given vtol_edge_2d
  bgui_linked_vsol_soview2D_line_seg(vsol_line_2d_sptr const& seg);

  //: Returns the type of this class ('bgui_linked_vsol_soview2D_edge').
  vcl_string type_name() const { return "bgui_linked_vsol_soview2D_line_seg"; }
};

//: vsol_polyline_2d
class bgui_linked_vsol_soview2D_polyline : public vgui_soview2D_linestrip
{
 public:
  //: Constructor - creates a default edge_2d view
  bgui_linked_vsol_soview2D_polyline() {}

  //: Constructor - creates a view of a given vsol_polyline_2d
  bgui_linked_vsol_soview2D_polyline(vsol_polyline_2d_sptr const& pline);

  //: Print details about this vtol_edge_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_linked_vsol_soview2D_polyline').
  vcl_string type_name() const { return "bgui_linked_vsol_soview2D_polyline"; }
};

//: vsol_polygon_2d
class bgui_linked_vsol_soview2D_polygon : public vgui_soview2D_polygon
{
 public:
  //: Constructor - creates a default vdgl_polygon view
  bgui_linked_vsol_soview2D_polygon() {;}

  //: Constructor - creates a view of a vdgl_polygon
  bgui_linked_vsol_soview2D_polygon(vsol_polygon_2d_sptr const& e);

  //: Print details about this vdgl_polygon to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_linked_vsol_soview2D_polygon').
  vcl_string type_name() const { return "bgui_linked_vsol_soview2D_polygon"; }
};

//: a group of vsol_line_2d - used for projecting 3-d shapes
class bgui_linked_vsol_soview2D_line_group : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vtol_edge_2d_group view
  bgui_linked_vsol_soview2D_line_group() {}

  //: Constructor - creates a view of a vtol_edge_2d group
  bgui_linked_vsol_soview2D_line_group(vcl_vector<vsol_line_2d_sptr>& edges);

  //: Print details about this vtol_edge_2d  group to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_linked_vsol_soview2D_line_group').
  vcl_string type_name() const { return "bgui_linked_vsol_soview2D_line_group"; }
};

//: vdgl_dotted_digital_curve
// This one displays points
class bgui_linked_vsol_soview2D_dotted_digital_curve : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vdgl_digital_curve view
  bgui_linked_vsol_soview2D_dotted_digital_curve() {;}

  //: Constructor - creates a view of a vdgl_digital_curve
  bgui_linked_vsol_soview2D_dotted_digital_curve(vdgl_digital_curve_sptr const& e);

  //: Print details about this vdgl_digital_curve to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_linked_vsol_soview2D_dotted_digital_curve').
  vcl_string type_name() const { return "bgui_linked_vsol_soview2D_dotted_digital_curve"; }
};
*/
//: vdgl_digital_curve
class bgui_linked_vsol_soview2D_digital_curve : public vgui_soview2D
{
 public:
  //: Constructor - creates a view of a vdgl_digital_curve
  bgui_linked_vsol_soview2D_digital_curve(vdgl_digital_curve_sptr const& e)
  { sptr = e; };

  //: Destructor - does nothing, smart pointers pass out of scope automatically
  ~bgui_linked_vsol_soview2D_digital_curve() {};
  
  //: Print details about this vdgl_digital_curve to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_linked_vsol_soview2D_digital_curve').
  vcl_string type_name() const { return "bgui_linked_vsol_soview2D_digital_curve"; };

  //: Render this 2D digital_curve on the display.
  void draw() const;

  //: Returns the distance squared from this 2D digital_curve to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid of this 2D digital_curve.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D digital_curve by the given x and y distances.
  void translate(float x, float y);

  //: Smart pointer to vdgl_digital_curve
  vdgl_digital_curve_sptr sptr;
};
/*
//: a group of vsol_polygon_2d - used for projecting 3-d polyhedra
class bgui_linked_vsol_soview2D_polygon_group : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vtol_edge_2d_group view
  bgui_linked_vsol_soview2D_polygon_group() {}

  //: Constructor - creates a view of a vtol_edge_2d group
  bgui_linked_vsol_soview2D_polygon_group(vcl_vector<vsol_polygon_2d_sptr>& edges);

  //: Print details about this vtol_edge_2d  group to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_linked_vsol_soview2D_polygon_group').
  vcl_string type_name() const { return "bgui_linked_vsol_soview2D_polygon_group"; }
};
*/
#endif // bgui_linked_vsol_soview2D_h_
