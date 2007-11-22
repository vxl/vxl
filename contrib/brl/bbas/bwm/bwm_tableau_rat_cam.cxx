#include "bwm_tableau_rat_cam.h"
#include <vsol/vsol_point_2d.h>

bool bwm_tableau_rat_cam::handle(const vgui_event &e) 
{ 
  return bwm_tableau_cam::handle(e); 
}

//----------------------------------------------------------------------------
class bwm_load_lvcs_command : public vgui_command
{
 public:
  bwm_load_lvcs_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->load_lvcs(); }

  bwm_tableau_rat_cam *tab;
};

//----------------------------------------------------------------------------
class bwm_save_lvcs_command : public vgui_command
{
 public:
  bwm_save_lvcs_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->save_lvcs(); }

  bwm_tableau_rat_cam *tab;
};

//----------------------------------------------------------------------------
class bwm_define_lvcs_command : public vgui_command
{
 public:
  bwm_define_lvcs_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->define_lvcs(); }

  bwm_tableau_rat_cam *tab;
};

class bwm_convert_to_lvcs_command : public vgui_command
{
 public:
  bwm_convert_to_lvcs_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->convert_file_to_lvcs(); }

  bwm_tableau_rat_cam *tab;
};

//----------------------------------------------------------------------------
class bwm_save_command : public vgui_command
{
 public:
  bwm_save_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->save(); }

  bwm_tableau_rat_cam *tab;
};

class bwm_save_all_command : public vgui_command
{
 public:
  bwm_save_all_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->save_all(); }

  bwm_tableau_rat_cam *tab;
};

class bwm_save_gml_command : public vgui_command
{
 public:
  bwm_save_gml_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->save_gml(); }

  bwm_tableau_rat_cam *tab;
};

class bwm_save_kml_command : public vgui_command
{
 public:
  bwm_save_kml_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->save_kml(); }

  bwm_tableau_rat_cam *tab;
};

class bwm_save_kml_collada_command : public vgui_command
{
 public:
  bwm_save_kml_collada_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->save_kml_collada(); }

  bwm_tableau_rat_cam *tab;
};

class bwm_save_x3d_command : public vgui_command
{
 public:
  bwm_save_x3d_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->save_x3d(); }

  bwm_tableau_rat_cam *tab;
};

class bwm_adjust_camera_to_world_pt_command : public vgui_command
{
 public:
  bwm_adjust_camera_to_world_pt_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->adjust_camera_to_world_pt(); }

  bwm_tableau_rat_cam *tab;
};

class bwm_adj_cam_offset_command : public vgui_command
{
 public:
  bwm_adj_cam_offset_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->adjust_camera_offset(); }

  bwm_tableau_rat_cam *tab;
};


class bwm_center_pos_command : public vgui_command
{
 public:
  bwm_center_pos_command(bwm_tableau_rat_cam* t) : tab(t) {}
  void execute() { tab->center_pos(); }

  bwm_tableau_rat_cam *tab;
};

void bwm_tableau_rat_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu) 
{
    
  bwm_tableau_cam::get_popup(params, menu);
 
  menu.separator();
  menu.add( "Adjust Camera To LVCS", new bwm_adj_cam_offset_command(this));
  menu.add( "Register Image to World Pt", new bwm_adjust_camera_to_world_pt_command(this));
  menu.add( "Center to the position", new bwm_center_pos_command(this));

  menu.separator();
  vgui_menu lvcs_submenu;
  lvcs_submenu.add( "Load LVCS", new bwm_load_lvcs_command(this));
  lvcs_submenu.add( "Save LVCS", new bwm_save_lvcs_command(this));
  lvcs_submenu.add( "Define LVCS", new bwm_define_lvcs_command(this));
  lvcs_submenu.add( "Convert File to LVCS", new bwm_convert_to_lvcs_command(this));
  menu.add("LVCS", lvcs_submenu);

  menu.separator();
  vgui_menu save_submenu;
  save_submenu.add( "Save Selected", new bwm_save_command(this));
  save_submenu.add( "Save All ", new bwm_save_all_command(this));
  save_submenu.add( "Save gml ", new bwm_save_gml_command(this));
  save_submenu.add( "Save kml ", new bwm_save_kml_command(this));
  save_submenu.add( "Save kml collada ", new bwm_save_kml_collada_command(this));
  save_submenu.add( "Save x3d", new bwm_save_kml_collada_command(this));
  menu.add("SAVE", save_submenu);

  //add this submenu to the popup menu
  //menu.add("Rational Camera Tools", parent_menu);
}

void bwm_tableau_rat_cam::load_lvcs()
{
  my_observer_->load_lvcs();
}

void bwm_tableau_rat_cam::save_lvcs()
{
  my_observer_->save_lvcs();
}

void bwm_tableau_rat_cam::convert_file_to_lvcs()
{
  my_observer_->convert_file_to_lvcs();
}

void bwm_tableau_rat_cam::define_lvcs()
{
  float x1, y1;
  pick_point(&x1, &y1);
  my_observer_->define_lvcs(x1, y1);
}

void bwm_tableau_rat_cam::adjust_camera_offset()
{
  float x1, y1;
  pick_point(&x1, &y1);
  vsol_point_2d_sptr img_point = new vsol_point_2d(x1, y1);
  my_observer_->adjust_camera_offset(img_point);
}

void bwm_tableau_rat_cam::adjust_camera_to_world_pt()
{
  my_observer_->adjust_camera_to_world_pt();
}

void bwm_tableau_rat_cam::center_pos()
{
  my_observer_->center_pos();
}

void bwm_tableau_rat_cam::save() 
{
  my_observer_->save();
}

void bwm_tableau_rat_cam::save_all()
{
  my_observer_->save_all();
}

void bwm_tableau_rat_cam::save_gml()
{
  my_observer_->save_gml();
}

void bwm_tableau_rat_cam::save_kml()
{
  my_observer_->save_kml();
}

void bwm_tableau_rat_cam::save_kml_collada()
{
  my_observer_->save_kml_collada();
}

void bwm_tableau_rat_cam::save_x3d()
{
  my_observer_->save_x3d();
}


