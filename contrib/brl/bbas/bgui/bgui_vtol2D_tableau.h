// This is brl/bbas/bgui/bgui_vtol2D_tableau.h
#ifndef bgui_vtol2D_tableau_h_
#define bgui_vtol2D_tableau_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A child tableau of vgui_easy2D_tableau that knows how to display vtol objects.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 28, 2002    Initial version.
//   J.L.Mundy December 16, 2002    Added map between soviews and vtol objects
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vgui/vgui_style.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vgui/vgui_tableau_sptr.h>
#include <bgui/bgui_style_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>

class bgui_vtol_soview2D_point;
class bgui_vtol_soview2D_digital_curve;
class bgui_vtol_soview2D_vertex;
class bgui_vtol_soview2D_edge;
class bgui_vtol_soview2D_edge_group;
class bgui_vtol_soview2D_face;

class bgui_vtol2D_tableau : public vgui_easy2D_tableau
{
 public:
  bgui_vtol2D_tableau(const char* n="unnamed");

  bgui_vtol2D_tableau(vgui_image_tableau_sptr const& it,
                      const char* n="unnamed");

  bgui_vtol2D_tableau(vgui_tableau_sptr const& t,
                      const char* n="unnamed");

  ~bgui_vtol2D_tableau();

  //:virtual handle method for events
  virtual bool handle(vgui_event const &);

  //: display for vsol_point_2d  (not vtol but often useful)
  bgui_vtol_soview2D_point* 
    add_vsol_point_2d(vsol_point_2d_sptr const& p);

  //: display for digital_curve (not vtol but often useful)
  bgui_vtol_soview2D_digital_curve* 
    add_digital_curve(vdgl_digital_curve_sptr const& dc);

  //: the vtol display methods for individual topology classes
  bgui_vtol_soview2D_vertex* add_vertex(vtol_vertex_2d_sptr const& v);
  bgui_vtol_soview2D_edge* add_edge(vtol_edge_2d_sptr const& e);

  bgui_vtol_soview2D_edge_group* add_edge_group(vcl_vector<vtol_edge_2d_sptr>&
                                                edges);
  bgui_vtol_soview2D_face* add_face(vtol_face_2d_sptr const& f);

  //: display methods for vectors of topology classes (not grouped)

  void add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos);

  void add_topology_objects(vcl_vector<vtol_topology_object_sptr> const& tos);

  void add_edges(vcl_vector<vtol_edge_2d_sptr> const & edges,
                 bool verts=false);

  void add_faces(vcl_vector<vtol_face_2d_sptr> const & faces, bool verts=false);

  //: clear the tableau including the highlight map
  void clear_all();

  //: Methods for getting mapped objects
  void enable_highlight(){highlight_ = true;}
  void disable_highlight(){highlight_ = false;}
  vtol_edge_2d_sptr get_mapped_edge(const int id);

  //: Methods for changing the default style of displayable objects
  void set_vsol_point_2d(const float r, const float g, const float b,
                               const point_radius);
  void set_digital_curve_style(const float r, const float g, const float b,
                               const line_width);
  void set_vertex_style(const float r, const float g, const float b,
                               const point_radius);
  void set_edge_style(const float r, const float g, const float b,
                      const line_width);
  void set_face_style(const float r, const float g, const float b,
                      const line_width);
 protected:
  bool highlight_;
  void init();
  vcl_map<int, vtol_topology_object_sptr> obj_map_;
  vcl_map<vcl_string, bgui_style_sptr> style_map_;
  int old_id_;
  bgui_style_sptr highlight_style_;
  bgui_style_sptr old_style_;
};

//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from xcv_image_tableau
struct bgui_vtol2D_tableau_new : public bgui_vtol2D_tableau_sptr
{
  typedef bgui_vtol2D_tableau_sptr base;

  bgui_vtol2D_tableau_new(const char* n="unnamed") :
    base(new bgui_vtol2D_tableau(n)) { }
  bgui_vtol2D_tableau_new(vgui_image_tableau_sptr const& it,
                          const char* n="unnamed") :
    base(new bgui_vtol2D_tableau(it,n)) { }

  bgui_vtol2D_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed")
    :  base(new bgui_vtol2D_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_vtol2D_tableau_h_
