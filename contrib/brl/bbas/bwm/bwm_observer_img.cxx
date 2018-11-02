#include <iostream>
#include <cmath>
#include "bwm_observer_img.h"
//:
// \file

#include <bwm/algo/bwm_algo.h>
#include <bwm/algo/bwm_utils.h>
#include <bwm/algo/bwm_image_processor.h>
#include <bwm/bwm_tableau_mgr.h>

#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_image_utils.h>
#include <vgui/vgui_section_render.h>
#include <vgui/vgui_projection_inspector.h>

#include <bsol/bsol_algs.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_line_2d.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>

#include <vil/vil_image_view.h>
#include <vil/file_formats/vil_nitf2_image.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bwm_observer_img::bwm_observer_img(bgui_image_tableau_sptr const& img, std::string name, std::string image_path, bool display_image_path)
: bgui_vsol2D_tableau(img), lock_vgui_status_(false), vgui_status_on_(false), draw_mode_(MODE_2D_POLY), img_tab_(img), viewer_(nullptr),
  change_type_("change"), show_image_path_(false), start_x_(0), start_y_(0), moving_p_(nullptr), moving_v_(nullptr), moving_vertex_(false),
  moving_polygon_(false), in_jog_mode_(false), row_(0), col_(0)
{
// LOAD IMAGE

  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = bwm_utils::load_image(image_path, params);
  if (!img_res) {
    //show_error("Image [" + image_path + "] NOT found");
    return;
  }
  img->set_image_resource(img_res, params);

  img->show_image_path(display_image_path);
  img->set_file_name(image_path);
  set_tab_name(name);
}

bool bwm_observer_img::handle(const vgui_event &e)
{
  vgui_projection_inspector pi;

  if (e.type == vgui_BUTTON_DOWN &&
      e.button == vgui_MIDDLE &&
      e.modifier == vgui_SHIFT)
  {
    bgui_vsol_soview2D* p=nullptr;
    bwm_soview2D_vertex* v = nullptr;

    // get the selected polyline or polygon
    if ((p = (bgui_vsol_soview2D*) get_selected_object(POLYGON_TYPE)) ||
        (p = (bgui_vsol_soview2D*) get_selected_object(POLYLINE_TYPE))) {
      // take the position of the first point
      pi.window_to_image_coordinates(e.wx, e.wy, start_x_, start_y_);
      moving_p_ = p;
      moving_polygon_ = true;
      moving_vertex_ = false;
      return true;
    }
    else if ((v = (bwm_soview2D_vertex*) get_selected_object(VERTEX_TYPE))) {
      pi.window_to_image_coordinates(e.wx, e.wy, start_x_, start_y_);
      moving_v_ = v;
      moving_p_ = v->obj();
      moving_vertex_ = true;
      moving_polygon_ = false;
      return true;
    }
  }
  else if (e.type == vgui_MOTION && e.button == vgui_MIDDLE &&
           e.modifier == vgui_SHIFT && moving_polygon_)
  {
    float x, y;
    pi.window_to_image_coordinates(e.wx, e.wy, x, y);
    float x_diff = x-start_x_;
    float y_diff = y-start_y_;
    moving_p_->translate(x_diff, y_diff);

    // move all the vertices of the polyline or polygon
    std::vector<bwm_soview2D_vertex*> vertices = vert_list[moving_p_->get_id()];
    for (unsigned i=0; i<vertices.size(); i++) {
      bwm_soview2D_vertex* v = vertices[i];
      v->translate(x_diff, y_diff);
    }
    start_x_ = x;
    start_y_ = y;

    post_redraw();
    return true;
  }
  else if (e.type == vgui_MOTION && e.button == vgui_MIDDLE &&
           e.modifier == vgui_SHIFT && moving_vertex_)
  {
    float x, y;
    pi.window_to_image_coordinates(e.wx, e.wy, x, y);
    float x_diff = x-start_x_;
    float y_diff = y-start_y_;
    // find the polyline including this vertex
    unsigned i = moving_v_->vertex_indx();
    moving_v_->translate(x_diff, y_diff);
    if (moving_p_->type_name().compare(POLYGON_TYPE) == 0) {
      bgui_vsol_soview2D_polygon* polygon = (bgui_vsol_soview2D_polygon*) moving_p_;
      polygon->sptr()->vertex(i)->set_x( polygon->sptr()->vertex(i)->x() + x_diff );
      polygon->sptr()->vertex(i)->set_y( polygon->sptr()->vertex(i)->y() + y_diff );
    }
    else if (moving_p_->type_name().compare(POLYLINE_TYPE) == 0) {
      bgui_vsol_soview2D_polyline* polyline = (bgui_vsol_soview2D_polyline*) moving_p_;
      polyline->sptr()->vertex(i)->set_x( polyline->sptr()->vertex(i)->x() + x_diff );
      polyline->sptr()->vertex(i)->set_y( polyline->sptr()->vertex(i)->y() + y_diff );
    }
    else {
      std::cerr << moving_p_->type_name() << " is NOT movable!!!!\n";
    }

    start_x_ = x;
    start_y_ = y;
    post_redraw();
    return true;
  }
  else if (e.type == vgui_BUTTON_UP && e.button == vgui_MIDDLE && e.modifier == vgui_SHIFT) {
    this->deselect_all();
    moving_vertex_ = false;
    moving_polygon_ = false;
    in_jog_mode_ = false;
    return true;
  }
  return base::handle(e);
}

