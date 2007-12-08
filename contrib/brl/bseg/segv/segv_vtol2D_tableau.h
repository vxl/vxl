// This is brl/bseg/segv/segv_vtol2D_tableau.h
#ifndef segv_vtol2D_tableau_h_
#define segv_vtol2D_tableau_h_
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
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>

#include "segv_vtol2D_tableau_sptr.h"

class segv_vtol_soview2D_vertex;
class segv_vtol_soview2D_edge;
class segv_vtol_soview2D_edge_group;
class segv_vtol_soview2D_face;

class segv_vtol2D_tableau : public vgui_easy2D_tableau
{
 public:
  segv_vtol2D_tableau(const char* n="unnamed");

  segv_vtol2D_tableau(vgui_image_tableau_sptr const& it,
                      const char* n="unnamed");

  segv_vtol2D_tableau(vgui_tableau_sptr const& t,
                      const char* n="unnamed");

  ~segv_vtol2D_tableau(){}

  //: the vtol display methods
  segv_vtol_soview2D_vertex* add_vertex(vtol_vertex_2d_sptr& v);
  segv_vtol_soview2D_edge* add_edge(vtol_edge_2d_sptr& e);

  segv_vtol_soview2D_edge_group* add_edge_group(vcl_vector<vtol_edge_2d_sptr>&
                                                edges);
  segv_vtol_soview2D_face* add_face(vtol_face_2d_sptr& f);
};

//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from xcv_image_tableau
struct segv_vtol2D_tableau_new : public segv_vtol2D_tableau_sptr
{
  typedef segv_vtol2D_tableau_sptr base;

  segv_vtol2D_tableau_new(const char* n="unnamed") :
    base(new segv_vtol2D_tableau(n)) { }
  segv_vtol2D_tableau_new(vgui_image_tableau_sptr const& it,
                          const char* n="unnamed") :
    base(new segv_vtol2D_tableau(it,n)) { }

  segv_vtol2D_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed")
    :  base(new segv_vtol2D_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // segv_vtol2D_tableau_h_
