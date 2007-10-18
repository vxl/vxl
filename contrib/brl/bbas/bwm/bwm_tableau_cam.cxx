#include "bwm_tableau_cam.h"
#include "bwm_tableau_mgr.h"
#include "bwm_observer_mgr.h"
#include "bwm_observable_mesh.h"
#include "bwm_tableau_text.h"

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

//----------------------------------------------------------------------------
class bwm_create_mesh_command : public vgui_command
{
 public:
  bwm_create_mesh_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->create_polygon_mesh(); /*tab->post_redraw(); */}

  bwm_tableau_cam *tab;
};

class bwm_tri_mesh_command : public vgui_command
{
 public:
  bwm_tri_mesh_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->triangulate_mesh(); /*tab->post_redraw(); */}

  bwm_tableau_cam *tab;
};

//----------------------------------------------------------------------------
class bwm_set_master_command : public vgui_command
{
 public:
  bwm_set_master_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->set_master(); }

  bwm_tableau_cam *tab;
};

//----------------------------------------------------------------------------
class bwm_move_vertex_command : public vgui_command
{
 public:
  bwm_move_vertex_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->move_obj_by_vertex(); }

  bwm_tableau_cam *tab;
};

//----------------------------------------------------------------------------
class bwm_extrude_command : public vgui_command
{
 public:
  bwm_extrude_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->extrude_face(); }

  bwm_tableau_cam *tab;
};

class bwm_divide_command : public vgui_command
{
 public:
  bwm_divide_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->divide_face(); }

  bwm_tableau_cam *tab;
};

class bwm_create_inner_face_command : public vgui_command
{
 public:
  bwm_create_inner_face_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->create_inner_face(); }

  bwm_tableau_cam *tab;
};

class bwm_deselect_all_command : public vgui_command
{
 public:
  bwm_deselect_all_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->deselect_all(); }

  bwm_tableau_cam *tab;
};

class bwm_clear_object_command : public vgui_command
{
 public:
  bwm_clear_object_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->clear_object(); }

  bwm_tableau_cam *tab;
};

class bwm_clear_all_command : public vgui_command
{
 public:
  bwm_clear_all_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->clear_all(); }

  bwm_tableau_cam *tab;
};

class bwm_move_corr_command: public vgui_command
{
  public:
  bwm_move_corr_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->move_corr(); }

  bwm_tableau_cam *tab;
};

class bwm_world_pt_corr_command: public vgui_command
{
  public:
  bwm_world_pt_corr_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->world_pt_corr(); }

  bwm_tableau_cam *tab;
};

class bwm_hist_plot_command : public vgui_command
{
 public:
  bwm_hist_plot_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->hist_plot(); }

  bwm_tableau_cam *tab;
};

class bwm_int_profile_command : public vgui_command
{
 public:
  bwm_int_profile_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->intensity_profile(); }

  bwm_tableau_cam *tab;
};

class bwm_range_map_command : public vgui_command
{
 public:
  bwm_range_map_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->range_map(); }

  bwm_tableau_cam *tab;
};

class bwm_define_proj_plane_command : public vgui_command
{
 public:
  bwm_define_proj_plane_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->define_proj_plane(); }

  bwm_tableau_cam *tab;
};

class bwm_select_proj_plane_command : public vgui_command
{
 public:
  bwm_select_proj_plane_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->select_proj_plane(); }

  bwm_tableau_cam *tab;
};

class bwm_save_command : public vgui_command
{
 public:
  bwm_save_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->save(); }

  bwm_tableau_cam *tab;
};

class bwm_xy_proj_plane_command : public vgui_command
{
 public:
  bwm_xy_proj_plane_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->define_xy_proj_plane(); }

  bwm_tableau_cam *tab;
};

class bwm_yz_proj_plane_command : public vgui_command
{
 public:
  bwm_yz_proj_plane_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->define_yz_proj_plane(); }

  bwm_tableau_cam *tab;
};

class bwm_xz_proj_plane_command : public vgui_command
{
 public:
  bwm_xz_proj_plane_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->define_xz_proj_plane(); }

  bwm_tableau_cam *tab;
};