//eliminate the segmentation soviews
bwm_observer_img::~bwm_observer_img()
{
  std::map<unsigned, std::vector<bgui_vsol_soview2D* > >::iterator mit =
    seg_views.begin();
  for (; mit!=seg_views.end(); ++mit)
  {
    std::vector<bgui_vsol_soview2D* > soviews = (*mit).second;
    for (unsigned i=0; i<soviews.size(); i++) {
      this->remove(soviews[i]);
    }
  }
  seg_views.clear();
  this->clear_reg_segmentation();
}

void bwm_observer_img::set_draw_mode(BWM_2D_DRAW_MODE mode)
{
  // if mode changed from the last time
  if (mode != draw_mode_) {
    draw_mode_ = mode;
    bool selectable = true;

    if (mode == MODE_2D_VERTEX) // vertex mode
      selectable = false;

    // polygons are mapped to soview ID, make them unselectable
    for (std::map<unsigned, bgui_vsol_soview2D*>::iterator it = obj_list.begin();
         it != obj_list.end(); it++) {
      //vgui_soview* p = vgui_soview::id_to_object(it->first);
      it->second->set_selectable(selectable);
    }
    // vector of vertices are mapped soview ID for each polygon
    for (std::map<unsigned, std::vector<bwm_soview2D_vertex* > >::iterator it=vert_list.begin();
         it != vert_list.end(); it++) {
      std::vector<bwm_soview2D_vertex* > v_list = it->second;
      for (unsigned i=0; i<v_list.size(); i++)
        v_list[i]->set_selectable(!selectable);
    }
  }
}

unsigned bwm_observer_img::create_box(vsol_box_2d_sptr box)
{
  vsol_polygon_2d_sptr pbox = bsol_algs::poly_from_box(box);
  return create_polygon(pbox);
}

unsigned bwm_observer_img::create_polygon(vsol_polygon_2d_sptr poly2d)
{
  float *x, *y;
  bwm_algo::get_vertices_xy(poly2d, &x, &y);
  unsigned nverts = poly2d->size();

  this->set_foreground(1,1,0);
  bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(poly2d);
  obj_list[polygon->get_id()] = polygon;
  if (draw_mode_ == 1)
    polygon->set_selectable(false);

  std::vector<bwm_soview2D_vertex*> verts;
  this->set_foreground(0,1,0);
  for (unsigned i = 0; i<nverts; ++i) {
    bwm_soview2D_vertex* vertex = new bwm_soview2D_vertex(x[i],y[i],0.5f, polygon, i);
    if (draw_mode_ == 0)
      vertex->set_selectable(false);
    this->add(vertex);
    verts.push_back(vertex);
  }
  vert_list[polygon->get_id()] = verts;
  return polygon->get_id();
}

unsigned bwm_observer_img::create_polyline(vsol_polyline_2d_sptr poly2d)
{
  float *x, *y;
  bwm_algo::get_vertices_xy(poly2d, &x, &y);
  unsigned nverts = poly2d->size();
  bgui_vsol_soview2D_polyline* polyline = this->add_vsol_polyline_2d(poly2d);
  obj_list[polyline->get_id()] = polyline;

  std::vector<bwm_soview2D_vertex*> verts;
  this->set_foreground(0,1,0);
  for (unsigned i = 0; i<nverts; ++i) {
    bwm_soview2D_vertex* vertex = new bwm_soview2D_vertex(x[i],y[i],0.5f, polyline, i);
    this->add(vertex);
    verts.push_back(vertex);
  }
  vert_list[polyline->get_id()] = verts;
  return polyline->get_id();
}

unsigned bwm_observer_img::create_point(vsol_point_2d_sptr p)
{
  bgui_vsol_soview2D_point* point = this->add_vsol_point_2d(p);
  obj_list[point->get_id()] = point;
  return point->get_id();
}

