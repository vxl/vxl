//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 16, 2008

#include <iostream>
#include <cmath>
#include "brec_hierarchy_edge.h"
#include <bsta/algo/bsta_gaussian_updater.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_quaternion.h>

#include <bxml/bxml_find.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_cross_product_matrix.h>
#include <vnl/vnl_double_3.h>

double
//brec_hierarchy_edge::prob_density(const vnl_vector_fixed<float,2>& pt)
brec_hierarchy_edge::prob_density(const float dist, const float angle)
{
  //: return non-normalized value
  //return float(loc_model_.var()*vnl_math::twopi*loc_model_.prob_density(pt));
  double d_dens = dist_model_.prob_density(dist);
  double d_var = dist_model_.var();
  d_dens = d_var*2.0f*vnl_math::pi*d_dens;

  double a_dens = angle_model_.prob_density(angle);
  double a_var = angle_model_.var();
  a_dens = a_var*2.0f*(vnl_math::pi)*a_dens;

  //return weight_*d_dens*a_dens;
  return d_dens*a_dens;  // not using weighted density (weights were given by mean-shit mode finding for 1D angle and distance distributions separately)
}

double
brec_hierarchy_edge::prob_density_dist(const float dist)
{
  //: return non-normalized value
  return dist_model_.var()*vnl_math::twopi*dist_model_.prob_density(dist);
}

double
brec_hierarchy_edge::prob_density_angle(const float angle)
{
  //: return non-normalized value
  return angle_model_.var()*vnl_math::twopi*angle_model_.prob_density(angle);
}

#if 0
//: if the model is updated then the to_central flag is made false since the edge becomes a non-central edge
void
brec_hierarchy_edge::update_model(const vnl_vector_fixed<float,2>& sample)
{
  to_central_ = false;
  bsta_update_gaussian(loc_model_, 1.0f, sample, 25.0f); // min stad_dev = 5 pixels
}
#endif // 0

//: if the model is updated then the to_central flag is made false since the edge becomes a non-central edge
void
brec_hierarchy_edge::update_dist_model(const float dist)
{
  to_central_ = false;
  //bsta_update_gaussian(dist_model_, 1.0f, dist, 4.0f);  // min stad_dev = 2 pixels
  bsta_update_gaussian(dist_model_, 1.0f, dist, min_stad_dev_dist_*min_stad_dev_dist_);
}

void
brec_hierarchy_edge::update_angle_model(const float angle)
{
  to_central_ = false;
  //float min_stad_dev = float(vnl_math::pi_over_180)*10.0f;  // min stad_dev = 10 degrees
  auto min_stad_dev = float(vnl_math::pi_over_180*min_stad_dev_angle_);  // min stad_dev = 10 degrees
  bsta_update_gaussian(angle_model_, 1.0f, angle, min_stad_dev*min_stad_dev);
}

void
brec_hierarchy_edge::calculate_dist_angle(const brec_part_instance_sptr& pi, vnl_vector_fixed<float,2>& dif_to_center, float& dist, float& angle)
{
  // if pi is a composed part we need to get its central part
  brec_part_instance_sptr pp = pi;
  while (pp->kind_ == brec_part_instance_kind::COMPOSED) {
    pp = pp->central_part()->cast_to_instance();
  }

  vnl_vector_fixed<float, 2> v = pp->direction_vector();  // get orientation vector of central part: pi

  dist = (float)dif_to_center.magnitude();
  vnl_vector_fixed<float, 2> v1_hat = dif_to_center.normalize();
  angle = (float)std::acos(dot_product(v, v1_hat));

  //: if angle is ~ 180 degrees return a positive angle
  if ((std::abs(angle-vnl_math::pi) < 0.17) || (angle < 0.17))  // allow for a 10 degree interval around 180 degrees and 0 degree
    return;

  //: now we want this angle positive or negative, depending on which side of v does v1 lie
  vnl_double_3 v_3(v[0], v[1], 0.0);
  vnl_double_3 v1_hat_3(v1_hat[0], v1_hat[1], 0.0);

  vnl_double_3x3 V = vnl_cross_product_matrix(v_3);

  vnl_double_3 v_v1_hat = V*v1_hat_3;
  if (v_v1_hat[2] < 0)
    angle = -angle;  // negate the angle

  return;
}

vgl_box_2d<float>
brec_hierarchy_edge::get_probe_box(const brec_part_instance_sptr& central_p)
{
  float cx = central_p->x_; float cy = central_p->y_;

  // if pi is a composed part we need to get its central part
  brec_part_instance_sptr pp = central_p;
  while (pp->kind_ == brec_part_instance_kind::COMPOSED) {
    pp = pp->central_part()->cast_to_instance();
  }
  vnl_vector_fixed<float, 2> v = pp->direction_vector();  // get orientation vector of central part: pi

  //: define a rotation about z axis (in the image plane)
  double mean_angle = this->mean_angle();
  double var_angle = this->var_angle();
  vnl_quaternion<float> q(0.0f, 0.0f, float(mean_angle-var_angle));

  double mean_dist = this->mean_dist();
  vnl_vector_fixed<float,3> v3d(v[0], v[1], 0.0f);
  vnl_vector_fixed<float,3> out = q.rotate(v3d);
  vnl_vector_fixed<float,3> out_dist = out*float(mean_dist);

  float mx = cx + out_dist[0];
  float my = cy + out_dist[1];
  auto rad = (float)std::ceil(std::sqrt(var_dist())+3);
  float si = mx - rad;
  float upper_i = mx + rad;
  float sj = my - rad;
  float upper_j = my + rad;

  vgl_point_2d<float> pr0(si, sj), pr1(si, upper_j), pr2(upper_i, upper_j), pr3(upper_i, sj);
  vgl_box_2d<float> probe;
  probe.add(pr0); probe.add(pr1); probe.add(pr2); probe.add(pr3);

  //: create these boxes for each var_angle() and take union of all boxes
  vnl_quaternion<float> q2(0.0f, 0.0f, float(mean_angle+var_angle));
  vnl_vector_fixed<float,3> out2 = q2.rotate(v3d);
  vnl_vector_fixed<float,3> out_dist2 = out2*float(mean_dist);

  mx = cx + out_dist2[0];
  my = cy + out_dist2[1];
  si = mx - rad;
  upper_i = mx + rad;
  sj = my - rad;
  upper_j = my + rad;
  pr0.set(si, sj); pr1.set(si, upper_j); pr2.set(upper_i, upper_j); pr3.set(upper_i, sj);
  probe.add(pr0); probe.add(pr1); probe.add(pr2); probe.add(pr3);

  return probe;
}

