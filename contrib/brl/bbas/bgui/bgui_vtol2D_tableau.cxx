#include "bgui_vtol2D_tableau.h"
//:
// \file
#include <bgui/bgui_vtol_soview2D.h>
#include <vgui/vgui.h>
#include <vgui/vgui_style.h>
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
  this->clear_all();
}

#ifdef DEBUG
static void print_edgels(vtol_edge_2d_sptr const & e)
{
  vsol_curve_2d_sptr c = e->curve();
  if (!c) return;
  vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
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
  vertex_style_     = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 0.0f);
  edge_style_       = vgui_style::new_style(0.0f, 1.0f, 0.0f, 1.0f, 3.0f);
  edge_group_style_ = vgui_style::new_style(0.0f, 1.0f, 0.0f, 1.0f, 3.0f);
  face_style_       = vgui_style::new_style(0.0f, 1.0f, 0.0f, 1.0f, 3.0f);

  //call the init() function of bgui_vsol2D_tableau
  bgui_vsol2D_tableau::init();
}

bool bgui_vtol2D_tableau::handle(vgui_event const &e)
{
  // We aren't interested in other events so pass them to the base class.
  return bgui_vsol2D_tableau::handle(e);
}

//display topological objects
bgui_vtol_soview2D_vertex*
bgui_vtol2D_tableau::add_vertex(vtol_vertex_2d_sptr const& v,
                                const vgui_style_sptr& style)
{
  bgui_vtol_soview2D_vertex* obj = new bgui_vtol_soview2D_vertex();
  obj->x = v->x();
  obj->y = v->y();
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( vertex_style_ );

  if (obj) {
    int id = obj->get_id();
    obj_map_[id]=v->cast_to_topology_object();
  }
  return obj;
}


bgui_vtol_soview2D_edge*
bgui_vtol2D_tableau::add_edge(vtol_edge_2d_sptr const& e,
                              const vgui_style_sptr& style)
{
#ifdef DEBUG
  print_edgels(e);
#endif
  bgui_vtol_soview2D_edge* obj = new bgui_vtol_soview2D_edge(e);

  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( edge_style_ );

  if (obj) {
    int id = obj->get_id();
    obj_map_[id]=e->cast_to_topology_object();
  }
  return obj;
}


bgui_vtol_soview2D_edge_group*
bgui_vtol2D_tableau::add_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges,
                                    const vgui_style_sptr& style )
{
  bgui_vtol_soview2D_edge_group* obj =
    new bgui_vtol_soview2D_edge_group(edges);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( edge_group_style_ );

  return obj;
}


bgui_vtol_soview2D_face*
bgui_vtol2D_tableau::add_face(vtol_face_2d_sptr const& f,
                              const vgui_style_sptr& style)
{
  bgui_vtol_soview2D_face* obj = new bgui_vtol_soview2D_face(f);

  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( face_style_ );

  if (obj) {
    int id = obj->get_id();
    obj_map_[id]=f->cast_to_topology_object();
  }
  return obj;
}

//--------------------------------------------------------------
// Add a list of generic topology objects
//
void bgui_vtol2D_tableau::
add_topology_objects(vcl_vector<vtol_topology_object_sptr> const& tos,
                     const vgui_style_sptr& style)
{
  for (vcl_vector<vtol_topology_object_sptr>::const_iterator tot = tos.begin();
       tot != tos.end(); tot++)
    {
      add_topology_object((*tot) , style );
    }
}


void bgui_vtol2D_tableau::
add_topology_object(vtol_topology_object_sptr const& tos,
                    const vgui_style_sptr& style)
{
  if (tos->cast_to_vertex()) {
    if (tos->cast_to_vertex()->cast_to_vertex_2d())
    {
      vtol_vertex_2d_sptr v = tos->cast_to_vertex()->cast_to_vertex_2d();
      this->add_vertex(v , style );
    }
  }
  else if (tos->cast_to_edge()) {
    if (tos->cast_to_edge()->cast_to_edge_2d())
    {
      vtol_edge_2d_sptr e = tos->cast_to_edge()->cast_to_edge_2d();
      this->add_edge(e , style );
    }
  }
  else if (tos->cast_to_face()) {
    if (tos->cast_to_face()->cast_to_face_2d())
    {
      vtol_face_2d_sptr f = tos->cast_to_face()->cast_to_face_2d();
      this->add_face(f , style );
    }
  }
}


void bgui_vtol2D_tableau::add_edges(vcl_vector<vtol_edge_2d_sptr> const& edges,
                                    bool verts ,
                                    const vgui_style_sptr& style )
{
  for (vcl_vector<vtol_edge_2d_sptr>::const_iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    this->add_edge(*eit , style );
    //optionally display the edge vertices
    if (verts)
    {
      vcl_vector<vtol_vertex_sptr>* vts = (*eit)->vertices();
      for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
           vit != vts->end(); vit++)
      {
        vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
        this->add_vertex(v , style);
      }
      delete vts;
    }
  }
}


void
bgui_vtol2D_tableau::add_faces(vcl_vector<vtol_face_2d_sptr> const& faces,
                               bool verts,
                               const vgui_style_sptr& style )
{
  for (vcl_vector<vtol_face_2d_sptr>::const_iterator fit = faces.begin();
       fit != faces.end(); fit++)
  {
    vtol_face_2d_sptr f = (*fit);
    this->add_face(f , style );
    if (verts)
    {
      vcl_vector<vtol_vertex_sptr>* vts = f->vertices();
      for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
           vit != vts->end(); vit++)
      {
        vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
        this->add_vertex(v , style );
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

  vgui_easy2D_tableau::clear();
}

void bgui_vtol2D_tableau::set_vtol_topology_object_style(vtol_topology_object_sptr tos,
                                                         const vgui_style_sptr& style)
{
  if (tos->cast_to_vertex())
    set_vertex_style(style);
  else if (tos->cast_to_edge()) {
    set_edge_style(style);
    set_edge_group_style(style);
  }
  else if (tos->cast_to_face())
    set_face_style(style);
}

void bgui_vtol2D_tableau::set_vertex_style(const vgui_style_sptr& style)
{
  vertex_style_ = style;
}

void bgui_vtol2D_tableau::set_edge_style(const vgui_style_sptr& style)
{
  edge_style_ = style;
}

void bgui_vtol2D_tableau::set_edge_group_style(const vgui_style_sptr& style)
{
  edge_group_style_ = style;
}

void bgui_vtol2D_tableau::set_face_style(const vgui_style_sptr& style)
{
  face_style_ = style;
}