//: save the polygon to paste later, always stores the last selected
void bwm_observer_img::copy()
{
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  if (select_list.size() > 1) {
    std::cerr << "Please select only one object to copy\n";
    return;
  }
  copy_obj_ = (vgui_soview2D*) select_list[0];
}

void bwm_observer_img::paste(float x, float y)
{
  if (!copy_obj_) {
    std::cerr << "No object is selected to paste, COPY first\n";
    return;
  }

  float cx, cy;
  if (copy_obj_->type_name().compare(POLYGON_TYPE) == 0) {
    bgui_vsol_soview2D_polygon* obj = (bgui_vsol_soview2D_polygon*) copy_obj_;
    obj->get_centroid(&cx, &cy);
    vsol_polygon_2d_sptr p = obj->sptr();

    // translate it
    unsigned int n = p->size();
    std::vector<vsol_point_2d_sptr> vertices;

    for (unsigned int i=0; i<n;i++)
      vertices.push_back(new vsol_point_2d(p->vertex(i)->x() + (x-cx), p->vertex(i)->y() + (y-cy)));

    vsol_polygon_2d_sptr new_p = new vsol_polygon_2d(vertices);
    create_polygon(new_p);
    post_redraw();
  }
  else if (copy_obj_->type_name().compare(POLYLINE_TYPE) == 0) {
    bgui_vsol_soview2D_polyline* obj = (bgui_vsol_soview2D_polyline*) copy_obj_;
    obj->get_centroid(&cx, &cy);
    vsol_polyline_2d_sptr l = obj->sptr();
    // translate it
    unsigned int n = l->size();
    std::vector<vsol_point_2d_sptr> vertices;

    for (unsigned int i=0; i<n;i++)
      vertices.push_back(new vsol_point_2d(l->vertex(i)->x() + (x-cx), l->vertex(i)->y() + (y-cy)));

    vsol_polyline_2d_sptr new_p = new vsol_polyline_2d(vertices);
    create_polyline(new_p);
    post_redraw();
  }

  this->deselect_all();
}

void bwm_observer_img::clear_objects()
{
  std::map<unsigned, bgui_vsol_soview2D*>::iterator oit = obj_list.begin();
  for (; oit!=obj_list.end(); ++oit)
    this->remove((*oit).second);
  obj_list.clear();
  std::map<unsigned, std::vector<bwm_soview2D_vertex* > >::iterator vsit =
    vert_list.begin();
  for (; vsit != vert_list.end(); ++vsit){
    std::vector<bwm_soview2D_vertex* >::iterator vit = (*vsit).second.begin();
    for (; vit != (*vsit).second.end(); ++vit)
      this->remove(*vit);
  }
  vert_list.clear();
}

bool bwm_observer_img::get_selected_box(bgui_vsol_soview2D_polygon* &box)
{
  bgui_vsol_soview2D_polygon* p = (bgui_vsol_soview2D_polygon*)get_selected_object(POLYGON_TYPE);
  if (p) {
#if 0
    if (p->sptr()->size() != 4) {
      std::cerr << "Selected polygon is not a box\n";
      return false;
    }
    vsol_polygon_2d_sptr poly = p->sptr();
    box = poly->get_bounding_box();
#endif
    box = p;
    return true;
  }

  return false;
}
bool bwm_observer_img::get_selected_poly(bgui_vsol_soview2D_polygon* &poly){
  bgui_vsol_soview2D_polygon* p = (bgui_vsol_soview2D_polygon*)get_selected_object(POLYGON_TYPE);
  if(!p) return false;
  poly = p;
  return true;
}
vgui_soview2D* bwm_observer_img::get_selected_object(std::string type,
                                                     bool warn)
{
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  std::vector<vgui_soview2D*> objs;
  vgui_soview2D* obj;

  for (unsigned i=0; i<select_list.size(); i++) {
#if 0
    std::cout << select_list[i]->type_name();
#endif
    if (select_list[i]->type_name().compare(type) == 0) {
      objs.push_back((vgui_soview2D*) select_list[i]);
    }
  }

  if (objs.size() == 1) {
    obj = (vgui_soview2D*) objs[0];
    return obj;
  }

  if (warn)
    std::cerr << "\nThe number of selected " << type << " is "
             << objs.size() << ". Please select only one!!!\n";
  return nullptr;
}

std::vector<vgui_soview2D*> bwm_observer_img::get_selected_objects(std::string type)
{
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  std::vector<vgui_soview2D*> objs;

  for (unsigned i=0; i<select_list.size(); i++) {
    std::cout << select_list[i]->type_name();
    if (select_list[i]->type_name().compare(type) == 0) {
      objs.push_back((vgui_soview2D*) select_list[i]);
    }
  }
  std::cout << "Number of selected objects of type " << type << " = " << objs.size();
  return objs;
}

