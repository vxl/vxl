#include <bgui/bgui_vtol_soview2D.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <vgui/vgui.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_style.h>
#include <vtol/vtol_face_2d.h>

bgui_vtol2D_tableau::bgui_vtol2D_tableau(const char* n):
  vgui_easy2D_tableau(n){this->init();}

bgui_vtol2D_tableau::bgui_vtol2D_tableau(vgui_image_tableau_sptr const& it,
                                         const char* n):
  vgui_easy2D_tableau(it, n){this->init();}

bgui_vtol2D_tableau::bgui_vtol2D_tableau(vgui_tableau_sptr const& t,
                                         const char* n):
  vgui_easy2D_tableau(t, n){this->init();}

bgui_vtol2D_tableau::~bgui_vtol2D_tableau()
{
  
}

void bgui_vtol2D_tableau::init()
{  
  old_id_ = 0;
  //set highlight display style parameters
  highlight_style_ = new bgui_style(0.0, 0.0, 1.0, 5.0, 5.0);
  //define default soview styles
  //these could be overridden by later tablau drawing commands
  //such as set_foreground.  Probably though we shouldn't use those
  //since normal users wouldn't want to change the color of standard
  //topology items.
  bgui_style_sptr vertex_style = new bgui_style(1.0, 0.0, 0.0, 3.0, 0.0);
  bgui_style_sptr edge_style = new bgui_style(0.5, 0.5, 0.0, 0.0, 3.0);
  bgui_style_sptr edge_group_style = new bgui_style(0.0, 1.0, 0.0, 0.0, 3.0);
  bgui_style_sptr face_style = new bgui_style(0.0, 1.0, 0.0, 0.0, 3.0);
  //put them into the map
  bgui_vtol_soview2D_vertex sv;
  style_map_[sv.type_name()]=vertex_style;

  bgui_vtol_soview2D_edge se;
  style_map_[se.type_name()]=edge_style;

  bgui_vtol_soview2D_edge_group sg;
  style_map_[sg.type_name()]=edge_group_style;

  bgui_vtol_soview2D_face sf;
  style_map_[sf.type_name()]=face_style;
}
//: 
// Provide roaming highlighting for soviews in the tableau.
// As the mouse moves the soview closest to the mouse is 
// changed to the highlighed style.
// vgui_displaylist2D_tableau::motion(..) has a mechanism for 
// highlighting the nearest object but it doesn't work.

bool bgui_vtol2D_tableau::handle(vgui_event const &e)
{
  if (e.type == vgui_MOTION)
    {
      //retrive the previously highlighted soview and
      //restore it to its default style
      vgui_soview* old_so = vgui_soview::id_to_object(old_id_);          
      if(old_so)
        {
          bgui_style_sptr default_sty = style_map_[old_so->type_name()];
          bgui_style* bs = (bgui_style*)default_sty.ptr();
          vgui_style* s = (vgui_style*)bs;
          old_so->set_style(s);
        }
      //get the soview that is closest to the mouse
      vgui_soview2D* high_so = (vgui_soview2D*)get_highlighted_soview();
      if (high_so)
        {
          //replace the old soview with the currently closest view
          int id = high_so->get_id();
          old_id_ = id;
          //set soview style to the highlight color and weight
          bgui_style* bsh = (bgui_style*)highlight_style_.ptr();
          vgui_style* sh = (vgui_style*)bsh;
          high_so->set_style(sh);
          this->post_redraw();
        }
    }
  // We aren't interested in other events so pass them to the base class.
  return vgui_easy2D_tableau::handle(e);
}

bgui_vtol_soview2D_digital_curve* 
bgui_vtol2D_tableau::add_digital_curve(vdgl_digital_curve_sptr& dc)
{
  this->set_line_width(3.0);
  this->set_foreground(0.0,0.0,1.0);
  bgui_vtol_soview2D_digital_curve* obj = 
    new bgui_vtol_soview2D_digital_curve(dc);
  add(obj);
  return obj;
}

bgui_vtol_soview2D_vertex* bgui_vtol2D_tableau::add_vertex(vtol_vertex_2d_sptr& v)
{
  bgui_vtol_soview2D_vertex* obj = new bgui_vtol_soview2D_vertex();
  obj->x = v->x();
  obj->y = v->y();
  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  if(obj)
    {
      sty->clone_style(obj->get_style());
      int id = obj->get_id();
      obj_map_[id]=v->cast_to_topology_object();
    }
  return obj;
}

bgui_vtol_soview2D_edge* bgui_vtol2D_tableau::add_edge(vtol_edge_2d_sptr& e)
{
  bgui_vtol_soview2D_edge* obj = new bgui_vtol_soview2D_edge(e);
  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  if(obj)
    {
      sty->clone_style(obj->get_style());
      int id = obj->get_id();
      obj_map_[id]=e->cast_to_topology_object();
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
  sty->clone_style(obj->get_style());
  return obj;
}

bgui_vtol_soview2D_face* bgui_vtol2D_tableau::add_face(vtol_face_2d_sptr& f)
{
  bgui_vtol_soview2D_face* obj = new bgui_vtol_soview2D_face(f);

  //set the default style
  bgui_style_sptr sty = style_map_[obj->type_name()];
  add(obj);
  if(obj)
    {
      sty->clone_style(obj->get_style());
      int id = obj->get_id();
      obj_map_[id]=f->cast_to_topology_object();
    }
  return obj;
}

void bgui_vtol2D_tableau::add_edges(vcl_vector<vtol_edge_2d_sptr>& edges,
                                    bool verts)
{
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
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
bgui_vtol2D_tableau::add_faces(vcl_vector<vtol_face_2d_sptr>& faces,
                               bool verts)
{
  for (vcl_vector<vtol_face_2d_sptr>::iterator fit = faces.begin();
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
  if(!to)
    {
      vcl_cout << "In bgui_vtol2D_tableau::get_mapped_edge(..) -"
               << " null map entry\n";
      return 0;
    }
  return to->cast_to_edge()->cast_to_edge_2d();
}
