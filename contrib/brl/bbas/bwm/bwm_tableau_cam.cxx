#include <iostream>
#include <algorithm>
#include <sstream>
#include "bwm_tableau_cam.h"
#include "bwm_site_mgr.h"
//:
// \file
#include <bwm/bwm_tableau_mgr.h>
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_observable_mesh.h>
#include <bwm/bwm_observable_mesh_circular.h>
#include <bwm/bwm_tableau_text.h>
#include <bwm/bwm_popup_menu.h>
#include <bwm/bwm_world.h>
#include <bwm/algo/bwm_utils.h>
#include <bwm/algo/bwm_algo.h>

#include <vgl/vgl_homg_plane_3d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>


#include <volm/volm_category_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

#define NUM_CIRCL_SEC 12

void bwm_tableau_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  menu.clear();

  bwm_popup_menu pop(this);
  vgui_menu submenu;
  pop.get_menu(menu);
}

void bwm_tableau_cam::create_polygon_mesh()
{
  // first lock the bgui_image _tableau
  this->lock();
  bwm_observer_mgr::instance()->stop_corr();
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);
  pick_polygon(poly2d);
  if (! poly2d) {
    std::cerr << "In bwm_tableau_cam::create_polygon_mesh - pick_polygon failed\n";
    return ;
  }
  vsol_polygon_3d_sptr poly3d;

  //vgl_homg_plane_3d<double> plane(0,0,1,-740);
  //my_observer_->set_proj_plane(plane);
  //std::cout << "setting proj plane: " << plane << std::endl;

  my_observer_->backproj_poly(poly2d, poly3d);
  if (!poly3d) {
    std::cout << "in bwm_tableau_cam::create_polygon_mesh - backprojection failed\n";
    return;
  }
  bwm_observable_mesh_sptr my_polygon = new bwm_observable_mesh(bwm_observable_mesh::BWM_MESH_FEATURE);
  bwm_world::instance()->add(my_polygon);
  bwm_observer_mgr::instance()->attach(my_polygon);
  my_polygon->set_object(poly3d);
  my_polygon->set_site(bwm_site_mgr::instance()->site_name());
  this->unlock();
}

void bwm_tableau_cam::triangulate_mesh()
{
  my_observer_->triangulate_meshes();
}

void bwm_tableau_cam::create_circular_polygon()
{
  this->lock();
  bwm_observer_mgr::instance()->stop_corr();
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);

  std::vector< vsol_point_2d_sptr > ps_list;
  unsigned max = 10;
  pick_point_set(ps_list, max);
  if (ps_list.size() == 0) {
    std::cerr << "In bwm_tableau_cam::create_circle - pick_points failed\n";
    return ;
  }
  vsol_polygon_3d_sptr poly3d;
  double r;
  vgl_point_2d<double> center;
  my_observer_->create_circular_polygon(ps_list, poly3d, NUM_CIRCL_SEC, r, center);
  bwm_observable_mesh_sptr my_polygon = new bwm_observable_mesh_circular(r, center);
  bwm_world::instance()->add(my_polygon);
  bwm_observer_mgr::instance()->attach(my_polygon);
  my_polygon->set_object(poly3d);
  this->unlock();
}

void bwm_tableau_cam::set_master()
{
  bwm_observer_mgr::BWM_MASTER_OBSERVER = this->my_observer_;
}

//: set the observer as per the image type
void bwm_tableau_cam::set_eo()
{
  bwm_observer_mgr::BWM_EO_OBSERVER = this->my_observer_;
}

void bwm_tableau_cam::set_other_mode()
{
  bwm_observer_mgr::BWM_OTHER_MODE_OBSERVER = this->my_observer_;
}

void bwm_tableau_cam::move_obj_by_vertex()
{
  // first check if master tableau is set
  bwm_observer_cam* mt = bwm_observer_mgr::BWM_MASTER_OBSERVER;
  if (mt == nullptr) {
    std::cerr << "Master Tableau is not selected, please select one different than the current one!\n";
    return;
  }
  else if (mt == this->my_observer_) {
    std::cerr << "Please select a tableau different than the current one!\n";
    return;
  }

  my_observer_->set_selection(false);
  float x,y;
  set_color(1, 0, 0);
  pick_point(&x, &y);
  vsol_point_2d_sptr pt = new vsol_point_2d((double)x,(double)y);
  my_observer_->set_selection(true);
  my_observer_->move_ground_plane(mt->get_proj_plane(), pt);
}

