#include <vcl_iostream.h>
#include <segv/segv_vtol_soview2D.h>
#include <segv/segv_vtol2D_tableau.h>

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
