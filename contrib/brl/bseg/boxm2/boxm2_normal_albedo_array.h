#ifndef boxm2_normal_albedo_array_h_
#define boxm2_normal_albedo_array_h_

#include <iostream>
#include <vector>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "boxm2_export.h"

class boxm2_normal_albedo_array
{
 public:
  boxm2_normal_albedo_array();

  static std::vector<vgl_vector_3d<double> > get_normals();

  float get_albedo(unsigned int i) const {return albedos[i];}
  float get_probability(unsigned int i) const {return probs[i];}

  void set_albedo(unsigned int i, float albedo) {albedos[i] = albedo;}
  void set_probability(unsigned int i, float prob) {probs[i] = prob;}

 protected:
  static boxm2_EXPORT_DATA const unsigned NUM_NORMALS = 16; // 8 at elevation 0 + 7 at elevation 45 + 1 at elevation 90
  float albedos[NUM_NORMALS];
  float probs[NUM_NORMALS];
};

class boxm2_normal_albedo_array_constants
{
 public:
  static boxm2_EXPORT_DATA constexpr double sigma_albedo = 0.02;
  static boxm2_EXPORT_DATA constexpr double sigma_optical_depth = 5.0;
  static boxm2_EXPORT_DATA constexpr double sigma_airlight = 0.04;
  static boxm2_EXPORT_DATA constexpr double sigma_skylight = 10.0;
};

#endif