void bwm_tableau_cam::extrude_face()
{
  my_observer_->extrude_face();
}

void bwm_tableau_cam::divide_face()
{
  float x1, y1, x2, y2;
  unsigned face_id;

  bwm_observer_mgr::instance()->stop_corr();
  bwm_observable_sptr obj = my_observer_->selected_face(face_id);
  if (obj) {
    pick_line(&x1, &y1, &x2, &y2);
    my_observer_->divide_face(obj, face_id, x1, y1, x2, y2);
  }
  else
    std::cerr << "Please first select the face to be divided\n";
}

void bwm_tableau_cam::create_inner_face()
{
  bwm_observer_mgr::instance()->stop_corr();
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);
  pick_polygon(poly2d);
  my_observer_->connect_inner_face(poly2d);
}

void bwm_tableau_cam::move_corr()
{
  float x,y;
  set_color(1, 0, 0);
  pick_point(&x, &y);
  vsol_point_2d_sptr pt = new vsol_point_2d((double)x,(double)y);
  my_observer_->move_corr_point(pt);
}

void bwm_tableau_cam::set_corr_to_vertex()
{
  my_observer_->set_corr_to_vertex();
}

void bwm_tableau_cam::world_pt_corr()
{
  my_observer_->world_pt_corr();
}


void bwm_tableau_cam::select_proj_plane()
{
  my_observer_->select_proj_plane();
}

void bwm_tableau_cam::define_proj_plane()
{
  bwm_observer_mgr::instance()->stop_corr();
  // pick the ground truth line
  float x1, y1, x2, y2;
  pick_line(&x1, &y1, &x2, &y2);
  std::cout << '(' << x1 << ',' << y1 << ')' << '(' << x2 << ',' << y2 << ')' << std::endl;
  my_observer_->set_ground_plane(x1, y1, x2, y2);
}

void bwm_tableau_cam::define_xy_proj_plane()
{
  static double z = 0;
  vgui_dialog zval("Define XY plane");
  zval.field("Elevation (meters)", z);
  if (!zval.ask())
    return;
  vgl_homg_plane_3d<double> plane(0,0,1,-z);
  my_observer_->set_proj_plane(plane);
}

void bwm_tableau_cam::define_yz_proj_plane()
{
  static double x = 0;
  vgui_dialog xval("Define YZ plane");
  xval.field("X (meters)", x);
  if (!xval.ask())
    return;
  vgl_homg_plane_3d<double> plane(1,0,0,-x);
  my_observer_->set_proj_plane(plane);
}

void bwm_tableau_cam::define_xz_proj_plane()
{
  static double y= 0;
  vgui_dialog yval("Define XZ plane");
  yval.field("Y (meters)", y);
  if (!yval.ask())
    return;
  vgl_homg_plane_3d<double> plane(0,1,0,-y);
  my_observer_->set_proj_plane(plane);
}

void bwm_tableau_cam::label_roof()
{
  my_observer_->label_roof();
}

void bwm_tableau_cam::label_wall()
{
  my_observer_->label_wall();
}

void bwm_tableau_cam::load_mesh()
{
  // get the file name for the mesh
  std::string file = bwm_utils::select_file();

  // load the mesh from the given file
  bwm_observable_mesh_sptr obj = new bwm_observable_mesh();
  bwm_observer_mgr::instance()->attach(obj);
  bool success = obj->load_from(file.data());
  if (success) {
    bwm_world::instance()->add(obj);
    obj->set_path(file.data());
    my_observer_->set_face_mode();
  }
  else
    bwm_observer_mgr::instance()->detach(obj);
}

