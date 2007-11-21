#ifndef bwm_site_h_
#define bwm_site_h_

#include "bwm/io/bwm_io_structs.h"

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vsol/vsol_point_3d.h>

class bwm_site
{
public:

  bwm_site(vcl_string site_name, vcl_string site_dir, 
    vcl_vector<vcl_string> images, 
    vcl_vector<bool> is_pyr, vcl_vector<bool> is_act,
    vcl_vector<vcl_string> levels, 
    vcl_vector<vcl_pair<vcl_string, vcl_string> > objects,
    vsol_point_3d lvcs)
    : name_(site_name), path_(site_dir), image_paths_(images), pyr_(is_pyr), 
    act_(is_act), pyr_levels_(levels), objects_(objects), lvcs_(lvcs) {}

  virtual ~bwm_site();

  void x_write(vcl_ostream& s);

//private:
  // input parameters that come from the site creation dialog
  vcl_string name_;
  vcl_string path_;
  vcl_vector<vcl_string> image_paths_;
  vcl_vector<bool> pyr_;
  vcl_vector<bool> act_;
  vcl_vector<vcl_string> pyr_levels_;
  //: holds <object path, object type>
  vcl_vector<vcl_pair<vcl_string, vcl_string> > objects_;
  vsol_point_3d lvcs_;

  // tableaus set after analyzing the input parameters
  vcl_vector<bwm_io_tab_config* > tableaus_;
};

#endif