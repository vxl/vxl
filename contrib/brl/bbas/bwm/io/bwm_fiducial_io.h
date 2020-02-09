#ifndef bwm_fiducial_io_h_
#define bwm_fiducial_io_h_
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
#include "../bwm_observer_fiducial.h" // for fiducial_corr

class bwm_fiducial_io 
{
 public:

  bwm_fiducial_io(){}

 bwm_fiducial_io(std::string const& site_name, std::string const& image_path, std::string const& fiducial_path, std::vector<fiducial_corr> fid_corrs):site_name_(site_name),
  image_path_(image_path), fiducial_path_(fiducial_path), fid_corrs_(fid_corrs){}
  
  ~bwm_fiducial_io();

  void x_write(std::ostream& s);
  void x_read(std::istream& s){}
  
  std::string site_name_;
  std::string image_path_;
  std::string fiducial_path_;
  std::vector<fiducial_corr> fid_corrs_;
};

#endif