std::vector<vsol_spatial_object_2d_sptr>
bwm_observer_img::get_spatial_objects_2d()
{
  std::vector<vsol_spatial_object_2d_sptr> sos;
  for (std::map<unsigned, bgui_vsol_soview2D*>::iterator soit = obj_list.begin();
       soit != obj_list.end(); ++soit)
    sos.push_back((*soit).second->base_sptr());
  return sos;
}

void bwm_observer_img::delete_selected()
{
  // first get the selected polygon
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();

  if (select_list.size() == 0)
    return;

  if ((select_list.size() == 1) &&
      ((select_list[0]->type_name().compare(POLYGON_TYPE) == 0) ||
       (select_list[0]->type_name().compare(POLYLINE_TYPE) == 0)))
  {
    //first check to see if this is an image processing box

    std::map<unsigned, std::vector<bgui_vsol_soview2D* > >::iterator mit =
      seg_views.begin();
    std::map<unsigned, std::vector<bgui_vsol_soview2D* > >::iterator to_remove =
      seg_views.end();
    for (; mit!=seg_views.end();++mit)
      if (select_list[0]->get_id()==(*mit).first)
      {
        std::vector<bgui_vsol_soview2D* > edges = (*mit).second;

        for (unsigned i=0; i<edges.size(); i++) {
          this->remove(edges[i]);
        }
        to_remove = mit;
      }
    if (to_remove != seg_views.end())
      seg_views.erase(to_remove);

    // remove the polygon and the vertices
    delete_polygon(select_list[0]);
  }
  else if (select_list[0]->type_name().compare(VERTEX_TYPE) == 0)
    delete_vertex(select_list[0]);
  this->post_redraw();
}

void bwm_observer_img::delete_all()
{
  this->clear_objects();
  this->post_redraw();
}

void bwm_observer_img::delete_polygon(vgui_soview* obj)
{
  // remove the polygon
  unsigned poly_id = obj->get_id();
  this->remove(obj);
  obj_list.erase(poly_id);

  // remove the vertices
  std::vector<bwm_soview2D_vertex*>  v = vert_list[poly_id];
  for (unsigned i=0; i<v.size(); i++) {
    this->remove(v[i]);
  }
  vert_list.erase(poly_id);
  this->post_redraw();
}

void bwm_observer_img::delete_vertex(vgui_soview* vertex)
{
  bwm_soview2D_vertex* v = static_cast<bwm_soview2D_vertex*> (vertex);

  if (v) {
    bgui_vsol_soview2D* obj = v->obj();
    unsigned i = v->vertex_indx();

    // remove the vertex from the object
    if (obj->type_name().compare(POLYGON_TYPE) == 0) {
      bgui_vsol_soview2D_polygon* polygon = static_cast<bgui_vsol_soview2D_polygon*> (obj);
      vsol_polygon_2d_sptr poly2d = polygon->sptr();
      if (poly2d->size() == 3) {
        std::cerr << "Cannot delete a vertex from a triangle\n";
        return;
      }

      if (i >= poly2d->size()) {
        std::cerr << "The index is invalid [" << i << " of " << poly2d->size() << '\n';
        return;
      }

      std::vector<vsol_point_2d_sptr> new_vertices;
      for (unsigned k=0; k < poly2d->size(); k++) {
        if (k != i) // exclude the vertex to be deleted
          new_vertices.push_back(poly2d->vertex(k));
      }

      // delete the object
      delete_polygon(obj);

      // draw the new one
      vsol_polygon_2d_sptr new_poly = new vsol_polygon_2d(new_vertices);
      create_polygon(new_poly);
    }

    else if (obj->type_name().compare(POLYLINE_TYPE) == 0) {
      bgui_vsol_soview2D_polyline* polyline = static_cast<bgui_vsol_soview2D_polyline*> (obj);
      vsol_polyline_2d_sptr poly2d = polyline->sptr();
      if (poly2d->size() == 2) {
        std::cerr << "Cannot delete a vertex from a polyline with 2 vertices\n";
        return;
      }

      if (i >= poly2d->size()) {
        std::cerr << "The index is invalid [" << i << " of " << poly2d->size() << '\n';
        return;
      }

      std::vector<vsol_point_2d_sptr> new_vertices;
      for (unsigned k=0; k < poly2d->size(); k++) {
        if (k != i) // exclude the vertex to be deleted
          new_vertices.push_back(poly2d->vertex(k));
      }

      // delete the object
      delete_polygon(obj);

      // draw the new one
      vsol_polyline_2d_sptr new_poly = new vsol_polyline_2d(new_vertices);
      create_polyline(new_poly);
    }
  }
}

