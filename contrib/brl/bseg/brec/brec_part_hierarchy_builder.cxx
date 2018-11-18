#include <vector>
#include <iostream>
#include <algorithm>
#include "brec_part_hierarchy_builder.h"
//:
// \file
// \author Ozge C Ozcanli (ozge at lems dot brown dot edu)
// \date Oct. 16, 2008

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "brec_part_hierarchy.h"
#include "brec_part_base_sptr.h"
#include "brec_part_base.h"
#include "brec_part_gaussian.h"

//: For sorting pairs wrt strength
inline bool
strength_more( const brec_part_instance_sptr& left_val,
               const brec_part_instance_sptr& right_val )
{
  return left_val->strength_ > right_val->strength_;
}

//: construct a hierarchy manually
brec_part_hierarchy_sptr brec_part_hierarchy_builder::construct_vehicle_detector()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("roi1_vehicle");

  // LAYER 0: two primitives:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=1.0,lambda1=1.0,theta=0,bright=true)
  h->add_vertex(p_0_0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);  // (lambda0=3.0,lambda1=1.5,theta=45,bright=false)
  h->add_vertex(p_0_0);
  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, true, 0);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 3.0f, 1.5f, 45.0f, false, 1);
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  // LAYER 1: only one layer 1 part
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_0, true);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_1, false);
  p_1_0->add_outgoing_edge(e_1_0_to_second);
  p_0_1->add_incoming_edge(e_1_0_to_second);

  // train this edge with two samples
  vnl_vector_fixed<float,2> sample1(4.0f,0.0f); // center difference measured from the image
  vnl_vector_fixed<float,2> sample2(3.0f,-1.0f); // a second center difference
  // calculate angle and dists for these two samples
  float a1, a2, d1, d2;
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);

  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_dist_model(d2);
  e_1_0_to_second->update_angle_model(a1);
  e_1_0_to_second->update_angle_model(a2);
  //e_1_0_to_second->update_model(sample1);
  //e_1_0_to_second->update_model(sample2);
  h->add_edge_no_check(e_1_0_to_second);

  return h;
}

//: Detector for short vehicles
//  Construct a hierarchy manually for ROI 1
brec_part_hierarchy_sptr
brec_part_hierarchy_builder::construct_detector_roi1_0()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("roi1_vehicle0");
  h->set_model_dir(".\\hierarchy_output_roi1_vehicle0\\");

  // LAYER 0: two primitives:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  h->add_vertex(p_0_0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);  // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  h->add_vertex(p_0_1);
  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, -45.0f, true, 0);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, -45.0f, false, 1);
  pi_0_1->cutoff_percentage_ = 0.5f;
  pi_0_0->detection_threshold_ = 0.0001f;
  pi_0_1->detection_threshold_ = 0.0001f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  // LAYER 1: only one layer 1 part
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  p_1_0->detection_threshold_ = 0.00001f;
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_0, true);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_1, false);
  p_1_0->add_outgoing_edge(e_1_0_to_second);
  p_0_1->add_incoming_edge(e_1_0_to_second);

  // train this edge with two samples
  vnl_vector_fixed<float,2> sample1(1.0f,3.0f); // center difference measured from the image
  vnl_vector_fixed<float,2> sample2(2.0f,2.0f); // a second center difference
  // calculate angle and dists for these two samples
  float a1, a2, d1, d2;
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  //e_1_0_to_second->set_min_stand_dev_angle(30.0f);
  e_1_0_to_second->set_min_stand_dev_angle(10.0f);
  //e_1_0_to_second->set_min_stand_dev_dist(3.0f);
  e_1_0_to_second->set_min_stand_dev_dist(1.0f);
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_dist_model(d2);
  e_1_0_to_second->update_angle_model(a1);
  e_1_0_to_second->update_angle_model(a2);
  h->add_edge_no_check(e_1_0_to_second);

  // create an edge to the third part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_third = new brec_hierarchy_edge(p_1_0, p_0_1, false);
  p_1_0->add_outgoing_edge(e_1_0_to_third);
  p_0_1->add_incoming_edge(e_1_0_to_third);

  // train this edge with two samples
  vnl_vector_fixed<float,2> sample3(-2.0f,-1.0f); // center difference measured from the image
  vnl_vector_fixed<float,2> sample4(-3.0f,-2.0f); // a second center difference
  // calculate angle and dists for these two samples
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d1, a1);
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d2, a2);
  //e_1_0_to_third->set_min_stand_dev_angle(30.0f);
  e_1_0_to_third->set_min_stand_dev_angle(10.0f);
  //e_1_0_to_third->set_min_stand_dev_dist(3.0f);
  e_1_0_to_third->set_min_stand_dev_dist(1.0f);
  e_1_0_to_third->update_dist_model(d1);
  e_1_0_to_third->update_dist_model(d2);
  e_1_0_to_third->update_angle_model(a1);
  e_1_0_to_third->update_angle_model(a2);
  h->add_edge_no_check(e_1_0_to_third);

  return h;
}