void bwm_tableau_cam::load_mesh_multiple()
{
  // read txt file
  std::string master_filename = bwm_utils::select_file();
  std::ifstream file_inp(master_filename.data());
  if (!file_inp.good()) {
    std::cerr << "error opening file "<< master_filename << '\n';
    return;
  }

  while (!file_inp.eof()) {
    std::ostringstream fullpath;
    std::string mesh_fname;
    file_inp >> mesh_fname;
    if (!mesh_fname.empty() && (mesh_fname[0] != '#')) {
      fullpath << master_filename << '.' << mesh_fname;
      // load the mesh from the given file
      bwm_observable_mesh_sptr obj = new bwm_observable_mesh();
      bwm_observer_mgr::instance()->attach(obj);
      obj->load_from(fullpath.str());
    }
  }
  file_inp.close();

  return;
}

void bwm_tableau_cam::delete_object()
{
  my_observer_->delete_object();
}

void bwm_tableau_cam::delete_all()
{
  my_observer_->delete_all();
}

void bwm_tableau_cam::save()
{
  my_observer_->save();
}

void bwm_tableau_cam::save(std::string path)
{
  my_observer_->save(path);
}

void bwm_tableau_cam::save_all()
{
  my_observer_->save();
}

void bwm_tableau_cam::save_all(std::string path)
{
  my_observer_->save();
}

void bwm_tableau_cam::scan_regions()
{
  my_observer_->scan_regions();
}

void bwm_tableau_cam::project_shadow()
{
  my_observer_->project_shadow();
}

void bwm_tableau_cam::show_geo_position()
{
  my_observer_->show_geo_position();
}

void bwm_tableau_cam::geo_position_vertex()
{
  my_observer_->position_vertex(true);
}

void bwm_tableau_cam::local_position_vertex()
{
  my_observer_->position_vertex(false);
}

void bwm_tableau_cam::scroll_to_point(double lx, double ly, double lz)
{
  my_observer_->scroll_to_point(lx,ly,lz);
}

void bwm_tableau_cam::create_terrain()
{
#if 0 // commented out
  this->lock();
  //1. pick a boundary for the terrain as a polygon
  bwm_observer_mgr::instance()->stop_corr();
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);
  pick_polygon(poly2d);
#endif // 0
  my_observer_->create_terrain();
#if 0 // commented out
  this->unlock();
#endif // 0
}

void bwm_tableau_cam::help_pop()
{
  bwm_tableau_text* text_tab = new bwm_tableau_text(500, 500);
#if 0
  text->set_text("C:/lems/lemsvxlsrc/lemsvxlsrc/contrib/bwm/doc/doc/HELP_cam.txt");
#endif
  std::string h("SHIFT p = create 3-d polygon\nSHIFT t = triangulate_mesh\nSHIFT m = move object by vertex\nSHIFT e = extrude face\nSHIFT s = save a selected 3-d polygon\nSHIFT h = key help documentation\n");
  text_tab->set_string(h);
  vgui_tableau_sptr v = vgui_viewer2D_tableau_new(text_tab);
  vgui_tableau_sptr s = vgui_shell_tableau_new(v);
  vgui_dialog popup("CAMERA TABLEAU HELP");
  popup.inline_tableau(s, 550, 550);
  if (!popup.ask())
    return;
}

bool bwm_tableau_cam::handle(const vgui_event& e)
{
  //std::cout << "Key:" << e.key << " modif: " << e.modifier << std::endl;
  if (e.key == 'p' && e.modifier == vgui_SHIFT && e.type == vgui_KEY_PRESS) {
    create_polygon_mesh();
    return true;
  }
  else if (e.key == 't' && e.modifier==vgui_SHIFT && e.type==vgui_KEY_PRESS) {
    this->triangulate_mesh();
    return true;
  }
  else if ( e.key == 'm' && e.modifier==vgui_SHIFT && e.type==vgui_KEY_PRESS) {
    this->move_obj_by_vertex();
    return true;
  }
  else if ( e.key == 'e' && e.modifier==vgui_SHIFT && e.type==vgui_KEY_PRESS) {
    this->extrude_face();
    return true;
  }
  else if ( e.key == 's' && e.modifier==vgui_SHIFT && e.type==vgui_KEY_PRESS) {
    this->save();
    return true;
  }
  else if ( e.key == '1' && e.modifier==vgui_SHIFT && e.type==vgui_KEY_PRESS) {
    this->project_shadow();
    return true;
  }
#if 0 //flakey behavior --needs work
  else if ( e.key == 'z' && e.modifier==vgui_SHIFT && e.type==vgui_KEY_PRESS) {
    this->deselect_all();
    return true;
  }
  else if ( e.key == 'a' && e.modifier==vgui_SHIFT && e.type==vgui_KEY_PRESS) {
    this->clear_all();
    return true;
  }
#endif
  else if ( e.key == 'h' && e.modifier==vgui_SHIFT && e.type==vgui_KEY_PRESS) {
    this->help_pop();
    return true;
  }
 return bwm_tableau_img::handle(e);
}