class bwm_label_roof_command : public vgui_command
{
 public:
  bwm_label_roof_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->label_roof(); }

  bwm_tableau_cam *tab;
};

class bwm_label_wall_command : public vgui_command
{
 public:
  bwm_label_wall_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->label_wall(); }

  bwm_tableau_cam *tab;
};

class bwm_JIMs_command : public vgui_command
{
 public:
  bwm_JIMs_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->JIMs_oper(); }

  bwm_tableau_cam *tab;
};

class bwm_cam_help_command : public vgui_command
{
 public:
  bwm_cam_help_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->help_pop(); }

  bwm_tableau_cam *tab;
};

void bwm_tableau_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu) {
    
  vgui_menu submenu;
  submenu.add( "Set as Master", new bwm_set_master_command(this));

  vgui_menu mesh_submenu;
  mesh_submenu.add("Create..", new bwm_create_mesh_command(this), 
    vgui_key('p'), vgui_modifier(vgui_SHIFT) );
  mesh_submenu.add("Triangulate..", new bwm_tri_mesh_command(this), 
    vgui_key('t'), vgui_modifier(vgui_SHIFT));
  submenu.add( "Mesh..", mesh_submenu);

  submenu.add( "Create Inner Face", new bwm_create_inner_face_command(this));
  submenu.add( "Move Face with Selected Vertex", new bwm_move_vertex_command(this), 
    vgui_key('m'), vgui_modifier(vgui_SHIFT));
  submenu.add( "Extrude Face", new bwm_extrude_command(this), 
    vgui_key('e'), vgui_modifier(vgui_SHIFT));
  submenu.add( "Save All", new bwm_save_command(this), 
    vgui_key('s'), vgui_modifier(vgui_SHIFT));
  submenu.add( "Divide Face", new bwm_divide_command(this));
  
  submenu.add( "Deselect All", new bwm_deselect_all_command(this), 
    vgui_key('-'));
  submenu.add( "Delete Object", new bwm_clear_object_command(this), 
    vgui_key('d'), vgui_modifier(vgui_SHIFT));
  submenu.add( "Delete All", new bwm_clear_all_command(this), 
    vgui_key('a'), vgui_modifier(vgui_SHIFT));

  vgui_menu corr_menu;
  corr_menu.add( "Move (selected)" , new bwm_move_corr_command(this));
  corr_menu.add( "Save World Point (selected)" , new bwm_world_pt_corr_command(this));
  submenu.add( "Correspondence", corr_menu);

  vgui_menu plane_submenu;
  plane_submenu.add( "Define XY Projection Plane", new bwm_xy_proj_plane_command(this));
  plane_submenu.add( "Define YZ Projection Plane", new bwm_yz_proj_plane_command(this));
  plane_submenu.add( "Define XZ Projection Plane", new bwm_xz_proj_plane_command(this));
  plane_submenu.add( "Selected Face", new bwm_select_proj_plane_command(this));
  plane_submenu.add("Projection Plane", plane_submenu);

  vgui_menu label_submenu;
  label_submenu.add( "Label Roof", new bwm_label_roof_command(this));
  label_submenu.add( "Label Wall ", new bwm_label_wall_command(this));
  submenu.add("Label", label_submenu);

  vgui_menu image_submenu;
  image_submenu.add("Histogram Plot", new bwm_hist_plot_command(this));
  image_submenu.add("Intensity Profile", new bwm_int_profile_command(this));
  image_submenu.add("Range Map", new bwm_range_map_command(this));
  image_submenu.add("JIMs menu item...", new bwm_JIMs_command(this));
  submenu.add("Image...", image_submenu);

  submenu.add( "HELP..." , new bwm_cam_help_command(this), 
    vgui_key('h'),vgui_modifier(vgui_SHIFT));

  //add this submenu to the popup menu
  menu.separator();
  menu.add("Modeling Tools", submenu);
}

void bwm_tableau_cam::create_polygon_mesh()
{
  // first lock the bgui_image _tableau
  my_observer_->image_tableau()->lock_linenum(true);
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);
  pick_polygon(poly2d);

  vsol_polygon_3d_sptr poly3d;
  my_observer_->backproj_poly(poly2d, poly3d);
  bwm_observable_mesh_sptr my_polygon = new bwm_observable_mesh();
  bwm_observer_mgr::instance()->attach(my_polygon);
  my_polygon->set_object(poly3d);
  my_observer_->image_tableau()->lock_linenum(false);
}

