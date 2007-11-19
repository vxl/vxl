#include "bwm_observer_img.h"
#include <bwm/algo/bwm_algo.h>
#include <bwm/algo/bwm_image_processor.h>

#include <bwm/bwm_tableau_mgr.h>
#include <vcl_cmath.h>
#include <bgui/bgui_image_tableau.h>
#include <vgui/vgui_section_render.h>

#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_box_2d.h>
#include <bsol/bsol_algs.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

#define VERTEX_TYPE "bwm_soview2D_vertex"
#define POLYLINE_TYPE "bgui_vsol_soview2D_polyline"
#define POLYGON_TYPE "bgui_vsol_soview2D_polygon"
#define POINT_TYPE "bgui_vsol_soview2D_point"

bool bwm_observer_img::handle(const vgui_event &e)
{
  vgui_projection_inspector pi;
  
  if (e.type == vgui_BUTTON_DOWN && 
    e.button == vgui_MIDDLE && 
    e.modifier == vgui_SHIFT) {

    bgui_vsol_soview2D* p=0;
    bwm_soview2D_vertex* v = 0;

    // get the selected polyline or polygon
    if ((p = (bgui_vsol_soview2D*) get_selected_object(POLYGON_TYPE)) ||
      (p = (bgui_vsol_soview2D*) get_selected_object(POLYLINE_TYPE))) {
      // take the position of the first point
      pi.window_to_image_coordinates(e.wx, e.wy, start_x_, start_y_);
      moving_p_ = p;
      moving_polygon_ = true;
      moving_vertex_ = false;
      return true;
    } else if (v = (bwm_soview2D_vertex*) get_selected_object(VERTEX_TYPE)) {
      pi.window_to_image_coordinates(e.wx, e.wy, start_x_, start_y_);
      moving_v_ = v;
      moving_p_ = v->obj();
      moving_vertex_ = true;
      moving_polygon_ = false;
      return true;
    }
  } else if (e.type == vgui_MOTION && e.button == vgui_MIDDLE && 
    e.modifier == vgui_SHIFT && moving_polygon_) {
    float x, y;
    pi.window_to_image_coordinates(e.wx, e.wy, x, y);
    float x_diff = x-start_x_;
    float y_diff = y-start_y_;
    moving_p_->translate(x_diff, y_diff);

    // move all the vertices of the polyline or polygon
    vcl_vector<bwm_soview2D_vertex*> vertices = vert_list[moving_p_->get_id()];
    for (unsigned i=0; i<vertices.size(); i++) {
      bwm_soview2D_vertex* v = vertices[i];
      v->translate(x_diff, y_diff);
    }
    start_x_ = x;
    start_y_ = y;
    post_redraw();
    return true;

  } else if (e.type == vgui_MOTION && e.button == vgui_MIDDLE && 
    e.modifier == vgui_SHIFT && moving_vertex_) {
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
    } else if (moving_p_->type_name().compare(POLYLINE_TYPE) == 0) {
      bgui_vsol_soview2D_polyline* polyline = (bgui_vsol_soview2D_polyline*) moving_p_;
      polyline->sptr()->vertex(i)->set_x( polyline->sptr()->vertex(i)->x() + x_diff );
      polyline->sptr()->vertex(i)->set_y( polyline->sptr()->vertex(i)->y() + y_diff );
    } else {
      vcl_cerr << moving_p_->type_name() << " is NOT movable!!!!" << vcl_endl;
    }

    start_x_ = x;
    start_y_ = y;
    post_redraw();
    return true;
  } else if (e.type == vgui_BUTTON_UP && e.button == vgui_MIDDLE && e.modifier == vgui_SHIFT) {
    this->deselect_all();
    moving_vertex_ = false;
    moving_polygon_ = false;
    return true;
  }
  return bgui_vsol2D_tableau::handle(e);
}

void bwm_observer_img::create_box(vsol_box_2d_sptr box)
{
  vsol_polygon_2d_sptr pbox = bsol_algs::poly_from_box(box);
  create_polygon(pbox);
}

void bwm_observer_img::create_polygon(vsol_polygon_2d_sptr poly2d)
{
  float *x, *y;
  bwm_algo::get_vertices_xy(poly2d, &x, &y);
  unsigned nverts = poly2d->size();

  this->set_foreground(1,1,0);
  bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(poly2d);
  obj_list[polygon->get_id()] = polygon;

  vcl_vector<bwm_soview2D_vertex*> verts;
  this->set_foreground(0,1,0);
  for(unsigned i = 0; i<nverts; ++i) {
    bwm_soview2D_vertex* vertex = new bwm_soview2D_vertex(x[i],y[i],2.0f, polygon, i);
    this->add(vertex);
    verts.push_back(vertex);
  }
  vert_list[polygon->get_id()] = verts;
}

