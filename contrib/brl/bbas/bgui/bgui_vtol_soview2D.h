//this-sets-emacs-to-*-c++-*-mode
#ifndef bgui_vtol_soview2D_h_
#define bgui_vtol_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A collection of vgui_soview2D objects that form views of vtol topology objects
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 28, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vgui/vgui_soview2D.h>

//: vsol_point_2d
// This view is essentially the same as a point the only difference
// is style. Probably move to a geometry tableau later
class bgui_vtol_soview2D_point : public vgui_soview2D_point
{
 public:
  //: Constructor - creates a default vsol_point_2d view
  bgui_vtol_soview2D_point(){}

  //: Print details about this vtol_point to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vtol_soview2D_point').
  vcl_string type_name() const { return "bgui_vtol_soview2D_point"; }
};

//: vdgl_dotted_digital_curve (maybe later move to a geometry tableau)
// This one displays points
class bgui_vtol_soview2D_dotted_digital_curve : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vdgl_digital_curve view
  bgui_vtol_soview2D_dotted_digital_curve() {;}

  //: Constructor - creates a view of a vdgl_digital_curve
  bgui_vtol_soview2D_dotted_digital_curve(vdgl_digital_curve_sptr const& e);

  //: Print details about this vdgl_digital_curve to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vtol_soview2D_dotted_digital_curve').
  vcl_string type_name() const { return "bgui_vtol_soview2D_dotted_digital_curve"; }
};

//: vdgl_digital_curve (maybe later move to a geometry tableau)
class bgui_vtol_soview2D_digital_curve : public vgui_soview2D_linestrip
{
 public:
  //: Constructor - creates a default vdgl_digital_curve view
  bgui_vtol_soview2D_digital_curve() {;}

  //: Constructor - creates a view of a vdgl_digital_curve
  bgui_vtol_soview2D_digital_curve(vdgl_digital_curve_sptr const& e);

  //: Print details about this vdgl_digital_curve to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vtol_soview2D_digital_curve').
  vcl_string type_name() const { return "bgui_vtol_soview2D_digital_curve"; }
};

//: vtol_vertex_2d
// This view is essentially the same as a point the only difference
// would be style, which we will tackle later
class bgui_vtol_soview2D_vertex : public vgui_soview2D_point
{
 public:
  //: Constructor - creates a default vertex_2d view
  bgui_vtol_soview2D_vertex(){}

  //: Print details about this vtol_vertex_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vtol_soview2D_vertex').
  vcl_string type_name() const { return "bgui_vtol_soview2D_vertex"; }
};

class bgui_vtol_soview2D_line_seg : public vgui_soview2D_lineseg
{
 public:
  //: Constructor - creates a default edge_2d view
  bgui_vtol_soview2D_line_seg() {}

  //: Constructor - creates a view of a given vtol_edge_2d
  bgui_vtol_soview2D_line_seg(vsol_line_2d_sptr const& seg);

  //: Returns the type of this class ('bgui_vtol_soview2D_edge').
  vcl_string type_name() const { return "bgui_vtol_soview2D_line_seg"; }
};

class bgui_vtol_soview2D_edge : public vgui_soview2D_linestrip
{
 public:
  //: Constructor - creates a default edge_2d view
  bgui_vtol_soview2D_edge() {}

  //: Constructor - creates a view of a given vtol_edge_2d
  bgui_vtol_soview2D_edge(vtol_edge_2d_sptr const& e);

  //: Print details about this vtol_edge_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vtol_soview2D_edge').
  vcl_string type_name() const { return "bgui_vtol_soview2D_edge"; }
};

class bgui_vtol_soview2D_edge_group : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vtol_edge_2d_group view
  bgui_vtol_soview2D_edge_group() {}

  //: Constructor - creates a view of a vtol_edge_2d group
  bgui_vtol_soview2D_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges);

  //: Print details about this vtol_edge_2d  group to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vtol_soview2D_edge_group').
  vcl_string type_name() const { return "bgui_vtol_soview2D_edge_group"; }
};

class bgui_vtol_soview2D_face : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default vtol_face_2d view
  bgui_vtol_soview2D_face() {}

  //: Constructor - creates a view of a vtol_face_2d
  bgui_vtol_soview2D_face(vtol_face_2d_sptr const& f);

  //: Print details about this vtol_face_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vtol_soview2D_face').
  vcl_string type_name() const { return "bgui_vtol_soview2D_face"; }
};

#endif // bgui_vtol_soview2D_h_
