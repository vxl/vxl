// This is brl/bbas/bgui/bgui_vtol2D_tableau.h
#ifndef bgui_vtol2D_tableau_h_
#define bgui_vtol2D_tableau_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A child tableau of bgui_vsol2D_tableau that knows how to display vtol objects.
// \author
//   J.L. Mundy
//
//   Default styles are defined for each geometry and topology object soview.
//   Users can change the default style by using the set_*_style commands,
//   e.g. set_edge_style(0.0, 0.5, 0.5, 3) will define the style for each
//   new edge added to the display.  This default is the style that the
//   edge appearance will resume after being highlighted.
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 28, 2002    Initial version.
//   J.L. Mundy December 16, 2002    Added map between soviews and vtol objects
//   J.L. Mundy March 22, 2003       Added set style commands
//   Amir Tamrakar April 22, 2002    Removed the functions to display vsol objects
//                                   to bgui_vsol2D_tableau and subclasses this
//                                   from it instead
//   Mark Johnson June 13, 2003      Stopped using interior class functions to
//                                   highlight objects. Added support for
//                                   specifying colors of individual objects.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vgui/vgui_style.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <bgui/bgui_easy2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>

class bgui_vtol_soview2D_vertex;
class bgui_vtol_soview2D_edge;
class bgui_vtol_soview2D_edge_group;
class bgui_vtol_soview2D_face;

class bgui_vtol2D_tableau : public bgui_vsol2D_tableau
{
 public:
  bgui_vtol2D_tableau(const char* n="unnamed");

  bgui_vtol2D_tableau(vgui_image_tableau_sptr const& it,
                      const char* n="unnamed");

  bgui_vtol2D_tableau(vgui_tableau_sptr const& t,
                      const char* n="unnamed");

  ~bgui_vtol2D_tableau();

  //: the vtol display methods for individual topology classes
  bgui_vtol_soview2D_vertex* add_vertex(vtol_vertex_2d_sptr const& v);

  bgui_vtol_soview2D_vertex* add_vertex(vtol_vertex_2d_sptr const& v,
                                        const float r,
                                        const float g,
                                        const float b,
                                        const float point_radius );

  bgui_vtol_soview2D_edge* add_edge(vtol_edge_2d_sptr const& e);

  bgui_vtol_soview2D_edge* add_edge(vtol_edge_2d_sptr const& e,
                                    const float r,
                                    const float g,
                                    const float b,
                                    const float line_width );

  bgui_vtol_soview2D_edge_group* add_edge_group(vcl_vector<vtol_edge_2d_sptr>&
                                                edges);

  bgui_vtol_soview2D_edge_group* add_edge_group(vcl_vector<vtol_edge_2d_sptr>&
                                                edges,
                                                const float r,
                                                const float g,
                                                const float b,
                                                const float line_width);

  bgui_vtol_soview2D_face* add_face(vtol_face_2d_sptr const& f);

  bgui_vtol_soview2D_face* add_face(vtol_face_2d_sptr const& f,
                                    const float r,
                                    const float g,
                                    const float b,
                                    const float line_width);

  //: display methods for vectors of topology classes (not grouped)
  void add_topology_object(vtol_topology_object_sptr const& tos);

  void add_topology_object(vtol_topology_object_sptr const& tos,
                           const float r,
                           const float g,
                           const float b,
                           const float line_width,
                           const float point_radius);

  void add_topology_objects(vcl_vector<vtol_topology_object_sptr> const& tos);

  void add_topology_objects(vcl_vector<vtol_topology_object_sptr> const& tos,
                            const float r,
                            const float g,
                            const float b,
                            const float line_width,
                            const float point_radius);

  void add_edges(vcl_vector<vtol_edge_2d_sptr> const & edges,
                 bool verts=false);

  void add_edges(vcl_vector<vtol_edge_2d_sptr> const & edges,
                 bool verts,
                 const float r,
                 const float g,
                 const float b,
                 const float line_width,
                 const float point_radius);

  void add_faces(vcl_vector<vtol_face_2d_sptr> const & faces, bool verts=false);

  void add_faces(vcl_vector<vtol_face_2d_sptr> const & faces, bool verts,
                 const float r,
                 const float g,
                 const float b,
                 const float line_width,
                 const float point_radius);

  //: clear the tableau including the highlight map
  void clear_all();

  //: Methods for getting mapped objects
  //void enable_highlight(){highlight_ = true;}
  //void disable_highlight(){highlight_ = false;}
  vtol_edge_2d_sptr get_mapped_edge(const int id);

  //: Methods for changing the default style of displayable objects
  void set_vtol_topology_object_style(vtol_topology_object_sptr tos,
                                      const float r, const float g, const float b,
                                      const float line_width, const float point_radius);
  void set_vertex_style(const float r, const float g, const float b,
                        const float point_radius);
  void set_edge_style(const float r, const float g, const float b,
                      const float line_width);
  void set_edge_group_style(const float r, const float g, const float b,
                            const float line_width);
  void set_face_style(const float r, const float g, const float b,
                      const float line_width);
 protected:
  DefaultStyle vertex_style_;
  DefaultStyle edge_style_;
  DefaultStyle edge_group_style_;
  DefaultStyle face_style_;
  void init();
  vcl_map<int, vtol_topology_object_sptr> obj_map_;
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

  operator bgui_vsol2D_tableau_sptr () const { bgui_vsol2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // bgui_vtol2D_tableau_h_
