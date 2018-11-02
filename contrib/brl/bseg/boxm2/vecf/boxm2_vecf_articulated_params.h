#pragma once
#include <iostream>
#include <string>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//:
// \file
// \brief  Parent class for articulated parameters used in articulated scenes
//
// \author O. T. Biris
// \date   2 Jul 2015
//
class boxm2_vecf_articulated_params{
 public:
  boxm2_vecf_articulated_params(){
      app_.fill(static_cast<unsigned char>(0));
      app_[1]=static_cast<unsigned char>(32); app_[2] = static_cast<unsigned char>(255);
  }
  virtual ~boxm2_vecf_articulated_params()= default;;

  // internal voxel processing parameters
  double neighbor_radius() const {return 1.7320508075688772;}
  double gauss_sigma() const {return 3;}
  //:members
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app_;
};
