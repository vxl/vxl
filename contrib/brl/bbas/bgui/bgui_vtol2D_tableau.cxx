#include "bgui_vtol2D_tableau.h"
//:
// \file

#include <bgui/bgui_vtol_soview2D.h>
#include <vgui/vgui.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_style.h>
#include <vtol/vtol_face_2d.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>

bgui_vtol2D_tableau::bgui_vtol2D_tableau(const char* n) :
  bgui_vsol2D_tableau(n) { this->init(); }

bgui_vtol2D_tableau::bgui_vtol2D_tableau(vgui_image_tableau_sptr const& it,
                                         const char* n) :
  bgui_vsol2D_tableau(it, n) { this->init(); }

bgui_vtol2D_tableau::bgui_vtol2D_tableau(vgui_tableau_sptr const& t,
                                         const char* n) :
  bgui_vsol2D_tableau(t, n) { this->init(); }

bgui_vtol2D_tableau::~bgui_vtol2D_tableau()
{
}

#ifdef DEBUG
static void print_edgels(vtol_edge_2d_sptr const & e)
{
  vsol_curve_2d_sptr c = e->curve();
  if (!c) return;
  vdgl_digital_curve_sptr dc = c->cast_to_digital_curve();
  if (!dc) return;
  vdgl_interpolator_sptr trp = dc->get_interpolator();
  if (!trp) return;
  vdgl_edgel_chain_sptr ec = trp->get_edgel_chain();
  if (!ec)
    return;
  int N = ec->size();
  for (int i = 0; i<N; i++)
    vcl_cout << "egl(" << i << ")=" << (*ec)[i] << '\n';
}
#endif

void bgui_vtol2D_tableau::init()
{
  //define default soview styles
  //these can be overridden by later set_*_syle commands prior to drawing.
  //
   this->set_vertex_style(1.0f, 0.0f, 0.0f, 3.0f);
   this->set_edge_style(0.0f, 1.0f, 0.0f, 3.0f);
   this->set_edge_group_style(0.0f, 1.0f, 0.0f, 3.0f);
   this->set_face_style(0.0f, 1.0f, 0.0f, 3.0f);

	//call the init() function of bgui_vsol2D_tableau
	bgui_vsol2D_tableau::init();
}

//display topological objects
bgui_vtol_soview2D_vertex*
bgui_vtol2D_tableau::add_vertex(vtol_vertex_2d_sptr const& v)
{
  bgui_vtol_soview2D_vertex* obj = new bgui_vtol_soview2D_vertex();
  obj->x = v->x();
  obj->y = v->y();
  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  if (obj)
    {
      obj->set_style(sty.ptr());
      if (highlight_)
        {
          int id = obj->get_id();
          obj_map_[id]=v->cast_to_topology_object();
        }
    }
  return obj;
}

bgui_vtol_soview2D_edge*
bgui_vtol2D_tableau::add_edge(vtol_edge_2d_sptr const& e)
{
#ifdef DEBUG
  print_edgels(e);
#endif
  bgui_vtol_soview2D_edge* obj = new bgui_vtol_soview2D_edge(e);
  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  if (obj)
    {
      obj->set_style(sty.ptr());
      if (highlight_)
        {
          int id = obj->get_id();
        obj_map_[id]=e->cast_to_topology_object();
        }
    }
  return obj;
}

bgui_vtol_soview2D_edge_group*
bgui_vtol2D_tableau::add_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  bgui_vtol_soview2D_edge_group* obj =
    new bgui_vtol_soview2D_edge_group(edges);
  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  obj->set_style(sty.ptr());
  return obj;
}

bgui_vtol_soview2D_face*
bgui_vtol2D_tableau::add_face(vtol_face_2d_sptr const& f)
{
  bgui_vtol_soview2D_face* obj = new bgui_vtol_soview2D_face(f);

  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  if (obj)
    {
      obj->set_style(sty.ptr());
      if (highlight_)
        {
          int id = obj->get_id();
          obj_map_[id]=f->cast_to_topology_object();
        }
    }
  return obj;
}

//--------------------------------------------------------------
// Add a list of generic topology objects
//
void bgui_vtol2D_tableau::
add_topology_objects(vcl_vector<vtol_topology_object_sptr> const& tos)
{
  for (vcl_vector<vtol_topology_object_sptr>::const_iterator tot = tos.begin();
       tot != tos.end(); tot++)
    {
      add_topology_object((*tot));
    }
}

