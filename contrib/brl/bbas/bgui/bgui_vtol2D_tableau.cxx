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
#include <vgui/vgui_style_factory.h>


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

vcl_string bgui_vtol2D_tableau::type_name() const
{
  return "bgui_vtol2D_tableau";
}

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

bool bgui_vtol2D_tableau::handle(vgui_event const &e)
{
  // We aren't interested in other events so pass them to the base class.
  return bgui_vsol2D_tableau::handle(e);
}

//display topological objects
bgui_vtol_soview2D_vertex*
bgui_vtol2D_tableau::add_vertex(vtol_vertex_2d_sptr const& v)
{
  return add_vertex( v , vertex_style_.r,
                         vertex_style_.g,
                         vertex_style_.b,
                         vertex_style_.point_radius );
}

bgui_vtol_soview2D_vertex*
bgui_vtol2D_tableau::add_vertex(vtol_vertex_2d_sptr const& v,
                                const float r,
                                const float g,
                                const float b,
                                const float point_radius)
{
  bgui_vtol_soview2D_vertex* obj = new bgui_vtol_soview2D_vertex();
  obj->x = v->x();
  obj->y = v->y();
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r,
                                                 g,
                                                 b,
                                                 point_radius,
                                                 0.0f ) );

  //set the default style
  //bgui_style_sptr sty = style_map_[obj->type_name()];
  if (obj) {
    //obj->set_style(sty.ptr());
    //if (highlight_)
    //  {
    int id = obj->get_id();
    obj_map_[id]=v->cast_to_topology_object();
    //  }
  }
  return obj;
}

bgui_vtol_soview2D_edge*
bgui_vtol2D_tableau::add_edge(vtol_edge_2d_sptr const& e)
{
  return add_edge( e , edge_style_.r,
                       edge_style_.g,
                       edge_style_.b,
                       edge_style_.line_width );
}

bgui_vtol_soview2D_edge*
bgui_vtol2D_tableau::add_edge(vtol_edge_2d_sptr const& e , 
                              const float r,
                              const float g,
                              const float b,
                              const float line_width)
{

#ifdef DEBUG
  print_edgels(e);
#endif
  bgui_vtol_soview2D_edge* obj = new bgui_vtol_soview2D_edge(e);
  //set the default style
  //bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r ,
                                                 g ,
                                                 b ,
                                                 1.0f,
                                                 line_width ) );

  if (obj) {
    //obj->set_style(sty.ptr());
    // if (highlight_)
    //   {
    int id = obj->get_id();
    obj_map_[id]=e->cast_to_topology_object();
    //   }
  }
  return obj;
}


bgui_vtol_soview2D_edge_group*
bgui_vtol2D_tableau::add_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  return add_edge_group( edges , edge_group_style_.r,
                                 edge_group_style_.g,
                                 edge_group_style_.b,
                                 edge_group_style_.line_width );
}

bgui_vtol_soview2D_edge_group*
bgui_vtol2D_tableau::add_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges,
                                    const float r ,
                                    const float g ,
                                    const float b ,
                                    const float line_width )
{
  bgui_vtol_soview2D_edge_group* obj =
    new bgui_vtol_soview2D_edge_group(edges);
  //set the default style
  //bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r ,
                                                 g ,
                                                 b , 
                                                 1.0f ,
                                                 line_width ) );
  //obj->set_style(sty.ptr());
  return obj;
}

bgui_vtol_soview2D_face*
bgui_vtol2D_tableau::add_face(vtol_face_2d_sptr const& f)
{
  return add_face( f , face_style_.r,
                       face_style_.g,
                       face_style_.b,
                       face_style_.line_width );
}

