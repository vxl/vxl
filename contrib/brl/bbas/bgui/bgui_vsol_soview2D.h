// This is brl/bbas/bgui/bgui_vsol_soview2D.h
#ifndef bgui_vsol_soview2D_h_
#define bgui_vsol_soview2D_h_
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

#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>

#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_style.h>

//: This view is essentially the same as a point, the only difference is style.
// Probably move to a geometry tableau later
class bgui_vsol_soview2D_point : public vgui_soview2D_point
{
 public:
  //: Constructor - creates a default vsol_point_2d view
  bgui_vsol_soview2D_point(){}

  //: Print details about this vtol_point to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vsol_soview2D_point').
  vcl_string type_name() const { return "bgui_vsol_soview2D_point"; }
};

//: vsol_line_2d
class bgui_vsol_soview2D_line_seg : public vgui_soview2D_lineseg
{
 public:
  //: Constructor - creates a default edge_2d view
  bgui_vsol_soview2D_line_seg() {}

  //: Constructor - creates a view of a given vtol_edge_2d
  bgui_vsol_soview2D_line_seg(vsol_line_2d_sptr const& seg);

  //: Returns the type of this class ('bgui_vsol_soview2D_edge').
  vcl_string type_name() const { return "bgui_vsol_soview2D_line_seg"; }
};

//: vsol_polyline_2d
class bgui_vsol_soview2D_polyline : public vgui_soview2D_linestrip
{
 public:
  //: Constructor - creates a default edge_2d view
  bgui_vsol_soview2D_polyline() {}

  //: Constructor - creates a view of a given vsol_polyline_2d
  bgui_vsol_soview2D_polyline(vsol_polyline_2d_sptr const& pline);

  //: Print details about this vtol_edge_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vsol_soview2D_polyline').
  vcl_string type_name() const { return "bgui_vsol_soview2D_polyline"; }
};

//: vsol_polygon_2d
class bgui_vsol_soview2D_polygon : public vgui_soview2D_polygon
{
 public:
  //: Constructor - creates a default vdgl_polygon view
  bgui_vsol_soview2D_polygon() {;}

  //: Constructor - creates a view of a vdgl_polygon
  bgui_vsol_soview2D_polygon(vsol_polygon_2d_sptr const& e);

  //: Print details about this vdgl_polygon to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vsol_soview2D_polygon').
  vcl_string type_name() const { return "bgui_vsol_soview2D_polygon"; }
};

//: a group of vsol_line_2d - used for projecting 3-d shapes
class bgui_vsol_soview2D_line_group : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vtol_edge_2d_group view
  bgui_vsol_soview2D_line_group() {}

  //: Constructor - creates a view of a vtol_edge_2d group
  bgui_vsol_soview2D_line_group(vcl_vector<vsol_line_2d_sptr>& edges);

  //: Print details about this vtol_edge_2d  group to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vsol_soview2D_line_group').
  vcl_string type_name() const { return "bgui_vsol_soview2D_line_group"; }
};

//: vdgl_dotted_digital_curve
// This one displays points
class bgui_vsol_soview2D_dotted_digital_curve : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vdgl_digital_curve view
  bgui_vsol_soview2D_dotted_digital_curve() {;}

  //: Constructor - creates a view of a vdgl_digital_curve
  bgui_vsol_soview2D_dotted_digital_curve(vdgl_digital_curve_sptr const& e);

  //: Print details about this vdgl_digital_curve to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vsol_soview2D_dotted_digital_curve').
  vcl_string type_name() const { return "bgui_vsol_soview2D_dotted_digital_curve"; }
};

//: vdgl_digital_curve
class bgui_vsol_soview2D_digital_curve : public vgui_soview2D_linestrip
{
 public:
  //: Constructor - creates a default vdgl_digital_curve view
  bgui_vsol_soview2D_digital_curve() {;}

  //: Constructor - creates a view of a vdgl_digital_curve
  bgui_vsol_soview2D_digital_curve(vdgl_digital_curve_sptr const& e);

  //: Print details about this vdgl_digital_curve to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vsol_soview2D_digital_curve').
  vcl_string type_name() const { return "bgui_vsol_soview2D_digital_curve"; }
};

//: a group of vsol_polygon_2d - used for projecting 3-d polyhedra
class bgui_vsol_soview2D_polygon_group : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vtol_edge_2d_group view
  bgui_vsol_soview2D_polygon_group() {}

  //: Constructor - creates a view of a vtol_edge_2d group
  bgui_vsol_soview2D_polygon_group(vcl_vector<vsol_polygon_2d_sptr>& edges);

  //: Print details about this vtol_edge_2d  group to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vsol_soview2D_polygon_group').
  vcl_string type_name() const { return "bgui_vsol_soview2D_polygon_group"; }
};

#endif // bgui_vsol_soview2D_h_