//: samples the position of the part linked with this edge wrt to the position (x,y)
vnl_vector_fixed<float,2> brec_hierarchy_edge::sample_position(const brec_part_instance_sptr& central_p, float x, float y, vnl_random& rng)
{
  // if central_p is a composed part we need to get its central part
  brec_part_instance_sptr pp = central_p;
  while (pp->kind_ == brec_part_instance_kind::COMPOSED) {
    pp = pp->central_part()->cast_to_instance();
  }
  vnl_vector_fixed<float, 2> v = pp->direction_vector();  // get orientation vector of central part

  //: define a rotation about z axis (in the image plane)
  vnl_quaternion<float> q(0.0f, 0.0f, float(angle_model_.sample(rng)));

  vnl_vector_fixed<float,3> v3d(v[0], v[1], 0.0f);
  vnl_vector_fixed<float,3> out = q.rotate(v3d);
  vnl_vector_fixed<float,3> out_dist = out*float(dist_model_.sample(rng));

  float mx = x + out_dist[0];
  float my = y + out_dist[1];
  vnl_vector_fixed<float, 2> out_v(mx, my);
  return out_v;
}

vnl_vector_fixed<float,2> brec_hierarchy_edge::mean_position(const brec_part_instance_sptr& central_p, float x, float y)
{
  // if central_p is a composed part we need to get its central part
  brec_part_instance_sptr pp = central_p;
  while (pp->kind_ == brec_part_instance_kind::COMPOSED) {
    pp = pp->central_part()->cast_to_instance();
  }
  vnl_vector_fixed<float, 2> v = pp->direction_vector();  // get orientation vector of central part

  //: define a rotation about z axis (in the image plane)
  vnl_quaternion<float> q(0.0f, 0.0f, float(angle_model_.mean()));

  vnl_vector_fixed<float,3> v3d(v[0], v[1], 0.0f);
  vnl_vector_fixed<float,3> out = q.rotate(v3d);
  vnl_vector_fixed<float,3> out_dist = out*float(dist_model_.mean());

  float mx = x + out_dist[0];
  float my = y + out_dist[1];
  vnl_vector_fixed<float, 2> out_v(mx, my);
  return out_v;
}

bxml_data_sptr brec_hierarchy_edge::xml_element()
{
  bxml_element* data = new bxml_element("edge");

  data->set_attribute("dist_mean",dist_model_.mean());
  data->set_attribute("dist_var",dist_model_.var());
  data->set_attribute("angle_mean",angle_model_.mean());
  data->set_attribute("angle_var",angle_model_.var());

  data->set_attribute("min_stad_dev_dist",min_stad_dev_dist_);
  data->set_attribute("min_stad_dev_angle",min_stad_dev_angle_);
  data->set_attribute("weight",weight_);

  if (to_central_)
    data->set_attribute("to_central",1);
  else
    data->set_attribute("to_central",0);

  data->append_text("\n ");
  return data;
}

bool brec_hierarchy_edge::xml_parse_element(bxml_data_sptr data)
{
  bxml_element query("edge");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);

  if (!base_root)
    return false;

  float dist_mean, dist_var, angle_mean, angle_var;
  if (base_root->type() == bxml_data::ELEMENT) {
    bool o = (((bxml_element*)base_root.ptr())->get_attribute("dist_mean", dist_mean) &&
             ((bxml_element*)base_root.ptr())->get_attribute("dist_var", dist_var) &&
             ((bxml_element*)base_root.ptr())->get_attribute("angle_mean", angle_mean) &&
             ((bxml_element*)base_root.ptr())->get_attribute("angle_var", angle_var) &&
             ((bxml_element*)base_root.ptr())->get_attribute("weight", weight_) &&
             ((bxml_element*)base_root.ptr())->get_attribute("min_stad_dev_dist", min_stad_dev_dist_) &&
             ((bxml_element*)base_root.ptr())->get_attribute("min_stad_dev_angle", min_stad_dev_angle_));

    int to_central_int;
    o = o && ((bxml_element*)base_root.ptr())->get_attribute("to_central", to_central_int);

    if (!o)
      return false;

    to_central_ = to_central_int == 0 ? false : true;

    dist_model_.set_mean(dist_mean);
    dist_model_.set_var(dist_var);
    angle_model_.set_mean(angle_mean);
    angle_model_.set_var(angle_var);

    return true;
  }
  else
    return false;
}
