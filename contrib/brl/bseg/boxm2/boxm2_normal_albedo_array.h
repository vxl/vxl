#ifndef boxm2_normal_albedo_array_h_
#define boxm2_normal_albedo_array_h_

#include <vgl/vgl_vector_3d.h>
#include <vcl_vector.h>

class boxm2_normal_albedo_array
{
 public:
  boxm2_normal_albedo_array();

  static vcl_vector<vgl_vector_3d<double> > get_normals();

  float get_albedo(unsigned int i) const {return albedos[i];}
  float get_probability(unsigned int i) const {return probs[i];}

  void set_albedo(unsigned int i, float albedo) {albedos[i] = albedo;}
  void set_probability(unsigned int i, float prob) {probs[i] = prob;}

 protected:
  static const unsigned NUM_NORMALS = 16; // 8 at elevation 0 + 7 at elevation 45 + 1 at elevation 90
  float albedos[NUM_NORMALS];
  float probs[NUM_NORMALS];
};

class boxm2_normal_albedo_array_constants
{
 public:
  static const double sigma_albedo;
  static const double sigma_optical_depth;
  static const double sigma_airlight;
  static const double sigma_skylight;
};

#endif