//: Detector for longer vehicles
//  Construct a hierarchy manually for ROI 1
brec_part_hierarchy_sptr
brec_part_hierarchy_builder::construct_detector_roi1_1()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("roi1_vehicle1");
  h->set_model_dir(".\\hierarchy_output_roi1_vehicle1\\");

  // LAYER 0: two primitives:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  h->add_vertex(p_0_0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);  // (lambda0=1.0,lambda1=0.5,theta=45,bright=false)
  h->add_vertex(p_0_1);
  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, -45.0f, true, 0);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 45.0f, false, 1);
  pi_0_1->cutoff_percentage_ = 0.5f;
  //brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 45.0f, false, 1);
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  // LAYER 1: only one layer 1 part
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_0, true);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_1, false);
  p_1_0->add_outgoing_edge(e_1_0_to_second);
  p_0_1->add_incoming_edge(e_1_0_to_second);

  vnl_vector_fixed<float,2> c1(557.0f,662.0f); // center measured from the image
  vnl_vector_fixed<float,2> c2(598.0f,583.0f); // center measured from the image
  vnl_vector_fixed<float,2> c3(605.0f,574.0f); // center measured from the image
  vnl_vector_fixed<float,2> c4(421.0f,776.0f); // center measured from the image

  vnl_vector_fixed<float,2> p1c1(563.0f,655.0f); //
  vnl_vector_fixed<float,2> p1c2(600.0f,579.0f); //
  vnl_vector_fixed<float,2> p1c3(609.0f,569.0f); //
  vnl_vector_fixed<float,2> p1c4(423.0f,773.0f); //

  vnl_vector_fixed<float,2> p2c1(553.0f,666.0f); //
  vnl_vector_fixed<float,2> p2c2(593.0f,586.0f); //
  vnl_vector_fixed<float,2> p2c3(600.0f,579.0f); //
  vnl_vector_fixed<float,2> p2c4(418.0f,778.0f); //

  // train this edge
  // subtract central part's center (p_0_0) from this part's center (p_0_1)
  vnl_vector_fixed<float,2> sample1 = c1 - p1c1; // center difference measured from the image
  vnl_vector_fixed<float,2> sample2 = c2 - p1c2; // a second center difference
  vnl_vector_fixed<float,2> sample3 = c3 - p1c3; //
  vnl_vector_fixed<float,2> sample4 = c4 - p1c4; //
  // calculate angle and dists
  float a1, a2, a3, a4, d1, d2, d3, d4;
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d3, a3);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d4, a4);
  //e_1_0_to_second->set_min_stand_dev_dist(10.0f); // 10 pixels
  e_1_0_to_second->set_min_stand_dev_dist(1.0f); // 10 pixels
  //e_1_0_to_second->set_min_stand_dev_angle(10.0f); // 10 degrees
  e_1_0_to_second->set_min_stand_dev_angle(10.0f); // 10 degrees
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_dist_model(d2);
  e_1_0_to_second->update_dist_model(d3);
  e_1_0_to_second->update_dist_model(d4);
  e_1_0_to_second->update_angle_model(a1);
  e_1_0_to_second->update_angle_model(a2);
  e_1_0_to_second->update_angle_model(a3);
  e_1_0_to_second->update_angle_model(a4);
  h->add_edge_no_check(e_1_0_to_second);

  // create an edge to the third part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_third = new brec_hierarchy_edge(p_1_0, p_0_1, false);
  p_1_0->add_outgoing_edge(e_1_0_to_third);
  p_0_1->add_incoming_edge(e_1_0_to_third);

  // train this edge
  // subtract central part's center (p_0_0) from this part's center (p_0_1)
  sample1 = c1 - p2c1; // center difference measured from the image
  sample2 = c2 - p2c2; // a second center difference
  sample3 = c3 - p2c3; //
  sample4 = c4 - p2c4; //
  // calculate angle and dists for these two samples
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d3, a3);
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d4, a4);
  //e_1_0_to_third->set_min_stand_dev_dist(10.0f); // 10 pixels
  e_1_0_to_third->set_min_stand_dev_dist(1.0f); // 10 pixels
  //e_1_0_to_third->set_min_stand_dev_angle(10.0f); // 10 degrees
  e_1_0_to_third->set_min_stand_dev_angle(10.0f); // 10 degrees
  e_1_0_to_third->update_dist_model(d1);
  e_1_0_to_third->update_dist_model(d2);
  e_1_0_to_third->update_dist_model(d3);
  e_1_0_to_third->update_dist_model(d4);
  e_1_0_to_third->update_angle_model(a1);
  e_1_0_to_third->update_angle_model(a2);
  e_1_0_to_third->update_angle_model(a3);
  e_1_0_to_third->update_angle_model(a4);
  h->add_edge_no_check(e_1_0_to_third);

  return h;
}

