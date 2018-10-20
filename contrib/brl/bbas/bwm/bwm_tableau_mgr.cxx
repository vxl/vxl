#include "bwm_tableau_mgr.h"
//:
// \file
#include "bwm_observer_mgr.h"
#include "bwm_observer_video.h"
#include "bwm_corr_sptr.h"
#include "bwm_load_commands.h"
#include "bwm_tableau_rat_cam.h"
#include "bwm_tableau_proj_cam.h"
#include "bwm_tableau_img.h"

#include "algo/bwm_algo.h"
#include "algo/bwm_utils.h"

#include <bgui/bgui_image_utils.h>
#include <vgui/vgui_dialog.h>

bwm_tableau_mgr* bwm_tableau_mgr::instance_ = nullptr;
std::map<std::string, bwm_command_sptr> bwm_tableau_mgr::tab_types_;

bwm_tableau_mgr* bwm_tableau_mgr::instance()
{
  if (!instance_) {
    instance_ = new bwm_tableau_mgr();
  }
  return bwm_tableau_mgr::instance_;
}

bwm_tableau_mgr::bwm_tableau_mgr()
{
  grid_ = vgui_grid_tableau_new ();
  grid_->set_frames_selectable(true);
  grid_->set_unique_selected(true);
  grid_->set_grid_size_changeable(true);
  display_image_path_ = false;
  row_added_ = false;
}

void bwm_tableau_mgr::init_env()
{
  if (tableaus_.size() == 0)
    return;

  grid_->set_frames_selectable(true);
  grid_->set_unique_selected(true);
  grid_->set_grid_size_changeable(true);
  display_image_path_ = false;
  row_added_ = false;

  // delete the active tableaux
  tableaus_.clear();

  int rows = grid_->rows();
  int cols = grid_->cols();
  for (int row=0; row<rows; row++)
    grid_->remove_row();
  for (int col=0; col<cols; col++)
    grid_->remove_column();

  grid_->remove_at(0,0);
}

bwm_tableau_mgr::~bwm_tableau_mgr()
{
}

void bwm_tableau_mgr::add_tableau(bwm_tableau_img* tab, std::string name)
{
  //create only if registered
  std::map<std::string, bwm_command_sptr>::iterator iter = tab_types_.find(tab->type_name());
  if (iter == tab_types_.end()) {
    std::cerr << "Tableau type is not registered, not creating!\n";
    return;
  }

  vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(tab);

  unsigned row = 0, col = 0;
  add_to_grid(viewer, col, row);
  tab->set_viewer(viewer);
  tab->set_grid_location(col, row);
  tableaus_[name] = tab;
}

bool bwm_tableau_mgr::is_registered(std::string const& name)
{
  std::map<std::string, bwm_command_sptr>::iterator iter =
    tab_types_.find(name);
  return iter != tab_types_.end();
}

void bwm_tableau_mgr::register_tableau(bwm_command_sptr tab_comm)
{
  tab_types_[tab_comm->name()] = tab_comm;
}

bwm_command_sptr bwm_tableau_mgr::load_tableau_by_type(std::string tableau_type)
{
  bwm_command_sptr comm = nullptr;
  std::map<std::string, bwm_command_sptr>::iterator iter = tab_types_.find(tableau_type);
  if (iter != tab_types_.end())
    comm = iter->second;

  return comm;
}

std::string bwm_tableau_mgr::save_camera(std::string tab_name)
{
  vgui_tableau_sptr tab = find_tableau(tab_name);

  // tableau is not found
  if (!tab) {
    std::cerr << "Tableau " << tab_name << "is not found!\n";
    return "";
  }

  if (tab->type_name().compare("bwm_tableau_rat_cam") == 0) {
    bwm_tableau_rat_cam* tab_cam =
      static_cast<bwm_tableau_rat_cam*> (tab.as_pointer());
    std::string path = tab_cam->save_camera();
    return path;
  }else if(tab->type_name().compare("bwm_tableau_proj_cam") == 0){
    bwm_tableau_proj_cam* tab_cam =
      static_cast<bwm_tableau_proj_cam*> (tab.as_pointer());
    std::string path = tab_cam->save_camera();
    return path;
  }else if(tab->type_name().compare("bwm_tableau_generic_cam") == 0){
    std::cout << " generic cams not currently saved \n";
    return "";
  }else{
    std::cerr << "Tableau " << tab_name << "is an unknown camera tableau!\n";
    return "";
  }
}

void bwm_tableau_mgr::save_cameras()
{
  std::map<std::string, vgui_tableau_sptr>::iterator iter = tableaus_.begin();
  while (iter != tableaus_.end()) {
    save_camera(iter->first);
    iter++;
  }
}

void bwm_tableau_mgr::add_corresp(std::string tab_name, bwm_corr_sptr corr, double X, double Y)
{
  vgui_tableau_sptr tab = this->find_tableau(tab_name);
  if (tab) {
    if ((tab->type_name().compare("bwm_tableau_proj_cam") == 0) ||
        (tab->type_name().compare("bwm_tableau_rat_cam") == 0)) {
          bwm_tableau_cam* tab_cam = static_cast<bwm_tableau_cam*> (tab.as_pointer());
          // ??????tab_cam->add_corresp_cross(X, Y);
          bwm_observer_cam* obs = tab_cam->observer();
          if (obs) {
            corr->set_match(obs, X, Y);
            obs->add_cross(X, Y, 3);
          }
    }
  }
}

