// This is brl/bseg/segv/segv_vtol_soview2D.h
#ifndef segv_vtol_soview2D_h_
#define segv_vtol_soview2D_h_
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
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vgui/vgui_soview2D.h>

//: vtol_vertex_2d
// This view is essentially the same as a point the only difference
// would be style, which we will tackle later
class segv_vtol_soview2D_vertex : public vgui_soview2D_point
{
 public:
  //: Constructor - creates a default vertex_2d view
  segv_vtol_soview2D_vertex() {}

  //: Print details about this vtol_vertex_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('segv_vtol_soview2D_vertex').
  vcl_string type_name() const { return "segv_vtol_soview2D_vertex"; }
};

class segv_vtol_soview2D_edge : public vgui_soview2D_linestrip
{
 public:
  //: Constructor - creates a default edge_2d view
  segv_vtol_soview2D_edge() {}

  //: Constructor - creates a view of a vtol_vertex_2d
  segv_vtol_soview2D_edge(vtol_edge_2d_sptr const& e);

  //: Print details about this vtol_vertex_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('segv_vtol_soview2D_edge').
  vcl_string type_name() const { return "segv_vtol_soview2D_edge"; }
};

class segv_vtol_soview2D_edge_group : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default edge_2d view
  segv_vtol_soview2D_edge_group() {}

  //: Constructor - creates a view of a vtol_vertex_2d
  segv_vtol_soview2D_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges);

  //: Print details about this vtol_vertex_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('segv_vtol_soview2D_edge_group').
  vcl_string type_name() const { return "segv_vtol_soview2D_edge_group"; }
};

class segv_vtol_soview2D_face : public vgui_soview2D_group
{
 public:
  //: Constructor - creates a default edge_2d view
  segv_vtol_soview2D_face() {}

  //: Constructor - creates a view of a vtol_vertex_2d
  segv_vtol_soview2D_face(vtol_face_2d_sptr& f);

  //: Print details about this vtol_vertex_2d to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('segv_vtol_soview2D_face').
  vcl_string type_name() const { return "segv_vtol_soview2D_face"; }
};

#endif // segv_vtol_soview2D_h_