// ======================   camera calibration methods =============
void bwm_tableau_cam::set_focal_length()
{
  static double focal_length = 3000.0;
  vgui_dialog fval("Set Focal Length");
  fval.field("Focal Length (pixel units)", focal_length);
  if (!fval.ask())
    return;
  my_observer_->set_focal_length(focal_length);
}

void bwm_tableau_cam::set_cam_height()
{
  static double cam_height = 1.6;
  vgui_dialog hval("Set Camera Center Height");
  hval.field("Height (m)", cam_height);
  if (!hval.ask())
    return;
  my_observer_->set_cam_height(cam_height);
}

void bwm_tableau_cam::add_ground_plane()
{
  // allowed land class
  std::vector<std::string> land_types = this->set_land_types();

  static std::string name = "ground_plane";
  static unsigned order = 0;
  static unsigned land_vec_id = 0;
  vgui_dialog vdval("Ground Plane Region");
  vdval.field("Name ", name);
  vdval.choice("Land Class ", land_types, land_vec_id);
  // obtain actual land id
  std::string land_name = land_types[land_vec_id];
  unsigned char land_id = volm_osm_category_io::volm_land_table_name[land_name].id_;
  if (!vdval.ask())
    return;
  my_observer_->add_ground_plane(order, (unsigned)land_id, name);
}

void bwm_tableau_cam::add_sky()
{
  static std::string name = "sky";
  static unsigned  order = 255;
  vgui_dialog vdval("Sky Region");
  vdval.field("Name ", name);
  if (!vdval.ask())
    return;
  my_observer_->add_sky(order, name);
}

#if 0
void bwm_tableau_cam::add_vertical_depth_region()
{
  static double min_depth = 0.0;
  static double max_depth = 100.0;
  static std::string name = "";
  vgui_dialog vdval("Vertical Region");
  vdval.field("Min Depth (m)", min_depth);
  vdval.field("Max Depth (m)", max_depth);
  vdval.field("Name ", name);
  if (!vdval.ask())
    return;
  my_observer_->add_vertical_depth_region(min_depth, max_depth, name);
}
#endif

void bwm_tableau_cam::add_region()
{
  // allowed orientation
  std::vector<std::string> orient_types;
  orient_types = this->set_orient_types();
  // allowed land class
  std::vector<std::string> land_types;
  land_types = this->set_land_types();

  static std::string name = "";
  static double     min_depth = 0.0;
  static double     max_depth = 100.0;
  static unsigned   order = 0;
  static unsigned   orientation = 0;
  static unsigned   land_vec_id = 0;
  static double     height = -1.0;
  // create menu
  vgui_dialog vdval("Add Region");
  vdval.field("Name ", name);
  vdval.field("Min Depth (m)", min_depth);
  vdval.field("Max Depth (m)", max_depth);
  vdval.field("Height (m)", height);
  vdval.field("Order ", order);
  vdval.choice("Orientation ", orient_types, orientation);
  vdval.choice("Land Class ", land_types, land_vec_id);
  if (!vdval.ask())
    return;
  // obtain the actual land id
  std::string land_name = land_types[land_vec_id];
  unsigned char land_id = volm_osm_category_io::volm_land_table_name[land_name].id_;
  // create associated depth_map_region
  my_observer_->add_region(name, min_depth, max_depth, order, orientation, (unsigned)land_id, height);
}