//: Recognize sides of roads, good to remove artefacts from change map
brec_part_hierarchy_sptr
brec_part_hierarchy_builder::construct_detector_roi1_2()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("roi1_road_side");

  // LAYER 0: one primitive:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=3.0,lambda1=1.0,theta=-45,bright=true)
  h->add_vertex(p_0_0);
  // create a dummy instance
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 3.0f, 1.0f, -45.0f, true, 0);
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());

  // LAYER 1: only one layer 1 part
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_0, true);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_0, false);
  p_1_0->add_outgoing_edge(e_1_0_to_second);
  p_0_0->add_incoming_edge(e_1_0_to_second);

  vnl_vector_fixed<float,2> c1(400.0f,781.0f); // center measured from the image
  vnl_vector_fixed<float,2> c2(379.0f,800.0f); // center measured from the image
  vnl_vector_fixed<float,2> c3(452.0f,728.0f); // center measured from the image
  vnl_vector_fixed<float,2> c4(425.0f,755.0f); // center measured from the image

  vnl_vector_fixed<float,2> p1c1(390.0f,790.0f); //
  vnl_vector_fixed<float,2> p1c2(369.0f,811.0f); //
  vnl_vector_fixed<float,2> p1c3(438.0f,742.0f); //
  vnl_vector_fixed<float,2> p1c4(411.0f,770.0f); //

  // train this edge
  vnl_vector_fixed<float,2> sample1 = c1 - p1c1; // center difference measured from the image
  vnl_vector_fixed<float,2> sample2 = c2 - p1c2; // a second center difference
  vnl_vector_fixed<float,2> sample3 = c3 - p1c3; //
  vnl_vector_fixed<float,2> sample4 = c4 - p1c4; //
  // calculate angle and dists
  float a1, a2, a3, a4, d1, d2, d3, d4;
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d3, a3);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d4, a4);
  e_1_0_to_second->set_min_stand_dev_dist(5.0f); //
  e_1_0_to_second->set_min_stand_dev_angle(3.0f); //
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_dist_model(d2);
  e_1_0_to_second->update_dist_model(d3);
  e_1_0_to_second->update_dist_model(d4);
  e_1_0_to_second->update_angle_model(a1);
  e_1_0_to_second->update_angle_model(a2);
  e_1_0_to_second->update_angle_model(a3);
  e_1_0_to_second->update_angle_model(a4);
  h->add_edge_no_check(e_1_0_to_second);

  // LAYER 2: only one layer 2 part
  brec_part_base_sptr p_2_0 = new brec_part_base(2, 0);
  h->add_vertex(p_2_0);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_2_0_to_central = new brec_hierarchy_edge(p_2_0, p_1_0, true);
  p_2_0->add_outgoing_edge(e_2_0_to_central);
  p_1_0->add_incoming_edge(e_2_0_to_central);
  h->add_edge_no_check(e_2_0_to_central);

  // create an edge to the second part of p_2_0
  brec_hierarchy_edge_sptr e_2_0_to_second = new brec_hierarchy_edge(p_2_0, p_1_0, false);
  p_2_0->add_outgoing_edge(e_2_0_to_second);
  p_1_0->add_incoming_edge(e_2_0_to_second);

  // train this edge
  sample1 = c1 - c2; // center difference measured from the image
  sample2 = c3 - c4; // a second center difference
  // calculate angle and dists
  e_2_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_2_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  e_2_0_to_second->set_min_stand_dev_dist(10.0f); //
  e_2_0_to_second->set_min_stand_dev_angle(3.0f); //
  e_2_0_to_second->update_dist_model(d1);
  e_2_0_to_second->update_dist_model(d2);
  e_2_0_to_second->update_angle_model(a1);
  e_2_0_to_second->update_angle_model(a2);
  h->add_edge_no_check(e_2_0_to_second);

  // LAYER 3: only one layer 3 part
  brec_part_base_sptr p_3_0 = new brec_part_base(3, 0);
  h->add_vertex(p_3_0);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_3_0_to_central = new brec_hierarchy_edge(p_3_0, p_2_0, true);
  p_3_0->add_outgoing_edge(e_3_0_to_central);
  p_2_0->add_incoming_edge(e_3_0_to_central);
  h->add_edge_no_check(e_3_0_to_central);

  // create an edge to the second part of p_3_0
  brec_hierarchy_edge_sptr e_3_0_to_second = new brec_hierarchy_edge(p_3_0, p_2_0, false);
  p_3_0->add_outgoing_edge(e_3_0_to_second);
  p_2_0->add_incoming_edge(e_3_0_to_second);

  // train this edge
  sample1 = c3 - c1; // center difference measured from the image
  // calculate angle and dists
  e_3_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_3_0_to_second->set_min_stand_dev_dist(15.0f); //
  e_3_0_to_second->set_min_stand_dev_angle(5.0f); //
  e_3_0_to_second->update_dist_model(d1);
  e_3_0_to_second->update_angle_model(a1);
  h->add_edge_no_check(e_3_0_to_second);

  return h;
}