void bwm_observer_img::clear_box()
{
  // get the selected box
  bgui_vsol_soview2D_polygon* p = nullptr;

  if (!this->get_selected_box(p))
  {
    std::cerr << "In bwm_observer_img::clear_box() - no box selected\n";
    return ;
  }

  std::vector<bgui_vsol_soview2D* >& soviews = seg_views[p->get_id()];
  for (unsigned i=0; i<soviews.size(); i++) {
    this->remove(soviews[i]);
  }

  soviews.clear();
  seg_views[p->get_id()] = soviews;
  this->post_redraw();
  // do not delete the information about deleted edges, we may want to bring them back
}

void bwm_observer_img::recover_edges()
{
  //make sure the box is actually empty
  this->clear_box();
  // get the selected box
  bgui_vsol_soview2D_polygon* p = nullptr;
  if (!this->get_selected_box(p))
  {
    std::cerr << "In bwm_observer_img::clear_box() - no box selected\n";
    return;
  }

  std::vector<vsol_digital_curve_2d_sptr > edges;
  edges = edge_list[p->get_id()];
  std::vector<bgui_vsol_soview2D*> soviews;
  for (unsigned i=0; i<edges.size(); i++) {
    bgui_vsol_soview2D_digital_curve* curve
      = this->add_digital_curve(edges[i]);
    soviews.push_back(curve);
  }
  seg_views[p->get_id()] = soviews;
  post_redraw();
}

void bwm_observer_img::recover_lines()
{
  //make sure the box is actually empty
  this->clear_box();

  // get the selected box
  bgui_vsol_soview2D_polygon* p = nullptr;
  if (!this->get_selected_box(p))
  {
    std::cerr << "In bwm_observer_img::clear_box() - no box selected\n";
    return ;
  }

  std::vector<vsol_line_2d_sptr> lines;
  lines = line_list[p->get_id()];
  std::vector<bgui_vsol_soview2D*> soviews;
  for (unsigned i=0; i<lines.size(); i++) {
    bgui_vsol_soview2D_line_seg* line
      = this->add_vsol_line_2d(lines[i]);
    soviews.push_back(line);
  }
  seg_views[p->get_id()] = soviews;
  post_redraw();
}
// display edges for experimental registration
void bwm_observer_img::
display_reg_seg(std::vector<vsol_digital_curve_2d_sptr> const& search_edges,
                std::vector<vsol_digital_curve_2d_sptr> const& model_edges)
{
  this->clear_reg_segmentation();
  vgui_style_sptr mstyle = vgui_style::new_style(0.1f, 0.8f, 0.1f, 1.0f, 3.0f);
  vgui_style_sptr sstyle = vgui_style::new_style(0.8f, 0.1f, 0.8f, 1.0f, 3.0f);

  std::vector<vsol_digital_curve_2d_sptr>::const_iterator cit =
    search_edges.begin();
  for (; cit != search_edges.end(); ++cit)
    reg_seg_views_.push_back(this->add_digital_curve(*cit, sstyle));

  cit = model_edges.begin();
  for (; cit != model_edges.end(); ++cit)
    reg_seg_views_.push_back(this->add_digital_curve(*cit, mstyle));

  this->post_redraw();
}

// clear the edges displayed for the experimental registration tasks
void bwm_observer_img::clear_reg_segmentation()
{
  for (std::vector<bgui_vsol_soview2D* >::iterator sit = reg_seg_views_.begin();
       sit != reg_seg_views_.end(); ++sit)
    this->remove(*sit);
  reg_seg_views_.clear();
}


void bwm_observer_img::hist_plot()
{
  bwm_image_processor::hist_plot(img_tab_);
}

void bwm_observer_img::hist_plot_in_poly(){
  bgui_vsol_soview2D_polygon* p;
  if(!get_selected_poly(p)){
    std::cerr << " No polygon to scan to produce hist plot\n";
    return;
  }
  vsol_polygon_2d_sptr poly = p->sptr();
  bwm_image_processor::hist_plot(img_tab_, poly);
}

void bwm_observer_img::intensity_profile(float start_col, float start_row,
                                         float end_col, float end_row)
{
  bwm_image_processor::intensity_profile(img_tab_, start_col, start_row, end_col, end_row);
}

void bwm_observer_img::range_map()
{
  bwm_image_processor::range_map(img_tab_);
}

void bwm_observer_img::toggle_show_image_path()
{
  show_image_path_ = !show_image_path_;
  img_tab_->show_image_path(show_image_path_);
}