void bwm_observer_img::create_polyline(vsol_polyline_2d_sptr poly2d)
{
  float *x, *y;
  bwm_algo::get_vertices_xy(poly2d, &x, &y);
  unsigned nverts = poly2d->size();
   bgui_vsol_soview2D_polyline* polyline = this->add_vsol_polyline_2d(poly2d);
  obj_list[polyline->get_id()] = polyline;

  vcl_vector<bwm_soview2D_vertex*> verts;
  this->set_foreground(0,1,0);
  for(unsigned i = 0; i<nverts; ++i) {
    bwm_soview2D_vertex* vertex = new bwm_soview2D_vertex(x[i],y[i],1.0f, polyline, i);
    this->add(vertex);
    verts.push_back(vertex);
  }
  vert_list[polyline->get_id()] = verts;
}

void bwm_observer_img::create_point(vsol_point_2d_sptr p)
{
  bgui_vsol_soview2D_point* point = this->add_vsol_point_2d(p);
  obj_list[point->get_id()] = point;
}

bool bwm_observer_img::get_selected_box(vsol_box_2d_sptr & box)
{

  bgui_vsol_soview2D_polygon* p;
  if (p = (bgui_vsol_soview2D_polygon*)get_selected_object(POLYGON_TYPE)) {
    vsol_polygon_2d_sptr poly = p->sptr();
    box = poly->get_bounding_box();
    return true;
  }

  return false;
}

vgui_soview2D* bwm_observer_img::get_selected_object(vcl_string type)
{

  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();
  vcl_vector<vgui_soview2D*> objs;
  vgui_soview2D* obj;

  for (unsigned i=0; i<select_list.size(); i++) {
    vcl_cout << select_list[i]->type_name();
    if (select_list[i]->type_name().compare(type) == 0) {
      objs.push_back((vgui_soview2D*) select_list[i]);
    }
  }

  if (objs.size() == 1) {
    obj = (vgui_soview2D*) objs[0];
    return obj;
  }

  vcl_cerr << vcl_endl << "The number of selected " << type << " is " << 
    objs.size() << ". Please select only one!!!" << vcl_endl;
  return 0;
}

void bwm_observer_img::delete_selected()
{
  // first get the selected polygon
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  if ((select_list.size() == 1) && 
      ((select_list[0]->type_name().compare(POLYGON_TYPE) == 0) ||
       (select_list[0]->type_name().compare(POLYLINE_TYPE) == 0))) {

      // remove the polygon and the vertices
      delete_polygon(select_list[0]);
  } else if (select_list[0]->type_name().compare(VERTEX_TYPE) == 0)
    delete_vertex(select_list[0]);
  this->post_redraw();
}

void bwm_observer_img::delete_all() 
{
  vcl_map<unsigned, bgui_vsol_soview2D*>::iterator it = obj_list.begin();
  while (it != obj_list.end()) {
    delete_polygon(it->second);
    it = obj_list.begin();
  }
  this->post_redraw();
}

void bwm_observer_img::delete_polygon(vgui_soview* obj)
{
  // remove the polygon
  unsigned poly_id = obj->get_id();
  this->remove(obj);
  obj_list.erase(poly_id);

  // remove the vertices
  vcl_vector<bwm_soview2D_vertex*>  v = vert_list[poly_id];
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
        vcl_cerr << "Cannot delete a vertex from a triangle" << vcl_endl;
        return;
      }

      if (i >= poly2d->size()) {
        vcl_cerr << "The index is invalid [" << i << " of " << poly2d->size() << vcl_endl;
        return;
      }

      vcl_vector<vsol_point_2d_sptr> new_vertices;
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
        vcl_cerr << "Cannot delete a vertex from a polyline with 2 vertices" << vcl_endl;
        return;
      }

      if (i >= poly2d->size()) {
        vcl_cerr << "The index is invalid [" << i << " of " << poly2d->size() << vcl_endl;
        return;
      }

      vcl_vector<vsol_point_2d_sptr> new_vertices;
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

void bwm_observer_img::save()
{
}

