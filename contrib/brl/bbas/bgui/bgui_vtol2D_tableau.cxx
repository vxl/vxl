#include <bgui/bgui_vtol_soview2D.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <vtol/vtol_face_2d.h>

bgui_vtol2D_tableau::bgui_vtol2D_tableau(const char* n):
  vgui_easy2D_tableau(n){}

bgui_vtol2D_tableau::bgui_vtol2D_tableau(vgui_image_tableau_sptr const& it,
                                         const char* n):
  vgui_easy2D_tableau(it, n){}

bgui_vtol2D_tableau::bgui_vtol2D_tableau(vgui_tableau_sptr const& t,
                                         const char* n):
  vgui_easy2D_tableau(t, n){}

bgui_vtol_soview2D_vertex* bgui_vtol2D_tableau::add_vertex(vtol_vertex_2d_sptr& v)
{
  bgui_vtol_soview2D_vertex* obj = new bgui_vtol_soview2D_vertex();
  obj->x = v->x();
  obj->y = v->y();
  add(obj);
  return obj;
}

bgui_vtol_soview2D_edge* bgui_vtol2D_tableau::add_edge(vtol_edge_2d_sptr& e)
{
  bgui_vtol_soview2D_edge* obj = new bgui_vtol_soview2D_edge(e);
  add(obj);
  return obj;
}

bgui_vtol_soview2D_edge_group*
bgui_vtol2D_tableau::add_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  bgui_vtol_soview2D_edge_group* obj =
    new bgui_vtol_soview2D_edge_group(edges);
  add(obj);
  return obj;
}

bgui_vtol_soview2D_face* bgui_vtol2D_tableau::add_face(vtol_face_2d_sptr& f)
{
  bgui_vtol_soview2D_face* obj = new bgui_vtol_soview2D_face(f);
  add(obj);
  return obj;
}


void bgui_vtol2D_tableau::add_edges(vcl_vector<vtol_edge_2d_sptr>& edges,
                                    bool verts)
{
  this->set_line_width(3.0);
  this->set_point_radius(5.0);

  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    {
      this->set_foreground(0.5,0.5,0.0);
      this->add_edge(*eit);
      //optionally display the edge vertices
      if (verts)
        {
          this->set_foreground(1.0,0.0,0.0);
          vcl_vector<vtol_vertex_sptr>* vts = (*eit)->vertices();
          for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
                vit != vts->end(); vit++)
            {
              vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
              this->set_foreground(1.0, 0, 0.0);
              this->add_vertex(v);
            }
          delete vts;
        }
    }
}

void 
bgui_vtol2D_tableau::add_faces(vcl_vector<vtol_face_2d_sptr>& faces,
                               bool verts)
{
   for (vcl_vector<vtol_face_2d_sptr>::iterator fit = faces.begin();
        fit != faces.end(); fit++)
     {
       vtol_face_2d_sptr f = (*fit);
       this->set_foreground(0.0, 1.0, 0.0);
       this->add_face(f);
       if (verts)
         {
           vcl_vector<vtol_vertex_sptr>* vts = f->vertices();
           for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
                vit != vts->end(); vit++)
             {
               vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
               this->set_foreground(1.0, 0, 0.0);
               this->add_vertex(v);
             }
           delete vts;
         }
     }

}
