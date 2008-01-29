#include "bwm_tableau_mgr.h"
//:
// \file
#include "bwm_observer_mgr.h"
#include "bwm_observer_video.h"
#include "bwm_corr_sptr.h"
#include "bwm_load_commands.h"

#include "algo/bwm_algo.h"
#include "algo/bwm_utils.h"
#include <vcl_cstdio.h>
#include <vsol/vsol_point_2d.h>

#include <bgui3d/bgui3d.h>
#include <bgui/bgui_image_utils.h>

#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_property.h>

bwm_tableau_mgr* bwm_tableau_mgr::instance_ = 0;
vcl_map<vcl_string, bwm_command_sptr> bwm_tableau_mgr::tab_types_;

bwm_tableau_mgr* bwm_tableau_mgr::instance()
{
  if (!instance_) {
    instance_ = new bwm_tableau_mgr();
    bgui3d_init();
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

void bwm_tableau_mgr::add_tableau(vgui_tableau_sptr tab, vcl_string name)
{
#if 0
  //create only if registered
  bwm_load_img_command comm;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(comm.name());
  if (iter == tab_types_.end()) {
    vcl_cerr << "Image tableau type is not registered, not creating!\n";
    return;
  }
#endif

  vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(tab);
  //obs->set_viewer(viewer);
  unsigned row = 0, col = 0;
  add_to_grid(viewer, col, row);
  //obs->set_grid_location(col, row);
  tableaus_[name] = tab;

  /*add_to_grid(tab);
  tableaus_[name] = tab;*/
}

void bwm_tableau_mgr::register_tableau(bwm_command_sptr tab_comm)
{
  tab_types_[tab_comm->name()] = tab_comm;
}

bwm_command_sptr bwm_tableau_mgr::load_tableau_by_type(vcl_string tableau_type)
{
  bwm_command_sptr comm = 0;
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = tab_types_.find(tableau_type);
  if (iter != tab_types_.end())
    comm = iter->second;

  return comm;
}

void bwm_tableau_mgr::add_corresp(vcl_string tab_name, bwm_corr_sptr corr, double X, double Y)
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

  vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter = tableaus_.begin();
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

#if 0
//: Calculate the range parameters for the input image
vgui_range_map_params_sptr bwm_tableau_mgr::
range_params(vil_image_resource_sptr const& image)
{
  float gamma = 1.0;
  bool invert = false;
  bool gl_map = false;
  bool cache = true;

  //Check if the image is blocked
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (bir){
    gl_map = true;
    //cache = true;
    vcl_cout << "image is blocked.\n";
  }else {
    vcl_cout << "image is not blocked.\n";
  }

  //Check if the image is a pyramid
  bool pyr = image->get_property(vil_property_pyramid, 0);
  if (pyr){
    gl_map = true;
    //cache = true;
    vcl_cout << "image is a pyramid.\n";
  }else {
    vcl_cout << "image is not a pyramid\n.";
  }
  //Get max min parameters

  double min=0, max=0;
  unsigned n_components = image->nplanes();
  vgui_range_map_params_sptr rmps;
  if (n_components == 1)
  {
    bgui_image_utils iu(image);
    iu.range(min, max);
    rmps= new vgui_range_map_params(min, max, gamma, invert,
                                    gl_map, cache);
  }
  else if (n_components == 3)
  {
    min = 0; max = 255;//for now - ultimately need to compute color histogram
    rmps = new vgui_range_map_params(min, max, min, max, min, max,
                                     gamma, gamma, gamma, invert,
                                     gl_map, cache);
  }
  return rmps;
}
#endif


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

  // it alternatively adds rows and colums, to equally divide the grid
  if ((tableaus_.size()%2 == 0) && (grid_->rows()*grid_->cols() == tableaus_.size())) {
    if (row_added_) {
      grid_->add_column();
      row_added_ = false;
    } else {
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

vgui_tableau_sptr bwm_tableau_mgr::find_tableau(vcl_string name)
{
  vcl_map<vcl_string, vgui_tableau_sptr>::iterator iter = tableaus_.find(name);
  if (iter != tableaus_.end()) {
    return iter->second;
  }
  return 0;
}

void bwm_tableau_mgr::exit()
{
  vcl_vector<bwm_observer_cam*> obs =
    bwm_observer_mgr::instance()->observers_cam();
  for (vcl_vector<bwm_observer_cam*>::iterator oit = obs.begin();
       oit != obs.end(); ++oit)
    if ((*oit)->type_name() == "bwm_observer_video")
    {
      bwm_observer_video* obv = static_cast<bwm_observer_video*>(*oit);
      obv->stop();
    }
  vgui::quit();
}