void bwm_observer_img::hist_plot() 
{
  bwm_image_processor::hist_plot(img_tab_);
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
  vsol_box_2d_sptr box;
  if(!this->get_selected_box(box))
    {
      vcl_cerr << "In bwm_observer_img::step_edges_vd() - no box selected\n";
      return ;
    }
  
  vcl_vector<vdgl_digital_curve_sptr> edges;
  if(!bwm_image_processor::step_edges_vd(img_tab_, box, edges))
    {
      vcl_cerr << "In bwm_observer_img::step_edges_vd() - no edges\n";
      return;
    }

  for(vcl_vector<vdgl_digital_curve_sptr>::iterator eit = edges.begin();
      eit != edges.end(); ++eit)
    {
      bgui_vsol_soview2D_edgel_curve* curve = this->add_edgel_curve(*eit);
      obj_list[curve->get_id()] = curve;
    }
  this->post_redraw();
}

void bwm_observer_img::lines_vd()
{
  vsol_box_2d_sptr box;
  if(!this->get_selected_box(box))
    {
      vcl_cerr << "In bwm_observer_img::lines_vd() - no box selected\n";
      return ;
    }
  
  vcl_vector<vsol_line_2d_sptr> lines;
  if(!bwm_image_processor::lines_vd(img_tab_, box, lines))
    {
      vcl_cerr << "In bwm_observer_img::lines_vd() - no lines\n";
      return;
    }
  for(vcl_vector<vsol_line_2d_sptr>::iterator lit = lines.begin();
      lit != lines.end(); ++lit)
    {
      bgui_vsol_soview2D_line_seg* line = this->add_vsol_line_2d(*lit);
      obj_list[line->get_id()] = line;
    }
  this->post_redraw();
}
//
//: (x, y) is the target point to be positioned at the center 
//  of the grid cell containing this observer
//
void bwm_observer_img::move_to_point(float x, float y)
{
  if(viewer_)
    {
      //Get the current viewer state (scale and offset)
      float sx = viewer_->token.scaleX, sy = viewer_->token.scaleY;
      float tx = viewer_->token.offsetX, ty = viewer_->token.offsetY;

      //The position of this observer in the grid
      unsigned r = this->row(), c = this->col();

      //The grid tableau
      vgui_grid_tableau_sptr grid = bwm_tableau_mgr::instance()->grid();
      if(!grid)
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

      // The required translation to position in the centern
      float transx = twx-wx;
      float transy = twy-wy;


      viewer_->token.offsetX += transx;
      viewer_->token.offsetY -= transy;
      viewer_->post_redraw();

#if 0 // debug printouts
      vcl_cout << "\n\n====--=====\n";
      vcl_cout << "sx = " << sx << "  sy = " << sy << '\n';
      vcl_cout << "tx = " << tx << "  ty = " << ty << '\n';
      vcl_cout << "r = " << r << "  c = " << c << '\n';
      vcl_cout << "target point (" << x << ' ' << y << ")\n";
      vcl_cout << "target point in window coords (" << wx << ' ' << wy << ")\n";
      vcl_cout << "cell center in window coords (" 
               << twx << ' ' << twy << ")\n";
      vcl_cout << "required tx = " << transx 
               << "  required ty = " << transy << '\n';
      vcl_cout << vcl_flush;
#endif
    }
}
void bwm_observer_img::zoom_to_fit()
{
  if(!viewer_ || !img_tab_)
    return;

  // the image size
  unsigned ni = img_tab_->get_image_resource()->ni();
  unsigned nj = img_tab_->get_image_resource()->nj();

  // current viewer scale
  float sx = viewer_->token.scaleX, sy = vcl_fabs(viewer_->token.scaleY);

  // the window size
  vgui_projection_inspector p_insp;
  vgl_box_2d<float> bb(p_insp.x1, p_insp.x2, p_insp.y1, p_insp.y2);
  float w = bb.width()*sx;
  float h = bb.height()*sy;
  // the requred scale to fit the image in the window
  float required_scale_x = w/ni;
  float required_scale_y = h/nj;
  float r = required_scale_x;
  if(r>required_scale_y)
    r = required_scale_y;

  // the center of the image
  float cx = ni/2, cy = nj/2;

  // set the scale on the viewer
  viewer_->token.scaleX = r;
  viewer_->token.scaleY = -r;

  // position so the image is centered
  viewer_->token.offsetX = w/2.0-cx*r;
  viewer_->token.offsetY = h/2.0+cy*r;
  
  viewer_->post_redraw();

#if 0 //debug printouts
  vcl_cout << "sx = " << sx << "  sy = " << sy << '\n';
  vcl_cout << "bb.w " << w << " bb.h " << h << '\n';
  vcl_cout << "required scale = " << r << "  c(" << cx << ' ' 
           << cy << ")\n";
#endif
}
