#include "bwm_tableau_cam.h"
#include "algo/bwm_utils.h"
//:
// \file
#include "bwm_tableau_mgr.h"
#include "bwm_observer_mgr.h"
#include "bwm_observable_mesh.h"
#include "bwm_tableau_text.h"
#include "bwm_popup_menu.h"
#include "bwm_world.h"
#include "algo/bwm_utils.h"

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

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
  if (! poly2d){
    vcl_cerr << "In bwm_tableau_cam::create_polygon_mesh -"
             << " pick_polygon failed\n";
    return ;
  }
  vsol_polygon_3d_sptr poly3d;
  my_observer_->backproj_poly(poly2d, poly3d);
  bwm_observable_mesh_sptr my_polygon = new bwm_observable_mesh(bwm_observable_mesh::BWM_MESH_FEATURE);
  bwm_world::instance()->add(my_polygon);
  bwm_observer_mgr::instance()->attach(my_polygon);
  my_polygon->set_object(poly3d);
  this->unlock();
}

void bwm_tableau_cam::triangulate_mesh()
{
  my_observer_->triangulate_meshes();
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
  if (mt == 0) {
    vcl_cerr << "Master Tableau is not selected, please select one different than the current one!\n";
    return;
  } else if (mt == this->my_observer_) {
    vcl_cerr << "Please select a tableau different than the current one!\n";
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
    vcl_cerr << "Please first select the face to be divided\n";
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
  vcl_cout << '(' << x1 << ',' << y1 << ')' << '(' << x2 << ',' << y2 << ')' << vcl_endl;
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
  vcl_string file = bwm_utils::select_file();

  // load the mesh from the given file
  bwm_observable_mesh_sptr obj = new bwm_observable_mesh();
  bwm_observer_mgr::instance()->attach(obj);
  obj->load_from(file.data());
}

void bwm_tableau_cam::load_mesh_multiple()
{
  // read txt file
  vcl_string master_filename = bwm_utils::select_file();
  vcl_ifstream file_inp(master_filename.data());
  if (!file_inp.good()) {
    vcl_cerr << "error opening file "<< master_filename <<vcl_endl;
    return;
  }

  while (!file_inp.eof()) {
    vcl_ostringstream fullpath;
    vcl_string mesh_fname;
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

void bwm_tableau_cam::save(vcl_string path)
{
  my_observer_->save(path);
}

void bwm_tableau_cam::save_all()
{
  my_observer_->save();
}

void bwm_tableau_cam::save_all(vcl_string path)
{
  my_observer_->save();
}

void bwm_tableau_cam::scan_regions()
{
  my_observer_->scan_regions();
}

void bwm_tableau_cam::show_geo_position()
{
  my_observer_->show_geo_position();
}

void bwm_tableau_cam::help_pop()
{
  bwm_tableau_text* text = new bwm_tableau_text(500, 500);

  text->set_text("C:\\lems\\lemsvxlsrc\\lemsvxlsrc\\contrib\\bwm\\doc\\doc\\HELP_cam.txt");
  vgui_tableau_sptr v = vgui_viewer2D_tableau_new(text);
  vgui_tableau_sptr s = vgui_shell_tableau_new(v);
  vgui_dialog popup("CAMERA TABLEAU HELP");
  popup.inline_tableau(s, 550, 550);
  if (!popup.ask())
    return;
}

bool bwm_tableau_cam::handle(const vgui_event& e)
{
  //vcl_cout << "Key:" << e.key << " modif: " << e.modifier << vcl_endl;
  if (e.key == 'p' && e.modifier == vgui_SHIFT) {
    create_polygon_mesh();
    return true;
  } else if (e.key == 't' && e.modifier == vgui_SHIFT) {
    this->triangulate_mesh();
    return true;
  } else if ( e.key == 'm' && e.modifier == vgui_SHIFT) {
    this->move_obj_by_vertex();
    return true;
  } else if ( e.key == 'e' && e.modifier == vgui_SHIFT) {
    this->extrude_face();
    return true;
  } else if ( e.key == 's' && e.modifier == vgui_SHIFT) {
    this->save();
    return true;
  } else if ( e.key == '-' && e.modifier == vgui_SHIFT) {
    this->deselect_all();
    return true;
  } else if ( e.key == 'a' && e.modifier == vgui_SHIFT) {
    this->clear_all();
    return true;
  } else if ( e.key == 'h' && e.modifier == vgui_SHIFT) {
    this->help_pop();
    return true;
  }
  return bwm_tableau_img::handle(e);
}

// Private Methods