void bgui_vtol2D_tableau::
add_topology_object(vtol_topology_object_sptr const& tos)
{
  if (tos->cast_to_vertex()) {
    if (tos->cast_to_vertex()->cast_to_vertex_2d())
      {
        vtol_vertex_2d_sptr v =
          tos->cast_to_vertex()->cast_to_vertex_2d();
        this->add_vertex(v);
      }
  } else if (tos->cast_to_edge()) {
    if (tos->cast_to_edge()->cast_to_edge_2d())
      {
        vtol_edge_2d_sptr e =
          tos->cast_to_edge()->cast_to_edge_2d();
        this->add_edge(e);
      }
  } else if (tos->cast_to_face()) {
    if (tos->cast_to_face()->cast_to_face_2d())
      {
        vtol_face_2d_sptr f =
          tos->cast_to_face()->cast_to_face_2d();
        this->add_face(f);
      }
  }
}

void bgui_vtol2D_tableau::add_edges(vcl_vector<vtol_edge_2d_sptr> const& edges,
                                    bool verts)
{
  for (vcl_vector<vtol_edge_2d_sptr>::const_iterator eit = edges.begin();
       eit != edges.end(); eit++)
    {
      this->add_edge(*eit);
      //optionally display the edge vertices
      if (verts)
        {
          vcl_vector<vtol_vertex_sptr>* vts = (*eit)->vertices();
          for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
               vit != vts->end(); vit++)
            {
              vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
              this->add_vertex(v);
            }
          delete vts;
        }
    }
}

void
bgui_vtol2D_tableau::add_faces(vcl_vector<vtol_face_2d_sptr> const& faces,
                               bool verts)
{
  for (vcl_vector<vtol_face_2d_sptr>::const_iterator fit = faces.begin();
       fit != faces.end(); fit++)
    {
      vtol_face_2d_sptr f = (*fit);
      this->add_face(f);
      if (verts)
        {
          vcl_vector<vtol_vertex_sptr>* vts = f->vertices();
          for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
               vit != vts->end(); vit++)
            {
              vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
              this->add_vertex(v);
            }
          delete vts;
        }
    }
}

vtol_edge_2d_sptr bgui_vtol2D_tableau::get_mapped_edge(const int id)
{
  vtol_topology_object_sptr to = obj_map_[id];
  if (!to)
    {
      vcl_cout << "In bgui_vtol2D_tableau::get_mapped_edge(..) - null map entry\n";
      return 0;
    }
  return to->cast_to_edge()->cast_to_edge_2d();
}

void bgui_vtol2D_tableau::clear_all()
{
  obj_map_.clear();
  //now call the clear_all from bgui_vsol2D_tableau

  bgui_vsol2D_tableau::clear_all();
}

void bgui_vtol2D_tableau::set_vtol_topology_object_style(vtol_topology_object_sptr tos,
                                                         const float r, const float g, const float b,
                                                         const float line_width,
                                                         const float point_radius)
{
  if (tos->cast_to_vertex()) {
    set_vertex_style(r,g,b, point_radius);
  } else if (tos->cast_to_edge()) {
    set_edge_style(r,g,b, line_width);
    set_edge_group_style(r,g,b, line_width);
  } else if (tos->cast_to_face()) {
    set_face_style(r,g,b, line_width);
  }
}

void bgui_vtol2D_tableau::set_vertex_style(const float r, const float g,
                                           const float b,
                                           const float point_radius)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, point_radius, 0.0f);
  bgui_vtol_soview2D_vertex sv;
  style_map_[sv.type_name()]=sty;
}
void bgui_vtol2D_tableau::set_edge_style(const float r, const float g,
                                         const float b, const float line_width)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vtol_soview2D_edge se;
  style_map_[se.type_name()]=sty;
}

void bgui_vtol2D_tableau::set_edge_group_style(const float r, const float g,
                                               const float b,
                                               const float line_width)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vtol_soview2D_edge_group sg;
  style_map_[sg.type_name()]=sty;
}

void bgui_vtol2D_tableau::set_face_style(const float r, const float g,
                                         const float b, const float line_width)
{
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vtol_soview2D_face sf;
  style_map_[sf.type_name()]=sty;
}