void bwm_tableau_mgr::remove_tableau()
{
  unsigned int col, row;
  grid_->get_last_selected_position(&col, &row);
  grid_->set_selected(row, col, false);
  vgui_tableau_sptr tab = grid_->get_tableau_at(col, row);

  std::map<std::string, vgui_tableau_sptr>::iterator iter = tableaus_.begin();
  while (iter != tableaus_.end()) {
    if ((iter->second == tab) || (tab->get_child(0) == iter->second)) {
      tableaus_.erase(iter);
      grid_->remove_at(col, row);
      return;
    }
    iter++;
  }
  grid_->layout_grid2();
}

vgui_tableau_sptr bwm_tableau_mgr::active_tableau()
{
  unsigned col_pos, row_pos;
  grid_->get_last_selected_position(&col_pos, &row_pos);
  vgui_tableau_sptr tab = grid_->get_tableau_at(col_pos, row_pos);
  return tab;
}

//: manages creating new tableaux on the grid.
// Decides on the layout of the grid based on the number of current tableaux.
void bwm_tableau_mgr::add_to_grid(vgui_tableau_sptr tab, unsigned& col,
                                  unsigned& row)
{
   if (tableaus_.size() == 0)
  {
    grid_->add_next(tab, col, row);
    return;
   }

  // it alternatively adds rows and columns, to equally divide the grid
  if ((tableaus_.size()%2 == 0) && (grid_->rows()*grid_->cols() == tableaus_.size())) {
    if (row_added_) {
      grid_->add_column();
      row_added_ = false;
    }
    else {
      grid_->add_row();
      row_added_ = true;
    }
  }
  grid_->add_next(tab, col, row);
}

void bwm_tableau_mgr::add_to_grid(vgui_tableau_sptr tab)
{
  unsigned row = 0, col=0;
  this->add_to_grid(tab, col, row);
}

vgui_tableau_sptr bwm_tableau_mgr::find_tableau(std::string name)
{
  std::map<std::string, vgui_tableau_sptr>::iterator iter = tableaus_.find(name);
  if (iter != tableaus_.end()) {
    return iter->second;
  }
  return nullptr;
}

void bwm_tableau_mgr::exit()
{
  std::vector<bwm_observer_cam*> obs =
    bwm_observer_mgr::instance()->observers_cam();
  for (std::vector<bwm_observer_cam*>::iterator oit = obs.begin();
       oit != obs.end(); ++oit)
    if ((*oit)->type_name() == "bwm_observer_video")
    {
      bwm_observer_video* obv = static_cast<bwm_observer_video*>(*oit);
      obv->stop();
    }
  vgui::quit();
}

void bwm_tableau_mgr::set_draw_mode_vertex()
{
  set_observer_draw_mode(bwm_observer::MODE_VERTEX);
}

void bwm_tableau_mgr::set_draw_mode_edge()
{
  this->set_observer_draw_mode(bwm_observer::MODE_EDGE);
}

void bwm_tableau_mgr::set_draw_mode_face()
{
  this->set_observer_draw_mode(bwm_observer::MODE_POLY);
}

void bwm_tableau_mgr::set_draw_mode_mesh()
{
  set_observer_draw_mode(bwm_observer::MODE_MESH);
}

void bwm_tableau_mgr::set_observer_draw_mode(int mode)
{
  std::vector<bwm_observer_cam*> obs =
    bwm_observer_mgr::instance()->observers_cam();
  for (unsigned i=0; i<obs.size(); i++)
  {
    obs[i]->set_draw_mode((bwm_observer::BWM_DRAW_MODE)mode);
  }
}

void bwm_tableau_mgr::zoom_to_fit()
{
  std::map<std::string, vgui_tableau_sptr>::iterator iter = tableaus_.begin();
  while (iter != tableaus_.end()) {
    bwm_tableau_img* tab = static_cast<bwm_tableau_img*> (iter->second.as_pointer());
    tab->zoom_to_fit();
    iter++;
  }
}

void bwm_tableau_mgr::scroll_to_point()
{
  double lx,ly,lz;
  /*ly = 12.952359;
  lx = 77.589078;
  lz = 850.0;*/
  ly = 32.3338678324;
  lx = 36.1991163567;
  lz = 750;

  vgui_dialog dialog("Point");
  dialog.message("3D World Point: ");
  dialog.field("lon:", lx);
  dialog.field("lat:", ly);
  dialog.field("elev:", lz);
  if (!dialog.ask())
    return;

  std::map<std::string, vgui_tableau_sptr>::iterator iter = tableaus_.begin();
  while (iter != tableaus_.end()) {
    bwm_tableau_cam* tab = dynamic_cast<bwm_tableau_cam*> (iter->second.as_pointer());
    if (tab)
      tab->scroll_to_point(lx,ly,lz);
    iter++;
  }
}