void bwm_observer_img::step_edges_vd()
{
  bgui_vsol_soview2D_polygon* p = nullptr;
  if (!this->get_selected_box(p))
  {
    std::cerr << "In bwm_observer_img::step_edges_vd() - no box selected\n";
    return;
  }

  std::vector<vsol_digital_curve_2d_sptr> edges;
  vsol_polygon_2d_sptr poly = p->sptr();
  vsol_box_2d_sptr box = poly->get_bounding_box();
  if (!bwm_image_processor::step_edges_vd(img_tab_, box, edges))
  {
    std::cerr << "In bwm_observer_img::step_edges_vd() - no edges\n";
    return;
  }

  // first clean up the box, if there is anything in it
  clear_box();

  std::vector<bgui_vsol_soview2D*> soviews;
  for (std::vector<vsol_digital_curve_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); ++eit)
  {
    bgui_vsol_soview2D_digital_curve* curve = this->add_digital_curve(*eit);
    soviews.push_back(curve);
  }
  edge_list[p->get_id()] = edges;
  seg_views[p->get_id()] = soviews;
  this->post_redraw();
}

void bwm_observer_img::lines_vd()
{
  bgui_vsol_soview2D_polygon* p = nullptr;
  if (!this->get_selected_box(p))
  {
    std::cerr << "In bwm_observer_img::lines_vd() - no box selected\n";
    return ;
  }

  std::vector<vsol_line_2d_sptr> lines;
  vsol_polygon_2d_sptr poly = p->sptr();
  vsol_box_2d_sptr box = poly->get_bounding_box();
  if (!bwm_image_processor::lines_vd(img_tab_, box, lines))
  {
    std::cerr << "In bwm_observer_img::lines_vd() - no lines\n";
    return;
  }

  // first clean up the box, if there is anything in it
  clear_box();

  std::vector<bgui_vsol_soview2D*> soviews;
  for (std::vector<vsol_line_2d_sptr>::iterator lit = lines.begin();
       lit != lines.end(); ++lit)
  {
    bgui_vsol_soview2D_line_seg* line = this->add_vsol_line_2d(*lit);
    // Gamze - do not add the lines one by one: create a vector and map it to the box
    soviews.push_back(line);
  }
  line_list[p->get_id()] = lines;
  seg_views[p->get_id()] = soviews;

  this->post_redraw();
}

bool bwm_observer_img::crop_image(vil_image_resource_sptr& chip)
{
  bgui_vsol_soview2D_polygon* p = nullptr;
  if (!this->get_selected_box(p))
  {
    std::cerr << "In bwm_observer_img::crop_image() - no box selected\n";
    return false;
  }
  vsol_polygon_2d_sptr poly = p->sptr();
  vsol_box_2d_sptr box = poly->get_bounding_box();
  return bwm_image_processor::crop_to_box(img_tab_, box, chip);
}

//: (x, y) is the target point to be positioned at the center of the grid cell containing this observer
//
void bwm_observer_img::move_to_point(float x, float y)
{
  // the image size
  unsigned ni = img_tab_->get_image_resource()->ni();
  unsigned nj = img_tab_->get_image_resource()->nj();
  if (x<0 || x>=ni || y<0 || y>=nj)
    std::cerr << "In bwm_observer_img::move_to_point(.) -"
             << " requested point outside of image bounds\n";
  if (x<0) x=0;
  if (x>=ni) x = ni-1;
  if (y<0) y=0;
  if (y>=nj) y = nj-1;
  if (viewer_)
  {
    //Get the current viewer state (scale and offset)
    float sx = viewer_->token.scaleX, sy = viewer_->token.scaleY;
    float tx = viewer_->token.offsetX, ty = viewer_->token.offsetY;

    //The position of this observer in the grid
    unsigned r = this->row(), c = this->col();

    //The grid tableau
    vgui_grid_tableau_sptr grid = bwm_tableau_mgr::instance()->grid();
    if (!grid)
      return;

    //The bounds of the grid cell containing this observer
    float xorig , yorig, xmax, ymax;
    grid->cell_bounding_box(c, r, xorig , yorig, xmax, ymax);

    // target image point in window coordinates
    float wx = sx*(x + tx/sx) + xorig;
    float wy = (ymax-yorig - sy*(y + ty/sy))+ yorig;

    // cell center in window coordinates
    float twx = (xorig + xmax)/2;
    float twy = (yorig + ymax)/2;

    // The required translation to position in the center
    float transx = twx-wx;
    float transy = twy-wy;

    viewer_->token.offsetX += transx;
    viewer_->token.offsetY -= transy;
    viewer_->post_redraw();

#if 0 // debug printouts
    std::cout << "\n\n====--=====\n"
             << "sx = " << sx << "  sy = " << sy << '\n'
             << "tx = " << tx << "  ty = " << ty << '\n'
             << "r = " << r << "  c = " << c << '\n'
             << "target point (" << x << ' ' << y << ")\n"
             << "target point in window coords (" << wx << ' ' << wy << ")\n"
             << "cell center in window coords ("
             << twx << ' ' << twy << ")\n"
             << "required tx = " << transx
             << "  required ty = " << transy << '\n'
             << std::flush;
#endif
  }
}