void bwm_tableau_cam::triangulate_mesh()
{
  my_observer_->triangulate_meshes();
}

void bwm_tableau_cam::set_master()
{
  bwm_observer_mgr::BWM_MASTER_OBSERVER = this->my_observer_;
}

void bwm_tableau_cam::move_obj_by_vertex() {

  // first check if master tableau is set
  bwm_observer_cam* mt = bwm_observer_mgr::BWM_MASTER_OBSERVER;
  if (mt == 0) {
    vcl_cerr << "Master Tableau is not selected, please select one different than the current one!" << vcl_endl;
    return;
  } else if (mt == this->my_observer_) {
    vcl_cerr << "Please select a tableau different than the current one!" << vcl_endl;
    return;
  }

  float x,y;
  set_color(1, 0, 0);
  pick_point(&x, &y);
  vsol_point_2d_sptr pt = new vsol_point_2d((double)x,(double)y);
  my_observer_->move_ground_plane(mt->get_proj_plane(), pt);
}

void bwm_tableau_cam::extrude_face()
{
  float x,y;

  set_color(1, 0, 0);
  pick_point(&x, &y);
  vsol_point_2d_sptr pt = new vsol_point_2d((double)x,(double)y);
  my_observer_->extrude_face(pt);

}

void bwm_tableau_cam::divide_face()
{
  float x1, y1, x2, y2;
  unsigned face_id;

  bwm_observable_sptr obj = my_observer_->selected_face(face_id);
  if (obj) {
    pick_line(&x1, &y1, &x2, &y2);
    my_observer_->divide_face(obj, face_id, x1, y1, x2, y2);
  } else
    vcl_cerr << "Please first select the face to be divided" << vcl_endl;
}

void bwm_tableau_cam::create_inner_face()
{
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);
  pick_polygon(poly2d);
  my_observer_->connect_inner_face(poly2d);
}

void bwm_tableau_cam::deselect_all()
{
  my_observer_->deselect_all();
}
void bwm_tableau_cam::clear_object()
{
  my_observer_->delete_object();
}

void bwm_tableau_cam::clear_all()
{
  my_observer_->delete_all();
}

void bwm_tableau_cam::move_corr()
{
  float x,y;
  set_color(1, 0, 0);
  pick_point(&x, &y);
  vsol_point_2d_sptr pt = new vsol_point_2d((double)x,(double)y);
  my_observer_->move_corr_point(pt);
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
  // pick the ground truth line
  float x1, y1, x2, y2;
  pick_line(&x1, &y1, &x2, &y2);
  vcl_cout << "(" << x1 << "," << y1 << ")" << "(" << x2 << "," << y2 << ")" << vcl_endl;
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

void bwm_tableau_cam::hist_plot()
{
  my_observer_->hist_plot();
}

void bwm_tableau_cam::intensity_profile()
{
  float x1, y1, x2, y2;

  pick_line(&x1, &y1, &x2, &y2);
  vcl_cout << x1 << "," << y1 << "-->" << x2 << "," << y2 << vcl_endl;
  my_observer_->intensity_profile(x1, y1, x2, y2);
}

void bwm_tableau_cam::range_map()
{
  my_observer_->range_map();
}

void bwm_tableau_cam::JIMs_oper()
{
  // if you need to pick a polygon on the tableau: 
  // (if not delete this picking part)
  vsol_polygon_2d_sptr poly2d;
  set_color(1, 0, 0);
  pick_polygon(poly2d);

  my_observer_->JIMs_oper();
}

void bwm_tableau_cam::save()
{
  my_observer_->save();
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
  } else if ( e.key == 'd' && e.modifier == vgui_SHIFT) {
    this->clear_object();
    return true;
  } else if ( e.key == 'a' && e.modifier == vgui_SHIFT) {
    this->clear_all();
    return true;
  } else if ( e.key == 'h' && e.modifier == vgui_SHIFT) {
    this->help_pop();
    return true;
  }
  return bgui_picker_tableau::handle(e);
}