void bwm_tableau_cam::edit_region_props()
{
  // allowed orientation
  std::vector<std::string> orient_types;
  orient_types = this->set_orient_types();
  // allowed land class
  std::vector<std::string> land_types;
  land_types = this->set_land_types();
  // fetch all regions in depth_map_scene, including all sky regions, ground regions and objects
  std::vector<depth_map_region_sptr> regions = my_observer_->scene_regions();
  // initialize region properties
  static std::map<std::string, unsigned> depth_order;
  static std::map<std::string, double> min_depth;
  static std::map<std::string, double> max_depth;
  static std::map<std::string, double> height;
  static std::map<std::string, unsigned> orient;
  static std::map<std::string, unsigned> land_id;
  static std::map<std::string, bool> active;
  static std::map<std::string, bool> is_reference;
  // for padding to align fields
  unsigned max_string_size = 0;
  for (std::vector<depth_map_region_sptr>::iterator rit = regions.begin();
       rit != regions.end(); ++rit) {
    std::string name  = (*rit)->name();
    if (name.size()>max_string_size)
      max_string_size  = name.size();
    depth_order[name]  = (*rit)->order();
    min_depth[name]    = (*rit)->min_depth();
    max_depth[name]    = (*rit)->max_depth();
    height[name]       = (*rit)->height();
    //depth_inc[name]   = (*rit)->depth_inc();
    active[name]       = (*rit)->active();
    orient[name]       = (*rit)->orient_type();
    land_id[name]      = (*rit)->land_id();
    is_reference[name] = (*rit)->is_ref();
  }
  vgui_dialog_extensions reg_dialog("Scene Region Editor");
  std::vector<depth_map_region_sptr>::iterator gpit;
  for (std::vector<depth_map_region_sptr>::iterator rit = regions.begin(); rit != regions.end(); ++rit) {
    std::string temp = (*rit)->name();
    // compute padding
    int pad_cnt = max_string_size-temp.size();
    for (int k = 0; k<pad_cnt; ++k)
      temp += ' ';
    reg_dialog.message(temp.c_str()) ;
    if ( ((*rit)->name()).find("sky") != std::string::npos ) {
      reg_dialog.line_break();
      continue;
    }
    //if ( ((*rit)->name()).find("ground_plane") != std::string::npos ) {
    //  reg_dialog.choice("Land Class", land_types, land_id[(*rit)->name()]);
    //  reg_dialog.line_break();
    //  continue;
    //}
    reg_dialog.field("Order", depth_order[(*rit)->name()]);
    reg_dialog.field("MinDepth", min_depth[(*rit)->name()]);
    reg_dialog.field("MaxDepth", max_depth[(*rit)->name()]);
    reg_dialog.field("Height",   height[(*rit)->name()]);
    reg_dialog.choice("Orientation", orient_types, orient[(*rit)->name()]);
    reg_dialog.choice("Land Class", land_types, land_id[(*rit)->name()]);
    reg_dialog.checkbox("Active", active[(*rit)->name()]);
    reg_dialog.checkbox("Reference", is_reference[(*rit)->name()]);
    reg_dialog.line_break();
  }

  if (!reg_dialog.ask())
    return;
  // update region properties
  for (std::vector<depth_map_region_sptr>::iterator rit = regions.begin(); rit != regions.end(); ++rit) {
    std::string name = (*rit)->name();
    (*rit)->set_order(depth_order[name]);
    (*rit)->set_min_depth(min_depth[name]);
    (*rit)->set_max_depth(max_depth[name]);
    (*rit)->set_height(height[name]);
    (*rit)->set_land_type(volm_osm_category_io::volm_land_table_name[land_types[land_id[name]]].id_);
    (*rit)->set_active(active[name]);
    (*rit)->set_ref(is_reference[name]);
    (*rit)->set_orient_type(orient[name]);
  }
  //my_observer_->set_ground_plane_max_depth();
}

