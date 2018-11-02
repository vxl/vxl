#ifndef bwm_site_h_
#define bwm_site_h_
//:
// \file

#include <iostream>
#include <iosfwd>
#include <string>
#include <vector>
#include <utility>
#include "bwm_io_structs.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_ref_count.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

class bwm_site : public vbl_ref_count
{
 public:

  bwm_site(){}

  bwm_site(std::string site_name, std::string site_dir, std::vector<std::string> images,
           std::vector<bool> is_pyr, std::vector<bool> is_act, std::vector<std::string> levels,
           std::vector<std::pair<std::string, std::string> > objects, vsol_point_3d_sptr lvcs)
    : name_(site_name), path_(site_dir), image_paths_(images), pyr_(is_pyr),
    act_(is_act), pyr_levels_(levels), lvcs_(lvcs), objects_(objects) {}

  virtual ~bwm_site();

  void add(std::vector<std::string> images, std::vector<bool> is_pyr, std::vector<bool> is_act,
           std::vector<std::string> levels, std::vector<std::pair<std::string, std::string> > objects,
           vsol_point_3d_sptr lvcs);

  void tableaus(std::vector<bwm_io_tab_config* > &tableaus) { tableaus = tableaus_; }


  void objects(std::vector<std::pair<std::string, std::string> > &obj) {obj = objects_; }

  //: load objects from single OBJ file (groups) and write multiple ply files
  void convert_object_groups_obj_to_multiple_ply(std::string const& obj_path);

  void x_write(std::ostream& s);

  // input parameters that come from the site creation dialog
  std::string name_;
  std::string path_;
  std::string pyr_exe_path_;
  std::vector<std::string> image_paths_;
  std::vector<bool> pyr_;
  std::vector<bool> act_;
  std::vector<std::string> pyr_levels_;
  vsol_point_3d_sptr lvcs_;

  //: the list of tableaux to be removed.
  //  It contains the index to be deleted in increasing order, like 3,5,8
  std::vector<unsigned> remove_;

//private:
  // tableaux set after analyzing the input parameters
  std::vector<bwm_io_tab_config* > tableaus_;

  //: holds <object path, object type>
  std::vector<std::pair<std::string, std::string> > objects_;

  //: correspondence points
std::vector<std::vector<std::pair<std::string, vsol_point_2d> > > corresp_;
  //std::vector<bwm_corr_sptr> corresp_;
  std::vector<vsol_point_3d> corresp_world_pts_;
  std::string corr_mode_;
  std::string corr_type_;
};

#endif