void bwm_observer_img::zoom_to_fit()
{
  if (!viewer_ || !img_tab_)
    return;

  if (!img_tab_->get_image_resource())
    return;

  // the image size
  unsigned ni = img_tab_->get_image_resource()->ni();
  unsigned nj = img_tab_->get_image_resource()->nj();

#if 0
  // current viewer scale
  float sx = viewer_->token.scaleX, sy = std::fabs(viewer_->token.scaleY);

  // the window size
  vgui_projection_inspector p_insp;
  vgl_box_2d<float> bb(p_insp.x1, p_insp.x2, p_insp.y1, p_insp.y2);
  float w = bb.width()*sx;
  float h = bb.height()*sy;
#endif
  //The grid tableau
  vgui_grid_tableau_sptr grid = bwm_tableau_mgr::instance()->grid();
  if (!grid)
    return;

  //The position of this observer in the grid
  unsigned ro = this->row(), cl = this->col();

  //The bounds of the grid cell containing this observer
  float xorig , yorig, xmax, ymax;
  grid->cell_bounding_box(cl, ro, xorig , yorig, xmax, ymax);

  float w = xmax-xorig, h = ymax-yorig;

  // the required scale to fit the image in the window
  float required_scale_x = w/ni;
  float required_scale_y = h/nj;
  float r = required_scale_x;
  if (r>required_scale_y)
    r = required_scale_y;

  // the center of the image
  float cx = ni/2, cy = nj/2;

  // set the scale on the viewer
  viewer_->token.scaleX = r;
  viewer_->token.scaleY = r;

  // position so the image is centered
  viewer_->token.offsetX = w/2.0-cx*r;
  viewer_->token.offsetY = h/2.0-cy*r;

  viewer_->post_redraw();
  viewer_->post_redraw();
#if 0 //debug printouts
  std::cout << "sx = " << sx << "  sy = " << sy << '\n'
           << "bb.w " << w << " bb.h " << h << '\n'
           << "required scale = " << r << "  c(" << cx << ' '
           << cy << ")\n";
#endif
}

void bwm_observer_img::scroll_to_point()
{
  static int ix = 0, iy = 0;
  vgui_dialog zoom("Move to Image Position");
  zoom.field ("image col", ix);
  zoom.field ("image row", iy);
  if (!zoom.ask())
    return;
  float x = static_cast<float>(ix), y = static_cast<float>(iy);
  this->move_to_point(x,y);
}

void bwm_observer_img::init_mask()
{
  mask_ = nullptr;
  change_polys_.clear();
}

void bwm_observer_img::set_change_type()
{
  unsigned int type = 0;
  vgui_dialog type_dialog("Change Type");

  if (this->change_choices_.empty())
  {
    this->change_choices_.push_back("change");
    this->change_choices_.push_back("don't care");
    this->change_choices_.push_back("vehicle");
    this->change_choices_.push_back("building");
    this->change_choices_.push_back("shadow");
    this->change_choices_.push_back("sewage");
    this->change_choices_.push_back("car");
    this->change_choices_.push_back("pick-up truck");
    this->change_choices_.push_back("utility");
    this->change_choices_.push_back("van");
    this->change_choices_.push_back("suv");
    this->change_choices_.push_back("minivan");
    this->change_choices_.push_back("high-quality");
    this->change_choices_.push_back("New Change Type");
  }
  type_dialog.choice("Change Type", this->change_choices_, type);
  if (!type_dialog.ask())
    return;

  if (type > this->change_choices_.size()) {
    std::cerr << "bwm_observer_img::set_change_type -- Invalid choice\n";
    return;
  }

  if (this->change_choices_[type] == "New Change Type")
  {
    vgui_dialog new_change_dialog("New Change Type");
    std::string new_change_type;
    new_change_dialog.field("New Change Type", new_change_type);
    new_change_dialog.ask();

    bool already_listed = false;
    for ( unsigned i = 0; i < this->change_choices_.size(); ++i )
    {
      if ( this->change_choices_[i] == new_change_type )
        already_listed = true;
    }
    if ( !already_listed )
    {
      this->change_choices_.pop_back();
      this->change_choices_.push_back(new_change_type);
      this->change_choices_.push_back("New Change Type");
    }

    this->change_type_ = new_change_type;
  }
  else
    change_type_ = this->change_choices_[type];
}