void bwm_tableau_cam::edit_region_weights()
{
  std::vector<volm_weight> weights;
  // get the depth_map_scene
  depth_map_scene dms = my_observer_->scene();

  // weight vector follows the order defined in depth_map_scene
  std::vector<depth_map_region_sptr> objs = dms.scene_regions();
  unsigned n_obj = (unsigned)objs.size();
  std::sort(objs.begin(), objs.end(), compare_order());

  if (my_observer_->weights().empty()) {
    // calculate average weight as default
    float w_avg, w_obj;
    if (!dms.sky().empty() && !dms.ground_plane().empty()) {
      w_avg = 1.0f / (2.0f + dms.scene_regions().size());
      float w_sky = w_avg * 1.5f;
      float w_grd = w_avg * 1.0f;
      w_obj = (1.0f - w_sky - w_grd) / dms.scene_regions().size();
      weights.push_back(volm_weight("sky", "sky", 0.0f, 0.0f, 0.0f, 1.0f, w_sky));
      weights.push_back(volm_weight("ground_plane", "ground_plane", 0.3f, 0.4f, 0.0f, 0.3f, w_grd));
    }
    else if (!dms.sky().empty()) {
      w_avg = 1.0f / (1 + dms.scene_regions().size());
      float w_sky = w_avg * 1.5f;
      w_obj = (1.0f - w_sky) / dms.scene_regions().size();
      weights.push_back(volm_weight("sky", "sky", 0.0f, 0.0f, 0.0f, 1.0f, w_sky));
    }
    else if (!dms.ground_plane().empty()) {
      w_avg = 1.0f / (1 + dms.scene_regions().size());
      float w_grd = w_avg * 1.0f;
      w_obj = (1.0f - w_grd) / dms.scene_regions().size();
      weights.push_back(volm_weight("ground_plane", "ground_plane", 0.3f, 0.4f, 0.0f, 0.3f, w_grd));
    }
    else {
      w_avg = 1.0f / dms.scene_regions().size();
      w_obj = w_avg;
    }
    for (unsigned i = 0; i < objs.size(); i++)
      weights.push_back(volm_weight(objs[i]->name(), objs[i]->name(), 0.25f, 0.25f, 0.25f, 0.25f, w_obj));
  }
  else {
    weights = my_observer_->weights();
  }
  // arrange the menu by order
  std::vector<depth_map_region_sptr> regions;
  if (!dms.sky().empty())
    regions.push_back(dms.sky()[0]);
  if (!dms.ground_plane().empty())
    regions.push_back(dms.ground_plane()[0]);
  for (unsigned i = 0; i < objs.size(); i++)
    regions.push_back(objs[i]);

  // for padding to align fields
  unsigned max_string_size = 0;
  for (std::vector<depth_map_region_sptr>::iterator rit = regions.begin(); rit != regions.end(); ++rit) {
    std::string tmp;
    if ( ((*rit)->name()).find("sky") != std::string::npos)
      tmp = "sky";
    else if ( ((*rit)->name()).find("ground_plane") != std::string::npos)
      tmp = "ground_plane";
    else
      tmp = (*rit)->name();
    if (tmp.size()> max_string_size)
      max_string_size = tmp.size();
  }

  vgui_dialog_extensions reg_dialog("Scene Region Weight Editor");
  for (unsigned i = 0; i < regions.size(); i++) {
    std::string tmp;
    if (regions[i]->name().find("sky") != std::string::npos)
      tmp = "sky";
    else if (regions[i]->name().find("ground_plane") != std::string::npos)
      tmp = "ground_plane";
    else
      tmp = regions[i]->name();
    // compute padding
    int pad_cnt = max_string_size - tmp.size();
    for (int k = 0; k < pad_cnt; k++)  tmp += ' ';
    reg_dialog.message(tmp.c_str());

    reg_dialog.field("Orientation", weights[i].w_ori_);
    reg_dialog.field("Land_Class",  weights[i].w_lnd_);
    reg_dialog.field("MinDepth",    weights[i].w_dst_);
    reg_dialog.field("Order",       weights[i].w_ord_);
    reg_dialog.field("Object",      weights[i].w_obj_);
    reg_dialog.line_break();
  }

  if (!reg_dialog.ask())
    return;

  // update weight
  my_observer_->set_weights(weights);
}

