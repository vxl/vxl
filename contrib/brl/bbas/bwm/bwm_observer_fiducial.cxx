#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "bwm_observer_fiducial.h"
//:
// \file
#include <bwm/bwm_observer_mgr.h>
#include <bwm/algo/bwm_algo.h>
#include <bwm/algo/bwm_image_processor.h>
#include <bwm/bwm_tableau_mgr.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgui/vgui.h>
#include <vnl/vnl_math.h>
#include <bwm/io/bwm_fiducial_io.h>
#include <bwm/io/bwm_io_config_parser.h>
#include <bwm/bwm_site_mgr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui/bgui_vsol_soview2D.h>
#include "bwm_observer_fiducial.h"
#include "bwm_utils.h"
float fiducial_corr::cross_radius_ = 2.0f;
bool bwm_observer_fiducial::add_fiducial_corrs(bwm_io_config_parser* parser){
  if(!parser){
    std::cerr <<"null parser in add_fiducial_corrs" << std::endl;
    return false;
  }
  site_name_ = parser->fid_site_name();
  mode_ = parser->fid_mode();
  type_ = parser->fid_type();
  composite_image_path_ = parser->fid_image_path();
  std::vector<std::pair<float, float> > fid_corrs = parser->fiducial_locations();
  size_t n = fid_corrs.size();
  for(size_t i = 0; i<n; ++i){
    fiducial_corr fc;
    float u = fid_corrs[i].first, v = fid_corrs[i].second;
    bwm_soview2D_cross* cross = new bwm_soview2D_cross(u, v, fiducial_corr::cross_radius_);
    cross->set_style(COMMITED_STYLE);
    fc.soview_ = cross;
    fc.loc_ = vgl_point_2d<double>(u, v);
    vgui_displaylist2D_tableau::add(cross);
    committed_fid_corrs_.push_back(fc);
  }
  return true;
}
bwm_observer_fiducial::bwm_observer_fiducial(bgui_image_tableau_sptr const& img,
                                             std::string const& site_name,
                                             std::string const& image_path,
                                             std::string const& fid_path,
                                             bool display_image_path): bwm_observer_vgui(img)
{
  COMMITED_STYLE =  vgui_style::new_style(0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
  img->show_image_path(display_image_path);
  if(image_path != "" && fid_path == ""){
    site_name_ = site_name;
    composite_image_path_=image_path;
  }else if(image_path == "" && fid_path != ""){
  // user must delete
    bwm_io_config_parser* parser = bwm_site_mgr::instance()->site_parser(fid_path);
    if(parser == nullptr){
      bwm_utils::show_error("Fiducial [" + image_path + "] is NOT found");
      return;
    }
    this->add_fiducial_corrs(parser);
    delete parser;
  }
  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = bwm_utils::load_image(composite_image_path_, params);
  if (!img_res) {
    bwm_utils::show_error("Image [" + image_path + "] is NOT found");
    return;
  }
  img->set_image_resource(img_res, params);
  img->set_file_name(composite_image_path_);
  std::string temp = site_name_;
  if(temp == "")
    temp = image_path;
  set_tab_name(temp);
  bwm_observer_mgr::instance()->add(this);
}
bwm_observer_fiducial::~bwm_observer_fiducial(){
  if(pending_corr_.soview_ != nullptr){
    this->remove(pending_corr_.soview_);
    pending_corr_.soview_ = nullptr;
  }
  for(size_t i = 0; i<committed_fid_corrs_.size(); ++i){
    fiducial_corr& fc = committed_fid_corrs_[i];
    if(fc.soview_ != nullptr){
      this->remove(fc.soview_);
      fc.soview_ = nullptr;
    }
  }
}

bool bwm_observer_fiducial::handle(const vgui_event &e){
  if (e.type==vgui_KEY_PRESS && e.key == 'a')
  {
    this->commit_pending();
    return true;
  }

  if (e.type==vgui_KEY_PRESS && e.key == 'r')
  {
    std::cout << "REMOVE.." << std::endl;
    this->remove_sel_committed();
    return true;
  }

  if (e.type == vgui_BUTTON_DOWN &&
      e.button == vgui_LEFT &&
      e.modifier == vgui_SHIFT &&
      bwm_observer_mgr::instance()->in_corr_picking())
    {
      vgui_projection_inspector pi;
      float x,y;
      pi.window_to_image_coordinates(e.wx, e.wy, x, y);
      vgl_point_2d<double> pt(x,y);
      this->update_pending(pt);
      return true;
    }

  return base::handle(e);
}
void bwm_observer_fiducial::update_pending(vgl_point_2d<double> const& pt){
  if(pending_corr_.soview_ != nullptr){
    this->remove(pending_corr_.soview_);
    pending_corr_.soview_ = nullptr;
  }
  bwm_soview2D_cross* cross = new bwm_soview2D_cross(pt.x(), pt.y(), fiducial_corr::cross_radius_);
  pending_corr_.soview_ = cross;
  pending_corr_.loc_ = pt;
  this->add(cross);
  this->post_redraw();
  std::cout << "UPDATE..." << std::endl;
}
void bwm_observer_fiducial::commit_pending(){
  std::cout << "COMMIT.." << std::endl;
  fiducial_corr fid_corr = pending_corr_;
  if (fid_corr.soview_ == nullptr) {
    std::cerr << "no pending corr to commit" << std::endl;
    return;
  }
  pending_corr_.soview_ = nullptr;
  fid_corr.soview_->set_style(COMMITED_STYLE);
  committed_fid_corrs_.push_back(fid_corr);
  this->post_redraw();
}
void bwm_observer_fiducial::remove_sel_committed(){
  size_t nc = committed_fid_corrs_.size();
  if(nc == 0){
    std::cerr << "no committed fiducial corrsespondences" << std::endl;
    return;
  }
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  size_t n = select_list.size();
  if(n == 0){
    std::cerr << "no fiducial corrsespondence selected to delete" << std::endl;
    return;
  }
  bool found = false;
  for(size_t i = 0; i<n&&!found; ++i)
    for(size_t j = 0; j<committed_fid_corrs_.size()&&!found; ++j){
      if(select_list[i]->get_id() == committed_fid_corrs_[j].soview_->get_id()){
        found = true;
        if(committed_fid_corrs_[j].soview_ != nullptr){
          this->remove(committed_fid_corrs_[j].soview_);
          committed_fid_corrs_[j].soview_ = nullptr;
        }
        committed_fid_corrs_.erase(committed_fid_corrs_.begin() + j);
      }
    }
  this->post_redraw();
}

bool bwm_observer_fiducial::save_fiducial_corrs(std::string path){
  std::ofstream ostr(path.c_str());
  if(!ostr){
    std::cerr << "Can't open " << path << " to write fiducial correspondences" << std::endl;
    return false;
  }
   bwm_fiducial_io fid_io(site_name_, composite_image_path_, fiducial_path_, committed_fid_corrs_);
  fid_io.x_write(ostr);
  return true;
}
void bwm_observer_fiducial::start_fid_corrs(){
  bwm_observer_mgr::instance()->start_corr();
  this->post_redraw();
}

void bwm_observer_fiducial::stop_fid_corrs(){
  bwm_observer_mgr::instance()->stop_corr();
  this->post_redraw();
}