void bwm_observer_img::add_poly_to_mask()
{
  bgui_vsol_soview2D_polygon* p=nullptr;

  if (!ground_truth_)
    ground_truth_ = new bvgl_changes();

  // get the selected polygon
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  for (unsigned i=0; i<polys.size(); i++) {
    p = (bgui_vsol_soview2D_polygon*) polys[i];
    vsol_polygon_2d_sptr poly = p->sptr();
    if (!poly)
      continue;
    vgl_polygon<double> v_poly =  bsol_algs::vgl_from_poly(poly);
    bvgl_change_obj_sptr obj = new bvgl_change_obj(v_poly, change_type_);
    change_polys_[polys[i]->get_id()] = obj;
    ground_truth_->add_obj(obj);
  }
}

void bwm_observer_img::remove_poly_from_mask()
{
  bgui_vsol_soview2D_polygon* p=nullptr;
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  for (unsigned i=0; i<polys.size(); i++) {
    p = (bgui_vsol_soview2D_polygon*) polys[i];
    std::map<unsigned int, bvgl_change_obj_sptr>::iterator poly;
    // search the change polygons for deletion
    poly = change_polys_.find(p->get_id());
    if (poly != change_polys_.end()) {
      ground_truth_->remove_obj(poly->second);
      change_polys_.erase(poly->first);
    }
  }
}

#if 0 // commented out
void bwm_observer_img::create_mask()
{
  mask_ = 0;
  //index through the polygons and create the boolean mask image
  // the image size
  unsigned ni = img_tab_->get_image_resource()->ni();
  unsigned nj = img_tab_->get_image_resource()->nj();
  vil_image_view<unsigned char>* mask = new vil_image_view<unsigned char>(ni, nj);
  mask->fill(0);

  // fill the change areas
  for (std::map<unsigned int, vsol_polygon_2d_sptr>::iterator pit = mask_polys_.begin();
       pit != mask_polys_.end(); ++pit)
  {
    vgl_polygon<double> v_poly =  bsol_algs::vgl_from_poly(pit->second);
    vgl_polygon_scan_iterator<double> psi(v_poly, false);
    for (psi.reset(); psi.next();){
      int y = psi.scany();
      for (int x = psi.startx(); x<=psi.endx(); ++x)
      {
        unsigned u = static_cast<unsigned>(x);
        unsigned v = static_cast<unsigned>(y);
        (*mask)(u,v) = 255;
      }
    }
  }

  // fill the don't care areas
  for (std::map<unsigned int, vsol_polygon_2d_sptr>::iterator pit = mask_dontcare_polys_.begin();
       pit != mask_dontcare_polys_.end(); ++pit)
  {
    vgl_polygon<double> v_poly =  bsol_algs::vgl_from_poly(pit->second);
    vgl_polygon_scan_iterator<double> psi(v_poly, false);
    for (psi.reset(); psi.next();){
      int y = psi.scany();
      for (int x = psi.startx(); x<=psi.endx(); ++x)
      {
        unsigned u = static_cast<unsigned>(x);
        unsigned v = static_cast<unsigned>(y);
        (*mask)(u,v) = 125;
      }
    }
  }
  mask_ = mask;
}
#endif // 0

vil_image_view_base_sptr bwm_observer_img::mask()
{
  return ground_truth_->create_mask_from_objs(img_tab_->get_image_resource()->ni(), img_tab_->get_image_resource()->nj(), "change");
}

bool bwm_observer_img::save_changes_binary()
{
  std::string fname = bwm_utils::select_file();
  vsl_b_ofstream os(fname);
  ground_truth_->b_write(os);
  return true;
}

bool bwm_observer_img::load_changes_binary()
{
  std::string fname = bwm_utils::select_file();
  vsl_b_ifstream is(fname);
  if (ground_truth_ == nullptr)
    ground_truth_= new bvgl_changes();
  ground_truth_->b_read(is);

  // draw the polygons on the image
  for (unsigned i=0; i<ground_truth_->size(); i++) {
    bvgl_change_obj_sptr obj = ground_truth_->obj(i);
    vgl_polygon<double> poly = obj->poly();
    vsol_polygon_2d_sptr poly2d = bsol_algs::poly_from_vgl(poly);
    unsigned id = this->create_polygon(poly2d);
    change_polys_[id] = obj;
  }
  return true;
}