bgui_vtol_soview2D_face*
bgui_vtol2D_tableau::add_face(vtol_face_2d_sptr const& f,
                              const float r,
                              const float g,
                              const float b,
                              const float line_width)
{
  bgui_vtol_soview2D_face* obj = new bgui_vtol_soview2D_face(f);

  //set the default style
  //bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  obj->set_style( vgui_style_factory::get_style( r ,
                                                 g ,
                                                 b , 
                                                 1.0f,
                                                 line_width ) );
  if (obj) {
    //  obj->set_style(sty.ptr());
    //  if (highlight_)
    //    {
    int id = obj->get_id();
    obj_map_[id]=f->cast_to_topology_object();
    //    }
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
add_topology_objects(vcl_vector<vtol_topology_object_sptr> const& tos,
                     const float r,
                     const float g,
                     const float b,
                     const float line_width,
                     const float point_radius)
{
  for (vcl_vector<vtol_topology_object_sptr>::const_iterator tot = tos.begin();
       tot != tos.end(); tot++)
    {
      add_topology_object((*tot) , r , g , b , line_width , point_radius );
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

void bgui_vtol2D_tableau::
add_topology_object(vtol_topology_object_sptr const& tos,
                    const float r ,
                    const float g ,
                    const float b ,
                    const float line_width ,
                    const float point_radius)
{
  if (tos->cast_to_vertex()) {
    if (tos->cast_to_vertex()->cast_to_vertex_2d())
      {
        vtol_vertex_2d_sptr v =
          tos->cast_to_vertex()->cast_to_vertex_2d();
        this->add_vertex(v , r , g , b , point_radius );
      }
  } else if (tos->cast_to_edge()) {
    if (tos->cast_to_edge()->cast_to_edge_2d())
      {
        vtol_edge_2d_sptr e =
          tos->cast_to_edge()->cast_to_edge_2d();
        this->add_edge(e , r , g , b , line_width );
      }
  } else if (tos->cast_to_face()) {
    if (tos->cast_to_face()->cast_to_face_2d())
      {
        vtol_face_2d_sptr f =
          tos->cast_to_face()->cast_to_face_2d();
        this->add_face(f , r , g , b , line_width );
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

void bgui_vtol2D_tableau::add_edges(vcl_vector<vtol_edge_2d_sptr> const& edges,
                                    bool verts ,
                                    const float r ,
                                    const float g ,
                                    const float b ,
                                    const float line_width ,
                                    const float point_radius )
{
  for (vcl_vector<vtol_edge_2d_sptr>::const_iterator eit = edges.begin();
       eit != edges.end(); eit++)
    {
      this->add_edge(*eit , r , g , b , line_width );
      //optionally display the edge vertices
      if (verts)
        {
          vcl_vector<vtol_vertex_sptr>* vts = (*eit)->vertices();
          for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
               vit != vts->end(); vit++)
            {
              vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
              this->add_vertex(v , r , g , b , point_radius);
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

void
bgui_vtol2D_tableau::add_faces(vcl_vector<vtol_face_2d_sptr> const& faces,
                               bool verts,
                               const float r ,
                               const float g ,
                               const float b ,
                               const float line_width , 
                               const float point_radius )
{
  for (vcl_vector<vtol_face_2d_sptr>::const_iterator fit = faces.begin();
       fit != faces.end(); fit++)
    {
      vtol_face_2d_sptr f = (*fit);
      this->add_face(f , r , g , b , line_width );
      if (verts)
        {
          vcl_vector<vtol_vertex_sptr>* vts = f->vertices();
          for (vcl_vector<vtol_vertex_sptr>::iterator vit = vts->begin();
               vit != vts->end(); vit++)
            {
              vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
              this->add_vertex(v , r , g , b , point_radius );
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
  if (tos->cast_to_vertex())
    set_vertex_style(r,g,b, point_radius);
  else if (tos->cast_to_edge()) {
    set_edge_style(r,g,b, line_width);
    set_edge_group_style(r,g,b, line_width);
  }
  else if (tos->cast_to_face())
    set_face_style(r,g,b, line_width);
}

void bgui_vtol2D_tableau::set_vertex_style(const float r, const float g,
                                           const float b,
                                           const float point_radius)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, point_radius, 0.0f);
  bgui_vtol_soview2D_vertex sv;
  style_map_[sv.type_name()]=sty;
#endif // 0
  vertex_style_.r = r;
  vertex_style_.g = g;
  vertex_style_.b = b;
  vertex_style_.point_radius = point_radius;
}
void bgui_vtol2D_tableau::set_edge_style(const float r, const float g,
                                         const float b, const float line_width)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vtol_soview2D_edge se;
  style_map_[se.type_name()]=sty;
#endif // 0
  edge_style_.r = r;
  edge_style_.g = g;
  edge_style_.b = b;
  edge_style_.line_width = line_width;
}

void bgui_vtol2D_tableau::set_edge_group_style(const float r, const float g,
                                               const float b,
                                               const float line_width)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vtol_soview2D_edge_group sg;
  style_map_[sg.type_name()]=sty;
#endif // 0
  edge_group_style_.r = r;
  edge_group_style_.g = g;
  edge_group_style_.b = b;
  edge_group_style_.line_width = line_width;
}

void bgui_vtol2D_tableau::set_face_style(const float r, const float g,
                                         const float b, const float line_width)
{
#if 0
  bgui_style_sptr sty = new bgui_style(r, g, b, 0.0f, line_width);
  bgui_vtol_soview2D_face sf;
  style_map_[sf.type_name()]=sty;
#endif // 0
  face_style_.r = r;
  face_style_.g = g;
  face_style_.b = b;
  face_style_.line_width = line_width;
}

//--------------------------------------------------------------
//  bgui_vtol2D_rubberband_client methods
//--------------------------------------------------------------
bgui_vtol2D_rubberband_client::
bgui_vtol2D_rubberband_client(bgui_vtol2D_tableau_sptr const& vtol2D) 
  : vtol2D_(vtol2D)
{
}

void
bgui_vtol2D_rubberband_client::
add_point(float x, float y)
{
  vtol2D_->add_point(x,y);
}


void
bgui_vtol2D_rubberband_client::
add_line(float x0, float y0, float x1, float y1)
{
  vtol2D_->add_line(x0, y0, x1, y1);
}


void
bgui_vtol2D_rubberband_client::
add_infinite_line(float a, float b, float c)
{
  vtol2D_->add_infinite_line(a, b, c);
}


void
bgui_vtol2D_rubberband_client::
add_circle(float x, float y, float r)
{
  vtol2D_->add_circle(x, y, r);
}


void
bgui_vtol2D_rubberband_client::
add_linestrip(int n, float const* x, float const* y)
{
  vtol2D_->add_linestrip(n, x, y);
}


void
bgui_vtol2D_rubberband_client::
add_polygon(int n, float const* x, float const* y)
{
  vcl_vector<vtol_vertex_sptr> verts;
  for(int i =0; i<n-1; i++)
    verts.push_back(new vtol_vertex_2d(x[i], y[i]));
  vtol_face_2d_sptr f2d = new vtol_face_2d(verts);
  vtol2D_->add_face(f2d);
  vtol2D_->set_temp(f2d->cast_to_face());
}


void
bgui_vtol2D_rubberband_client::
add_box(float x0, float y0, float x1, float y1)
{
  vtol_vertex_sptr v0 = new vtol_vertex_2d(x0, y0);  
  vtol_vertex_sptr v1 = new vtol_vertex_2d(x1, y0);  
  vtol_vertex_sptr v2 = new vtol_vertex_2d(x1, y1);  
  vtol_vertex_sptr v3 = new vtol_vertex_2d(x0, y1); 
  vcl_vector<vtol_vertex_sptr> verts;
  verts.push_back(v0);   verts.push_back(v1);
  verts.push_back(v2);   verts.push_back(v3);
  vtol_face_2d_sptr box = new vtol_face_2d(verts);
  vtol2D_->add_face(box);
  vtol2D_->set_temp(box->cast_to_face());
}


void
bgui_vtol2D_rubberband_client::
clear_highlight()
{
  vtol2D_->highlight(0);
}
