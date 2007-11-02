#include "bwm_observer_img.h"
#include <bwm/algo/bwm_algo.h>
#include <bwm/algo/bwm_image_processor.h>

#include <vgui/vgui_soview2D.h>

#include <bgui/bgui_vsol_soview2D.h>

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

void bwm_observer_img::create_box(vsol_box_2d_sptr box)
{
  vsol_polygon_2d_sptr pbox = bsol_algs::poly_from_box(box);
  create_polygon(pbox);
  /*float *x, *y;
  bwm_algo::get_vertices_xy(pbox, &x, &y);
  unsigned nverts = pbox->size();
  this->set_foreground(0,1,0);
  bgui_vsol_soview2D_polygon* polybox_view = this->add_vsol_polygon_2d(pbox);
  obj_list[polybox_view->get_id()] = polybox_view;*/
}

void bwm_observer_img::create_polygon(vsol_polygon_2d_sptr poly2d)
{
  float *x, *y;
  bwm_algo::get_vertices_xy(poly2d, &x, &y);
  unsigned nverts = poly2d->size();

  vcl_vector<vgui_soview2D_circle*> verts;
  this->set_foreground(0,1,0);
  for(unsigned i = 0; i<nverts; ++i) {
    vgui_soview2D_circle* circle = this->add_circle(x[i],y[i],1.0f);
    verts.push_back(circle);
  }
  this->set_foreground(1,1,0);
  bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(poly2d);
  obj_list[polygon->get_id()] = polygon;
  vert_list[polygon->get_id()] = verts;
}

void bwm_observer_img::create_polyline(vsol_polyline_2d_sptr poly2d)
{
  float *x, *y;
  bwm_algo::get_vertices_xy(poly2d, &x, &y);
  unsigned nverts = poly2d->size();

  vcl_vector<vgui_soview2D_circle*> verts;
  this->set_foreground(0,1,0);
  for(unsigned i = 0; i<nverts; ++i) {
    vgui_soview2D_circle* circle = this->add_circle(x[i],y[i],1.0f);
    verts.push_back(circle);
  }

  bgui_vsol_soview2D_polyline* polyline = this->add_vsol_polyline_2d(poly2d);
  obj_list[polyline->get_id()] = polyline;
  vert_list[polyline->get_id()] = verts;
}

void bwm_observer_img::create_point(vsol_point_2d_sptr p)
{
  bgui_vsol_soview2D_point* point = this->add_vsol_point_2d(p);
  obj_list[point->get_id()] = point;
}

bool bwm_observer_img::get_selected_box(vsol_box_2d_sptr & box)
{
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();
  vcl_vector<vgui_soview*> boxes;

  for (unsigned i=0; i<select_list.size(); i++) {
    vcl_cout << select_list[i]->type_name();
    if (select_list[i]->type_name().compare("bgui_vsol_soview2D_polygon") == 0) {
      boxes.push_back(select_list[i]);
    }
  }

  if (boxes.size() == 1) {
    bgui_vsol_soview2D_polygon* p = (bgui_vsol_soview2D_polygon*) boxes[0];
    vsol_polygon_2d_sptr poly = p->sptr();
    box = poly->get_bounding_box();
    return true;
  }
  return false;
}

void bwm_observer_img::delete_selected()
{
  // first get the selected polygon
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  if ((select_list.size() == 1) && 
      ((select_list[0]->type_name().compare("vgui_soview2D_polygon") == 0) ||
       (select_list[0]->type_name().compare("vgui_soview2D_linestrip") == 0) || 
       (select_list[0]->type_name().compare("vgui_soview2D_point") == 0))) {

      // remove the polygon and the vertices
      delete_polygon(select_list[0]);
  }
  this->post_redraw();
}

void bwm_observer_img::delete_all() 
{
  vcl_map<unsigned, vgui_soview2D*>::iterator it = obj_list.begin();
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
  vcl_vector<vgui_soview2D_circle*>  v = vert_list[poly_id];
  for (unsigned i=0; i<v.size(); i++) {
    this->remove(v[i]);    
  }
  vert_list.erase(poly_id);
  this->post_redraw();
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