void bwm_tableau_cam::save_depth_map_scene()
{
  // before save, put the image path into depth_map_scene
  my_observer_->set_image_path(this->img_path());
  // save depth_map_scene
  std::string path = bwm_utils::select_file();
  my_observer_->save_depth_map_scene(path);

  // save associated weight parameters
  std::string dir = vul_file::dirname(path);
  std::string weight_file = dir + "/weight_param.txt";
  my_observer_->save_weight_params(weight_file);
}

std::vector<std::string> bwm_tableau_cam::set_land_types()
{
#if 1
  return volm_osm_category_io::volm_category_name_table;
#endif
#if 0
  std::map<unsigned, volm_land_layer> m;
  std::map<int, volm_land_layer> nlcd_table = volm_osm_category_io::nlcd_land_table;
  std::map<int, volm_land_layer> geo_table = volm_osm_category_io::geo_land_table;
  std::map<std::pair<std::string, std::string>, volm_land_layer> osm_land_table;
  std::map<std::pair<int, int>, volm_land_layer> road_junction_table;
  std::string osm_to_volm_txt = "./osm_to_volm_labels.txt";
  volm_osm_category_io::load_category_table(osm_to_volm_txt, osm_land_table);
  std::string road_junction_txt = "./road_junction_category.txt";
  volm_osm_category_io::load_road_junction_table(road_junction_txt, road_junction_table);

  for (std::map<int, volm_land_layer>::iterator mit = nlcd_table.begin(); mit != nlcd_table.end(); ++mit)
    m.insert(std::pair<unsigned, volm_land_layer>(mit->second.id_, mit->second));

  for (std::map<int, volm_land_layer>::iterator mit = geo_table.begin(); mit != geo_table.end(); mit++)
    m.insert(std::pair<unsigned, volm_land_layer>(mit->second.id_, mit->second));

  for (std::map<std::pair<std::string, std::string>, volm_land_layer>::iterator mit = osm_land_table.begin();
       mit != osm_land_table.end(); ++mit)
    m.insert(std::pair<unsigned, volm_land_layer>(mit->second.id_, mit->second));

  for (std::map<std::pair<int, int>, volm_land_layer>::iterator mit = road_junction_table.begin();
       mit != road_junction_table.end(); ++mit)
    m.insert(std::pair<unsigned, volm_land_layer>(mit->second.id_, mit->second));

  std::vector<std::string> out;
  for (std::map<unsigned, volm_land_layer>::iterator mit = m.begin(); mit != m.end(); ++mit)
    out.push_back(mit->second.name_);
  return out;
#endif

#if 0
  std::vector<std::string> land_types;
  std::map<unsigned char, std::vector<std::string> > temp;
  std::map<int, volm_attributes >::iterator mit = volm_label_table::land_id.begin();
  unsigned cnt = 0;
  for (; mit != volm_label_table::land_id.end(); ++mit) {
    temp[mit->second.id_].push_back(mit->second.name_);
  }
  std::map<unsigned char, std::vector<std::string> >::iterator it = temp.begin();
  for (; it != temp.end(); ++it) {
    std::string land_name;
    for ( std::vector<std::string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
      land_name += (*vit);
      if (vit != it->second.end()-1) land_name += " OR ";
    }
    land_types.push_back(land_name);
  }
  return land_types;
#endif
}

std::vector<std::string> bwm_tableau_cam::set_orient_types()
{
  std::vector<std::string> orient_types;
  std::map<depth_map_region::orientation, std::string> orient_string;
  std::map<std::string, depth_map_region::orientation>::iterator mit = volm_orient_table::ori_id.begin();
  for (; mit != volm_orient_table::ori_id.end(); ++mit)
    orient_string[mit->second] = mit->first;
  std::map<depth_map_region::orientation, std::string>::iterator it = orient_string.begin();
  for (; it != orient_string.end(); ++it)
    orient_types.push_back(it->second);
  return orient_types;
}
