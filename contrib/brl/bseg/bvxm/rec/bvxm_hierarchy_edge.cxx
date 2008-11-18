//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct. 16, 2008

#include "bvxm_hierarchy_edge.h"
#include <bsta/algo/bsta_gaussian_updater.h>
#include <vcl_cmath.h>

float
//bvxm_hierarchy_edge::prob_density(const vnl_vector_fixed<float,2>& pt)
bvxm_hierarchy_edge::prob_density(const float dist, const float angle)
{
  //: return non-normalized value
  //return float(loc_model_.var()*2*vnl_math::pi*loc_model_.prob_density(pt));
  float d_dens = dist_model_.prob_density(dist);
  float d_var = dist_model_.var();
  d_dens = d_var*2.0f*((float)vnl_math::pi)*d_dens;

  float a_dens = angle_model_.prob_density(angle);
  float a_var = angle_model_.var();
  a_dens = a_var*2.0f*((float)vnl_math::pi)*a_dens;

  return d_dens*a_dens;
}

float
bvxm_hierarchy_edge::prob_density_dist(const float dist)
{
  //: return non-normalized value
  return float(dist_model_.var()*2*vnl_math::pi*dist_model_.prob_density(dist));
}

float
bvxm_hierarchy_edge::prob_density_angle(const float angle)
{
  //: return non-normalized value
  return float(angle_model_.var()*2*vnl_math::pi*angle_model_.prob_density(angle));
}

#if 0
//: if the model is updated then the to_central flag is made false since the edge becomes a non-central edge
void
bvxm_hierarchy_edge::update_model(const vnl_vector_fixed<float,2>& sample)
{
  to_central_ = false;
  bsta_update_gaussian(loc_model_, 1.0f, sample, 25.0f); // min stad_dev = 5 pixels
}
#endif // 0

//: if the model is updated then the to_central flag is made false since the edge becomes a non-central edge
void
bvxm_hierarchy_edge::update_dist_model(const float dist)
{
  to_central_ = false;
  //bsta_update_gaussian(dist_model_, 1.0f, dist, 4.0f);  // min stad_dev = 2 pixels
  bsta_update_gaussian(dist_model_, 1.0f, dist, min_stad_dev_dist_*min_stad_dev_dist_);
}

void
bvxm_hierarchy_edge::update_angle_model(const float angle)
{
  to_central_ = false;
  //float min_stad_dev = (float)(vnl_math::pi*(10.0f/180.0f));  // min stad_dev = 10 degrees
  float min_stad_dev = (float)(vnl_math::pi*(min_stad_dev_angle_/180.0f));  // min stad_dev = 10 degrees
  bsta_update_gaussian(angle_model_, 1.0f, angle, min_stad_dev*min_stad_dev);
}

void
bvxm_hierarchy_edge::calculate_dist_angle(bvxm_part_instance_sptr pi, vnl_vector_fixed<float,2>& dif_to_center, float& dist, float& angle)
{
  // if pi is a composed part we need to get its central part
  bvxm_part_instance_sptr pp = pi;
  while (pp->kind_ == bvxm_part_instance_kind::COMPOSED) {
    pp = pp->central_part()->cast_to_instance();
  }

  vnl_vector_fixed<float, 2> v = pp->direction_vector();  // get orientation vector of central part: pi

  dist = (float)dif_to_center.magnitude();
  vnl_vector_fixed<float, 2> v1_hat = dif_to_center.normalize();
  angle = (float)vcl_acos(dot_product(v, v1_hat));
  return;
}