//: building 1
brec_part_hierarchy_sptr
brec_part_hierarchy_builder::construct_detector_roi1_3()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("roi1_building3");

  // LAYER 0: two primitives:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=5.0,lambda1=2.5,theta=-40,bright=true)
  h->add_vertex(p_0_0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);  // (lambda0=2.0,lambda1=1.0,theta=45,bright=false)
  h->add_vertex(p_0_1);
  brec_part_base_sptr p_0_2 = new brec_part_base(0, 2);  // (lambda0=14.0,lambda1=4.0,theta=-40,bright=true)
  h->add_vertex(p_0_2);
  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 5.0f, 2.5f, -40.0f, true, 0);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 45.0f, false, 1);
  brec_part_gaussian_sptr pi_0_2 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 14.0f, 4.0f, -40.0f, true, 2);
  pi_0_1->cutoff_percentage_ = 0.5f;
  pi_0_0->detection_threshold_ = 0.001f;
  pi_0_1->detection_threshold_ = 0.001f;
  pi_0_2->detection_threshold_ = 0.001f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_2->cast_to_instance());

  // LAYER 1: only one layer 1 part
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  p_1_0->detection_threshold_ = 0.001f;
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_2, true);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_2->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_0, false);
  p_1_0->add_outgoing_edge(e_1_0_to_second);
  p_0_0->add_incoming_edge(e_1_0_to_second);

  vnl_vector_fixed<float,2> c_0_0_1(331.0f,787.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_0_2(312.0f,804.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_0_3(297.0f,816.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_1_1(284.0f,835.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_2_1(307.0f,811.0f); // center measured from the image

  // train this edge
  vnl_vector_fixed<float,2> sample1 = c_0_0_1 - c_0_2_1; // center difference measured from the image
  // calculate angle and dists
  float a1, d1;
  e_1_0_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->set_min_stand_dev_dist(2.0f);
  e_1_0_to_second->set_min_stand_dev_angle(2.0f);
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_angle_model(a1);
  h->add_edge_no_check(e_1_0_to_second);

  // create an edge to the third part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_third = new brec_hierarchy_edge(p_1_0, p_0_1, false);
  p_1_0->add_outgoing_edge(e_1_0_to_third);
  p_0_1->add_incoming_edge(e_1_0_to_third);

  // train this edge
  sample1 = c_0_1_1 - c_0_2_1; // center difference measured from the image
  // calculate angle and dists for these two samples
  e_1_0_to_third->calculate_dist_angle(pi_0_2->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_third->set_min_stand_dev_dist(1.0f);
  e_1_0_to_third->set_min_stand_dev_angle(2.0f);
  e_1_0_to_third->update_dist_model(d1);
  e_1_0_to_third->update_angle_model(a1);
  h->add_edge_no_check(e_1_0_to_third);

  return h;
}

//: building 2
brec_part_hierarchy_sptr
brec_part_hierarchy_builder::construct_detector_roi1_4()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("roi1_building4");

  // LAYER 0: two primitives:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=4.0,lambda1=2.0,theta=-40,bright=true)
  h->add_vertex(p_0_0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);  // (lambda0=10.0,lambda1=2.0,theta=-35,bright=true)
  h->add_vertex(p_0_1);
  brec_part_base_sptr p_0_2 = new brec_part_base(0, 2);  // (lambda0=2.0,lambda1=1.0,theta=45,bright=false)
  h->add_vertex(p_0_2);

  // create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 4.0f, 2.0f, -40.0f, true, 0);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 10.0f, 2.0f, -35.0f, true, 1);
  brec_part_gaussian_sptr pi_0_2 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 45.0f, false, 2);
  pi_0_0->cutoff_percentage_ = 0.5f;
  pi_0_2->cutoff_percentage_ = 0.5f;
  pi_0_0->detection_threshold_ = 0.01f;
  pi_0_1->detection_threshold_ = 0.01f;
  pi_0_2->detection_threshold_ = 0.00001f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_2->cast_to_instance());

  // LAYER 1: only one layer 1 part
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h->add_vertex(p_1_0);
  p_1_0->detection_threshold_ = 0.0000001f;
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_1, true);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_1->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_0, false);
  p_1_0->add_outgoing_edge(e_1_0_to_second);
  p_0_0->add_incoming_edge(e_1_0_to_second);

  vnl_vector_fixed<float,2> c_0_0_1(34.0f,516.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_0_2(17.0f,529.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_2_1(9.0f,535.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_1_1(34.0f,517.0f); // center measured from the image

  // train this edge
  vnl_vector_fixed<float,2> sample1 = c_0_0_2 - c_0_1_1; // center difference measured from the image
  // calculate angle and dists
  float a1, d1;
  e_1_0_to_second->calculate_dist_angle(pi_0_1->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->set_min_stand_dev_dist(1.0f);
  e_1_0_to_second->set_min_stand_dev_angle(2.0f);
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_angle_model(a1);
  h->add_edge_no_check(e_1_0_to_second);

  // create an edge to the third part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_t = new brec_hierarchy_edge(p_1_0, p_0_2, false);
  p_1_0->add_outgoing_edge(e_1_0_to_t);
  p_0_2->add_incoming_edge(e_1_0_to_t);

  // train this edge
  sample1 = c_0_2_1 - c_0_1_1; // center difference measured from the image
  // calculate angle and dists
  e_1_0_to_t->calculate_dist_angle(pi_0_1->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_t->set_min_stand_dev_dist(1.0f);
  e_1_0_to_t->set_min_stand_dev_angle(2.0f);
  e_1_0_to_t->update_dist_model(d1);
  e_1_0_to_t->update_angle_model(a1);
  h->add_edge_no_check(e_1_0_to_t);

  return h;
}

//: Recognize digit 8
brec_part_hierarchy_sptr
brec_part_hierarchy_builder::construct_eight_detector()
{
  // construct a hierarchy by hand starting from the primitives, construct one to recognize 8
  brec_part_hierarchy_sptr h_8 = new brec_part_hierarchy();
  h_8->set_name("eight_detector");

  //  a hierarchy tells us what types of parts are there at each level in an abstract way (they are not instantiated)

  // LAYER 0: only one primitive: a vertical bar (lambda0=2.0,lambda1=1.0,theta=0.0,bright=false)
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);
  h_8->add_vertex(p_0_1);
  // create a dummy instance and add to h
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 90.0f, false, 1);
  h_8->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  // LAYER 1: only one layer 1 part: 2 vertical bars side by side
  brec_part_base_sptr p_1_0 = new brec_part_base(1, 0);
  h_8->add_vertex(p_1_0);
  // the first child becomes the central part, create an edge to the central part
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_1, true);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_1->add_incoming_edge(e_1_0_to_central);
  h_8->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_1, false);     //  p_1_0
  p_1_0->add_outgoing_edge(e_1_0_to_second);    //                                             / \.
                                               //                                             p_0_1
  p_0_1->add_incoming_edge(e_1_0_to_second);
  // train this edge with two samples
  vnl_vector_fixed<float,2> sample1(5.0f,0.0f); // measured from the 8 sample
  vnl_vector_fixed<float,2> sample2(9.0f,0.0f); // measured from the 8 sample
  float a1, a2, d1, d2;
  e_1_0_to_second->calculate_dist_angle(pi_0_1->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->calculate_dist_angle(pi_0_1->cast_to_instance(), sample2, d2, a2);
  std::cout << "p_1_0 edge to second part training d1: " << d1 << " angle: " << a1*vnl_math::deg_per_rad << " degrees\n"
           << "p_1_0 edge to second part training d2: " << d2 << " angle: " << a2*vnl_math::deg_per_rad << " degrees\n";
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_dist_model(d2);
  e_1_0_to_second->update_angle_model(a1);
  e_1_0_to_second->update_angle_model(a2);

  h_8->add_edge_no_check(e_1_0_to_second);

  // LAYER 2: only 1 layer 2 part: two layer 1 parts on top of each other
  brec_part_base_sptr p_2_0 = new brec_part_base(2, 0);
  h_8->add_vertex(p_2_0);
  // create an edge to the central part
  brec_hierarchy_edge_sptr e_2_0_to_central = new brec_hierarchy_edge(p_2_0, p_1_0, true);
  p_2_0->add_outgoing_edge(e_2_0_to_central);
  p_1_0->add_incoming_edge(e_2_0_to_central);
  h_8->add_edge_no_check(e_2_0_to_central);

  // create an edge to the second part
  brec_hierarchy_edge_sptr e_2_0_to_second = new brec_hierarchy_edge(p_2_0, p_1_0, false);
  p_2_0->add_outgoing_edge(e_2_0_to_second);
  p_1_0->add_incoming_edge(e_2_0_to_second);
  h_8->add_edge(e_2_0_to_second);
  vnl_vector_fixed<float,2> sample3(-1.0f,8.0f); // measured from the 8 sample
  e_2_0_to_second->calculate_dist_angle(pi_0_1->cast_to_instance(), sample3, d1, a1); // using pi_0_1 as it is still the center of p_1_0
  std::cout << "p_2_0 edge to second part training d: " << d1 << " angle: " << a1*vnl_math::deg_per_rad << " degrees\n";
  e_2_0_to_second->update_dist_model(d1);
  e_2_0_to_second->update_angle_model(a1);

  h_8->add_edge_no_check(e_2_0_to_second);

  return h_8;
}

// a detector with one primitive part only
brec_part_hierarchy_sptr brec_part_hierarchy_builder::construct_test_detector()
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  h->set_name("test_detector");

  //: LAYER 0: one primitive:
  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);  // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  h->add_vertex(p_0_0);
  //: create a dummy instance from each and add to h
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, -45.0f, true, 0);
  //brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, -45.0f, false, 0);
  pi_0_0->detection_threshold_ = 0.0001f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());

  return h;
}
