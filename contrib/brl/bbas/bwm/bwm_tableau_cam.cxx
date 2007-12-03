#include "bwm_tableau_cam.h"
#include "bwm_tableau_mgr.h"
#include "bwm_observer_mgr.h"
#include "bwm_observable_mesh.h"
#include "bwm_tableau_text.h"
#include "bwm_world.h"
#include "algo/bwm_utils.h"

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
  void execute() { tab->create_polygon_mesh(); }

  bwm_tableau_cam *tab;
};

class bwm_tri_mesh_command : public vgui_command
{
 public:
  bwm_tri_mesh_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->triangulate_mesh(); }

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

class bwm_move_corr_command: public vgui_command
{
 public:
  bwm_move_corr_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->move_corr(); }

  bwm_tableau_cam *tab;
};

class bwm_set_corr_to_vertex_command: public vgui_command
{
 public:
  bwm_set_corr_to_vertex_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->set_corr_to_vertex(); }

  bwm_tableau_cam *tab;
};

class bwm_world_pt_corr_command: public vgui_command
{
 public:
  bwm_world_pt_corr_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->world_pt_corr(); }

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

class bwm_save_all_command : public vgui_command
{
 public:
  bwm_save_all_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->save_all(); }

  bwm_tableau_cam *tab;
};

class bwm_delete_command : public vgui_command
{
 public:
  bwm_delete_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->delete_object(); }

  bwm_tableau_cam *tab;
};

class bwm_delete_all_command : public vgui_command
{
 public:
  bwm_delete_all_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->delete_all(); }

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

class bwm_cam_help_command : public vgui_command
{
 public:
  bwm_cam_help_command(bwm_tableau_cam* t) : tab(t) {}
  void execute() { tab->help_pop(); }

  bwm_tableau_cam *tab;
};


void bwm_tableau_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  bwm_tableau_img::get_popup(params, menu);


  vgui_menu mesh_submenu;
  mesh_submenu.add( "Set as Master", new bwm_set_master_command(this));
  mesh_submenu.separator();
  mesh_submenu.add("Create Mesh Polygon", new bwm_create_mesh_command(this),
    vgui_key('p'), vgui_modifier(vgui_SHIFT) );
#if 0
  mesh_submenu.add("Load Mesh from File..",
    new vgui_command_simple<bwm_tableau_cam>(this,&bwm_tableau_cam::load_mesh),
    vgui_key('p'), vgui_modifier(vgui_SHIFT) );
#endif
  mesh_submenu.separator();
  mesh_submenu.add("Triangulate..", new bwm_tri_mesh_command(this),
    vgui_key('t'), vgui_modifier(vgui_SHIFT));
  mesh_submenu.separator();
  mesh_submenu.add( "Create Inner Face", new bwm_create_inner_face_command(this));
  mesh_submenu.separator();
  mesh_submenu.add( "Move Face with Selected Vertex", new bwm_move_vertex_command(this),
    vgui_key('m'), vgui_modifier(vgui_SHIFT));
  mesh_submenu.separator();
  mesh_submenu.add( "Extrude Face", new bwm_extrude_command(this),
    vgui_key('e'), vgui_modifier(vgui_SHIFT));
  mesh_submenu.separator();
  mesh_submenu.add( "Divide Face", new bwm_divide_command(this));
  mesh_submenu.separator();

  vgui_menu del_menu;
  del_menu.add( "Delete Selected", new bwm_delete_command(this));
  del_menu.separator();
  del_menu.add( "Delete All", new bwm_delete_all_command(this));
  del_menu.separator();
  del_menu.add( "Save Selected", new bwm_save_command(this));
  del_menu.separator();
  del_menu.add( "Save All", new bwm_save_all_command(this));
  mesh_submenu.add("SAVE/DELETE", del_menu);
  menu.add("3D OBJECTS", mesh_submenu);

  menu.separator();
  vgui_menu corr_menu;
  corr_menu.add( "Move (selected)" , new bwm_move_corr_command(this));
  corr_menu.separator();
  corr_menu.add( "Set Corr to Selected Vertex" ,
                 new bwm_set_corr_to_vertex_command(this));
  corr_menu.separator();
  corr_menu.add( "Save World Point (selected)" , new bwm_world_pt_corr_command(this));
  menu.add( "Correspondence", corr_menu);

  menu.separator();
  vgui_menu plane_submenu;
  plane_submenu.add( "Define XY Projection Plane", new bwm_xy_proj_plane_command(this));
  plane_submenu.separator();
  plane_submenu.add( "Define YZ Projection Plane", new bwm_yz_proj_plane_command(this));
  plane_submenu.separator();
  plane_submenu.add( "Define XZ Projection Plane", new bwm_xz_proj_plane_command(this));
  plane_submenu.separator();
  plane_submenu.add( "Selected Face", new bwm_select_proj_plane_command(this));
  menu.add("Projection Plane", plane_submenu);

#if 0
  menu.separator();
  vgui_menu label_submenu;
  label_submenu.add( "Label Roof", new bwm_label_roof_command(this));
  label_submenu.separator();
  label_submenu.add( "Label Wall ", new bwm_label_wall_command(this));
  menu.add("Label", label_submenu);

  menu.separator();
  menu.add( "HELP..." , new bwm_cam_help_command(this),
    vgui_key('h'),vgui_modifier(vgui_SHIFT));
#endif
}

void bwm_tableau_cam::create_polygon_mesh()
{
  // first lock the bgui_image _tableau
  my_observer_->image_tableau()->lock_linenum(true);
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
  return bgui_picker_tableau::handle(e);
}

// Private Methods

