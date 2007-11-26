#ifndef bwm_site_h_
#define bwm_site_h_

#include "io/bwm_io_structs.h"
#include "bwm_corr_sptr.h"
#include "bwm_corr.h"

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include <vbl/vbl_ref_count.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>

class bwm_site : public vbl_ref_count
{
public:

  bwm_site(){}

  bwm_site(vcl_string site_name, vcl_string site_dir, vcl_vector<vcl_string> images, 
           vcl_vector<bool> is_pyr, vcl_vector<bool> is_act, vcl_vector<vcl_string> levels, 
           vcl_vector<vcl_pair<vcl_string, vcl_string> > objects, vsol_point_3d_sptr lvcs)
    : name_(site_name), path_(site_dir), image_paths_(images), pyr_(is_pyr), 
    act_(is_act), pyr_levels_(levels), objects_(objects), lvcs_(lvcs) {}

  virtual ~bwm_site();

  void add(vcl_vector<vcl_string> images, vcl_vector<bool> is_pyr, vcl_vector<bool> is_act,
           vcl_vector<vcl_string> levels, vcl_vector<vcl_pair<vcl_string, vcl_string> > objects,
           vsol_point_3d_sptr lvcs);

  void tableaus(vcl_vector<bwm_io_tab_config* > &tableaus) { tableaus = tableaus_; }

  //void correspondences(vcl_vector<bwm_corr_sptr> &corr) 
  //{ corr = corresp_; }

  /*vcl_vector<vsol_point_3d> corresp_world_pts() { return corresp_world_pts_; }

  vcl_string corresp_mode() { return corr_mode_; }*/

  void objects(vcl_vector<vcl_pair<vcl_string, vcl_string> > &obj) {obj = objects_; } 

  void x_write(vcl_ostream& s);

  // input parameters that come from the site creation dialog
  vcl_string name_;
  vcl_string path_;
  vcl_vector<vcl_string> image_paths_;
  vcl_vector<bool> pyr_;
  vcl_vector<bool> act_;
  vcl_vector<vcl_string> pyr_levels_;
  vsol_point_3d_sptr lvcs_;

  //: the list of tableaus to be removed, it contains the index to be deleted in the 
  // increasing order, like 3,5,8
  vcl_vector<unsigned> remove_;

//private:
  // tableaus set after analyzing the input parameters
  vcl_vector<bwm_io_tab_config* > tableaus_;

  //: holds <object path, object type>
  vcl_vector<vcl_pair<vcl_string, vcl_string> > objects_;

  //: correspondence points
  vcl_vector<bwm_corr_sptr> corresp_;
  //vcl_vector<vsol_point_3d_sptr> corresp_world_pts_;
  vcl_string corr_mode_;
};


#endif