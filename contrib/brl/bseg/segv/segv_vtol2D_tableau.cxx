#include "segv_vtol2D_tableau.h"
//:
// \file
#include "segv_vtol_soview2D.h"

segv_vtol2D_tableau::segv_vtol2D_tableau(const char* n):
  vgui_easy2D_tableau(n){}

segv_vtol2D_tableau::segv_vtol2D_tableau(vgui_image_tableau_sptr const& it,
                                         const char* n):
  vgui_easy2D_tableau(it, n){}

segv_vtol2D_tableau::segv_vtol2D_tableau(vgui_tableau_sptr const& t,
                                         const char* n):
  vgui_easy2D_tableau(t, n){}

segv_vtol_soview2D_vertex* segv_vtol2D_tableau::add_vertex(vtol_vertex_2d_sptr& v)
{
  segv_vtol_soview2D_vertex* obj = new segv_vtol_soview2D_vertex();
  obj->x = v->x();
  obj->y = v->y();
  add(obj);
  return obj;
}

segv_vtol_soview2D_edge* segv_vtol2D_tableau::add_edge(vtol_edge_2d_sptr& e)
{
  segv_vtol_soview2D_edge* obj = new segv_vtol_soview2D_edge(e);
  add(obj);
  return obj;
}

segv_vtol_soview2D_edge_group*
segv_vtol2D_tableau::add_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  segv_vtol_soview2D_edge_group* obj =
    new segv_vtol_soview2D_edge_group(edges);
  add(obj);
  return obj;
}

segv_vtol_soview2D_face* segv_vtol2D_tableau::add_face(vtol_face_2d_sptr& f)
{
  segv_vtol_soview2D_face* obj = new segv_vtol_soview2D_face(f);
  add(obj);
  return obj;
}
