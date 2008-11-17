//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/08
//
//

#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vil/vil_image_resource.h>

#include <rec/bvxm_part_hierarchy_builder.h>
#include <rec/bvxm_part_hierarchy.h>
#include <rec/bvxm_part_base_sptr.h>
#include <rec/bvxm_part_base.h>
#include <rec/bvxm_part_gaussian.h>

//: For sorting pairs wrt strength
inline bool
strength_more( const bvxm_part_instance_sptr& left,
               const bvxm_part_instance_sptr& right )
{
  return (left->strength_ > right->strength_);
}
/*
bvxm_part_hierarchy_sptr bvxm_part_hierarchy_builder::construct_candidates_from_one_image(vil_image_resource_sptr img, float min_strength)
{
  //: extract all the primitives
  float lambda0 = 2.0f;
  float lambda1 = 1.0f;
  float theta = 0.0f;
  bool bright = false;
  vcl_vector<bvxm_part_instance_sptr> parts;
  extract_gaussian_primitives(img, lambda0, lambda1, theta, bright, 0.1f, 0, parts);
  theta = 90.0f;
  extract_gaussian_primitives(img, lambda0, lambda1, theta, bright, 0.1f, 1, parts);
  theta = 45.0f;
  extract_gaussian_primitives(img, lambda0, lambda1, theta, bright, 0.1f, 2, parts);
  theta = 135.0f;
  extract_gaussian_primitives(img, lambda0, lambda1, theta, bright, 0.1f, 3, parts);

  //: now sort primitives wrt strength
  vcl_sort(parts.begin(), parts.end(), strength_more);
  //: erase the ones with less than min_strength
  vcl_vector<bvxm_part_instance_sptr>::reverse_iterator it = parts.rbegin();
  for ( ; it != parts.rend(); it++) {
    if ((*it)->strength_ > min_strength)
      break;
  }
  parts.erase(it.base(), parts.end());

  bvxm_part_hierarchy_sptr h = new bvxm_part_hierarchy();
  //: add the primitive types
  bvxm_part_base_sptr p_0_0 = new bvxm_part_base(0, 0);  
  bvxm_part_base_sptr p_0_1 = new bvxm_part_base(0, 1);  
  bvxm_part_base_sptr p_0_2 = new bvxm_part_base(0, 2);  
  bvxm_part_base_sptr p_0_3 = new bvxm_part_base(0, 3);  
  h->add_vertex(p_0_0);
  h->add_vertex(p_0_1);
  h->add_vertex(p_0_2);
  h->add_vertex(p_0_3);

  construct_layer_candidates(1, h, parts); // pairs 

  //: now detect layer 1 instances and construct the second layer

  //: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
  vcl_vector<bvxm_part_instance_sptr> upper_parts;
  h->extract_upper_layer(parts, img->ni(), img->nj(), 0.1f, upper_parts);

  //: now sort primitives wrt strength
  vcl_sort(upper_parts.begin(), upper_parts.end(), strength_more);
  //: erase the ones with less than min_strength
  it = upper_parts.rbegin();
  for ( ; it != upper_parts.rend(); it++) {
    if ((*it)->strength_ > min_strength)
      break;
  }
  upper_parts.erase(it.base(), upper_parts.end());

  construct_layer_candidates(2, h, upper_parts); // pairs of pairs

  return h;
}
/*
//: assuming parts array contains detected parts of layer n-1
//  construct layer_n from all pairwise combinations of detected parts of layer_n-1
bool bvxm_part_hierarchy_builder::construct_layer_candidates(unsigned layer_n, bvxm_part_hierarchy_sptr& h, vcl_vector<bvxm_part_instance_sptr>& parts)
{
  if (layer_n-1 < 0)
    return false;
 
  unsigned layer_below = layer_n-1;

  //: make pairs from all combinations
  unsigned k = 0;
  for (unsigned i = 0; i < parts.size(); i++) {
    bvxm_part_instance_sptr pi = parts[i];
    bvxm_part_base_sptr p_0_c = h->get_node(layer_below, pi->type_);
    
    for (unsigned j = i+1; j < parts.size(); j++) {
      bvxm_part_instance_sptr pj = parts[j];
      bvxm_part_base_sptr p_0_s = h->get_node(layer_below, pj->type_);
      vnl_vector_fixed<float,2> sample(pj->x_ - pi->x_, pj->y_ - pi->y_);

      //: LAYER 1: 
      bvxm_part_base_sptr p_1_k = new bvxm_part_base(layer_n, k);  
      k++;
      h->add_vertex(p_1_k);
      
      //: the first child becomes the central part, create an edge to the central part
      bvxm_hierarchy_edge_sptr e_1_k_to_central = new bvxm_hierarchy_edge(p_1_k, p_0_c);
      p_1_k->add_outgoing_edge(e_1_k_to_central);
      p_0_c->add_incoming_edge(e_1_k_to_central);
      h->add_edge_no_check(e_1_k_to_central);

      //: create an edge to the second part of p_1_k
      bvxm_hierarchy_edge_sptr e_1_k_to_second = new bvxm_hierarchy_edge(p_1_k, p_0_s);    
      p_1_k->add_outgoing_edge(e_1_k_to_second);    //                                             
      p_0_s->add_incoming_edge(e_1_k_to_second);
      //: train this edge with the sample
      e_1_k_to_second->update_model(sample);
      h->add_edge_no_check(e_1_k_to_second);
    }
  }

  return true;
}
*/

//: construct a hierarchy manually
bvxm_part_hierarchy_sptr bvxm_part_hierarchy_builder::construct_vehicle_detector()
{
  bvxm_part_hierarchy_sptr h = new bvxm_part_hierarchy();

  //: LAYER 0: two primitives: 
  bvxm_part_base_sptr p_0_0 = new bvxm_part_base(0, 0);  // (lambda0=1.0,lambda1=1.0,theta=0,bright=true)
  h->add_vertex(p_0_0);
  bvxm_part_base_sptr p_0_1 = new bvxm_part_base(0, 1);  // (lambda0=3.0,lambda1=1.5,theta=45,bright=false)
  h->add_vertex(p_0_0);
  //: create a dummy instance from each and add to h
  bvxm_part_gaussian_sptr pi_0_0 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, true, 0);
  bvxm_part_gaussian_sptr pi_0_1 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 3.0f, 1.5f, 45.0f, false, 1);
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());
  
  //: LAYER 1: only one layer 1 part
  bvxm_part_base_sptr p_1_0 = new bvxm_part_base(1, 0);  
  h->add_vertex(p_1_0);
  //: the first child becomes the central part, create an edge to the central part
  bvxm_hierarchy_edge_sptr e_1_0_to_central = new bvxm_hierarchy_edge(p_1_0, p_0_0);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  //: create an edge to the second part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_second = new bvxm_hierarchy_edge(p_1_0, p_0_1);     
  p_1_0->add_outgoing_edge(e_1_0_to_second);        
  p_0_1->add_incoming_edge(e_1_0_to_second);
  
  //: train this edge with two samples
  vnl_vector_fixed<float,2> sample1(4.0f,0.0f); // center difference measured from the image
  vnl_vector_fixed<float,2> sample2(3.0f,-1.0f); // a second center difference
  //: calculate angle and dists for these two samples
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

//: construct a hierarchy manually for ROI 1
//: detector for short vehicles
bvxm_part_hierarchy_sptr 
bvxm_part_hierarchy_builder::construct_vehicle_detector_roi1_0()
{
  bvxm_part_hierarchy_sptr h = new bvxm_part_hierarchy();

  //: LAYER 0: two primitives: 
  bvxm_part_base_sptr p_0_0 = new bvxm_part_base(0, 0);  // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  h->add_vertex(p_0_0);
  bvxm_part_base_sptr p_0_1 = new bvxm_part_base(0, 1);  // (lambda0=2.0,lambda1=1.0,theta=-45,bright=false)
  h->add_vertex(p_0_1);
  //: create a dummy instance from each and add to h
  bvxm_part_gaussian_sptr pi_0_0 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, -45.0f, true, 0);
  bvxm_part_gaussian_sptr pi_0_1 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, -45.0f, false, 1);
  pi_0_1->cutoff_percentage_ = 0.5f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  //: LAYER 1: only one layer 1 part
  bvxm_part_base_sptr p_1_0 = new bvxm_part_base(1, 0);  
  h->add_vertex(p_1_0);
  //: the first child becomes the central part, create an edge to the central part
  bvxm_hierarchy_edge_sptr e_1_0_to_central = new bvxm_hierarchy_edge(p_1_0, p_0_0);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  //: create an edge to the second part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_second = new bvxm_hierarchy_edge(p_1_0, p_0_1);     
  p_1_0->add_outgoing_edge(e_1_0_to_second);        
  p_0_1->add_incoming_edge(e_1_0_to_second);
  
  //: train this edge with two samples
  vnl_vector_fixed<float,2> sample1(1.0f,3.0f); // center difference measured from the image
  vnl_vector_fixed<float,2> sample2(2.0f,2.0f); // a second center difference
  //: calculate angle and dists for these two samples
  float a1, a2, d1, d2;
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_dist_model(d2);
  e_1_0_to_second->update_angle_model(a1);
  e_1_0_to_second->update_angle_model(a2);
  h->add_edge_no_check(e_1_0_to_second);

  //: create an edge to the third part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_third = new bvxm_hierarchy_edge(p_1_0, p_0_1);     
  p_1_0->add_outgoing_edge(e_1_0_to_third);        
  p_0_1->add_incoming_edge(e_1_0_to_third);
  
  //: train this edge with two samples
  vnl_vector_fixed<float,2> sample3(-2.0f,-1.0f); // center difference measured from the image
  vnl_vector_fixed<float,2> sample4(-3.0f,-2.0f); // a second center difference
  //: calculate angle and dists for these two samples
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d1, a1);
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d2, a2);
  e_1_0_to_third->update_dist_model(d1);
  e_1_0_to_third->update_dist_model(d2);
  e_1_0_to_third->update_angle_model(a1);
  e_1_0_to_third->update_angle_model(a2);
  h->add_edge_no_check(e_1_0_to_third);

  return h;
}

//: construct a hierarchy manually for ROI 1
//: detector for longer vehicles
bvxm_part_hierarchy_sptr 
bvxm_part_hierarchy_builder::construct_vehicle_detector_roi1_1()
{
  bvxm_part_hierarchy_sptr h = new bvxm_part_hierarchy();

  //: LAYER 0: two primitives: 
  bvxm_part_base_sptr p_0_0 = new bvxm_part_base(0, 0);  // (lambda0=2.0,lambda1=1.0,theta=-45,bright=true)
  h->add_vertex(p_0_0);
  bvxm_part_base_sptr p_0_1 = new bvxm_part_base(0, 1);  // (lambda0=1.0,lambda1=0.5,theta=45,bright=false)
  h->add_vertex(p_0_1);
  //: create a dummy instance from each and add to h
  bvxm_part_gaussian_sptr pi_0_0 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, -45.0f, true, 0);
  bvxm_part_gaussian_sptr pi_0_1 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 45.0f, false, 1);
  pi_0_1->cutoff_percentage_ = 0.5f;
  //bvxm_part_gaussian_sptr pi_0_1 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 45.0f, false, 1);
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  //: LAYER 1: only one layer 1 part
  bvxm_part_base_sptr p_1_0 = new bvxm_part_base(1, 0);  
  h->add_vertex(p_1_0);
  //: the first child becomes the central part, create an edge to the central part
  bvxm_hierarchy_edge_sptr e_1_0_to_central = new bvxm_hierarchy_edge(p_1_0, p_0_0);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  //: create an edge to the second part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_second = new bvxm_hierarchy_edge(p_1_0, p_0_1);     
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

  //: train this edge 
  vnl_vector_fixed<float,2> sample1 = c1 - p1c1; // center difference measured from the image
  vnl_vector_fixed<float,2> sample2 = c2 - p1c2; // a second center difference
  vnl_vector_fixed<float,2> sample3 = c3 - p1c3; // 
  vnl_vector_fixed<float,2> sample4 = c4 - p1c4; // 
  //: calculate angle and dists 
  float a1, a2, a3, a4, d1, d2, d3, d4;
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d3, a3);
  e_1_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d4, a4);
  e_1_0_to_second->set_min_stand_dev_dist(10.0f); // 10 pixels
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

  //: create an edge to the third part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_third = new bvxm_hierarchy_edge(p_1_0, p_0_1);     
  p_1_0->add_outgoing_edge(e_1_0_to_third);        
  p_0_1->add_incoming_edge(e_1_0_to_third);
  
  //: train this edge 
  sample1 = c1 - p2c1; // center difference measured from the image
  sample2 = c2 - p2c2; // a second center difference
  sample3 = c3 - p2c3; // 
  sample4 = c4 - p2c4; // 
  //: calculate angle and dists for these two samples
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample3, d3, a3);
  e_1_0_to_third->calculate_dist_angle(pi_0_0->cast_to_instance(), sample4, d4, a4);
  e_1_0_to_third->set_min_stand_dev_dist(10.0f); // 10 pixels
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

// recognize sides of roads, good to remove artifacts from change map
bvxm_part_hierarchy_sptr 
bvxm_part_hierarchy_builder::construct_vehicle_detector_roi1_2()
{
  bvxm_part_hierarchy_sptr h = new bvxm_part_hierarchy();

  //: LAYER 0: one primitive: 
  bvxm_part_base_sptr p_0_0 = new bvxm_part_base(0, 0);  // (lambda0=3.0,lambda1=1.0,theta=-45,bright=true)
  h->add_vertex(p_0_0);
  //: create a dummy instance
  bvxm_part_gaussian_sptr pi_0_0 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 3.0f, 1.0f, -45.0f, true, 0);
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());

  //: LAYER 1: only one layer 1 part
  bvxm_part_base_sptr p_1_0 = new bvxm_part_base(1, 0);  
  h->add_vertex(p_1_0);
  //: the first child becomes the central part, create an edge to the central part
  bvxm_hierarchy_edge_sptr e_1_0_to_central = new bvxm_hierarchy_edge(p_1_0, p_0_0);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_0->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  //: create an edge to the second part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_second = new bvxm_hierarchy_edge(p_1_0, p_0_0);     
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

  //: train this edge 
  vnl_vector_fixed<float,2> sample1 = c1 - p1c1; // center difference measured from the image
  vnl_vector_fixed<float,2> sample2 = c2 - p1c2; // a second center difference
  vnl_vector_fixed<float,2> sample3 = c3 - p1c3; // 
  vnl_vector_fixed<float,2> sample4 = c4 - p1c4; // 
  //: calculate angle and dists 
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

  //: LAYER 2: only one layer 2 part
  bvxm_part_base_sptr p_2_0 = new bvxm_part_base(2, 0);  
  h->add_vertex(p_2_0);
  //: the first child becomes the central part, create an edge to the central part
  bvxm_hierarchy_edge_sptr e_2_0_to_central = new bvxm_hierarchy_edge(p_2_0, p_1_0);
  p_2_0->add_outgoing_edge(e_2_0_to_central);
  p_1_0->add_incoming_edge(e_2_0_to_central);
  h->add_edge_no_check(e_2_0_to_central);

  //: create an edge to the second part of p_2_0
  bvxm_hierarchy_edge_sptr e_2_0_to_second = new bvxm_hierarchy_edge(p_2_0, p_1_0);     
  p_2_0->add_outgoing_edge(e_2_0_to_second);        
  p_1_0->add_incoming_edge(e_2_0_to_second);
  
  //: train this edge 
  sample1 = c1 - c2; // center difference measured from the image
  sample2 = c3 - c4; // a second center difference
  //: calculate angle and dists 
  e_2_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_2_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample2, d2, a2);
  e_2_0_to_second->set_min_stand_dev_dist(10.0f); // 
  e_2_0_to_second->set_min_stand_dev_angle(3.0f); // 
  e_2_0_to_second->update_dist_model(d1);
  e_2_0_to_second->update_dist_model(d2);
  e_2_0_to_second->update_angle_model(a1);
  e_2_0_to_second->update_angle_model(a2);
  h->add_edge_no_check(e_2_0_to_second);

  //: LAYER 3: only one layer 3 part
  bvxm_part_base_sptr p_3_0 = new bvxm_part_base(3, 0);  
  h->add_vertex(p_3_0);
  //: the first child becomes the central part, create an edge to the central part
  bvxm_hierarchy_edge_sptr e_3_0_to_central = new bvxm_hierarchy_edge(p_3_0, p_2_0);
  p_3_0->add_outgoing_edge(e_3_0_to_central);
  p_2_0->add_incoming_edge(e_3_0_to_central);
  h->add_edge_no_check(e_3_0_to_central);

  //: create an edge to the second part of p_3_0
  bvxm_hierarchy_edge_sptr e_3_0_to_second = new bvxm_hierarchy_edge(p_3_0, p_2_0);     
  p_3_0->add_outgoing_edge(e_3_0_to_second);        
  p_2_0->add_incoming_edge(e_3_0_to_second);
  
  //: train this edge 
  sample1 = c3 - c1; // center difference measured from the image
  //: calculate angle and dists 
  e_3_0_to_second->calculate_dist_angle(pi_0_0->cast_to_instance(), sample1, d1, a1);
  e_3_0_to_second->set_min_stand_dev_dist(15.0f); // 
  e_3_0_to_second->set_min_stand_dev_angle(5.0f); // 
  e_3_0_to_second->update_dist_model(d1);
  e_3_0_to_second->update_angle_model(a1);
  h->add_edge_no_check(e_3_0_to_second);

  

  return h;

}

// building 1
bvxm_part_hierarchy_sptr
bvxm_part_hierarchy_builder::construct_vehicle_detector_roi1_3()
{
  bvxm_part_hierarchy_sptr h = new bvxm_part_hierarchy();

  //: LAYER 0: two primitives: 
  bvxm_part_base_sptr p_0_0 = new bvxm_part_base(0, 0);  // (lambda0=5.0,lambda1=2.5,theta=-40,bright=true)
  h->add_vertex(p_0_0);
  bvxm_part_base_sptr p_0_1 = new bvxm_part_base(0, 1);  // (lambda0=2.0,lambda1=1.0,theta=45,bright=false)
  h->add_vertex(p_0_1);
  bvxm_part_base_sptr p_0_2 = new bvxm_part_base(0, 2);  // (lambda0=14.0,lambda1=4.0,theta=-40,bright=true)
  h->add_vertex(p_0_2);
  //: create a dummy instance from each and add to h
  bvxm_part_gaussian_sptr pi_0_0 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 5.0f, 2.5f, -40.0f, true, 0);
  bvxm_part_gaussian_sptr pi_0_1 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 45.0f, false, 1);
  bvxm_part_gaussian_sptr pi_0_2 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 14.0f, 4.0f, -40.0f, true, 2);
  pi_0_1->cutoff_percentage_ = 0.5f;
  pi_0_0->detection_threshold_ = 0.001f;
  pi_0_1->detection_threshold_ = 0.001f;
  pi_0_2->detection_threshold_ = 0.001f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_2->cast_to_instance());

  //: LAYER 1: only one layer 1 part
  bvxm_part_base_sptr p_1_0 = new bvxm_part_base(1, 0);  
  h->add_vertex(p_1_0);
  p_1_0->detection_threshold_ = 0.001f;
  //: the first child becomes the central part, create an edge to the central part
  bvxm_hierarchy_edge_sptr e_1_0_to_central = new bvxm_hierarchy_edge(p_1_0, p_0_2);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_2->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  //: create an edge to the second part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_second = new bvxm_hierarchy_edge(p_1_0, p_0_0);     
  p_1_0->add_outgoing_edge(e_1_0_to_second);        
  p_0_0->add_incoming_edge(e_1_0_to_second);
  
  vnl_vector_fixed<float,2> c_0_0_1(331.0f,787.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_0_2(312.0f,804.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_0_3(297.0f,816.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_1_1(284.0f,835.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_2_1(307.0f,811.0f); // center measured from the image

  //: train this edge 
  vnl_vector_fixed<float,2> sample1 = c_0_0_1 - c_0_2_1; // center difference measured from the image
  //: calculate angle and dists 
  float a1, d1;
  e_1_0_to_second->calculate_dist_angle(pi_0_2->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->set_min_stand_dev_dist(2.0f);
  e_1_0_to_second->set_min_stand_dev_angle(2.0f); 
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_angle_model(a1);
  h->add_edge_no_check(e_1_0_to_second);

  //: create an edge to the third part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_third = new bvxm_hierarchy_edge(p_1_0, p_0_1);     
  p_1_0->add_outgoing_edge(e_1_0_to_third);        
  p_0_1->add_incoming_edge(e_1_0_to_third);
  
  //: train this edge 
  sample1 = c_0_1_1 - c_0_2_1; // center difference measured from the image
  //: calculate angle and dists for these two samples
  e_1_0_to_third->calculate_dist_angle(pi_0_2->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_third->set_min_stand_dev_dist(1.0f); 
  e_1_0_to_third->set_min_stand_dev_angle(2.0f); 
  e_1_0_to_third->update_dist_model(d1);
  e_1_0_to_third->update_angle_model(a1);
  h->add_edge_no_check(e_1_0_to_third);

  return h;
}


// building 2
bvxm_part_hierarchy_sptr 
bvxm_part_hierarchy_builder::construct_vehicle_detector_roi1_4()
{
  bvxm_part_hierarchy_sptr h = new bvxm_part_hierarchy();

  //: LAYER 0: two primitives: 
  bvxm_part_base_sptr p_0_0 = new bvxm_part_base(0, 0);  // (lambda0=4.0,lambda1=2.0,theta=-40,bright=true)
  h->add_vertex(p_0_0);
  bvxm_part_base_sptr p_0_1 = new bvxm_part_base(0, 1);  // (lambda0=10.0,lambda1=2.0,theta=-35,bright=true)
  h->add_vertex(p_0_1);
  bvxm_part_base_sptr p_0_2 = new bvxm_part_base(0, 2);  // (lambda0=2.0,lambda1=1.0,theta=45,bright=false)
  h->add_vertex(p_0_2);
  
  //: create a dummy instance from each and add to h
  bvxm_part_gaussian_sptr pi_0_0 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 4.0f, 2.0f, -40.0f, true, 0);
  bvxm_part_gaussian_sptr pi_0_1 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 10.0f, 2.0f, -35.0f, true, 1);
  bvxm_part_gaussian_sptr pi_0_2 = new bvxm_part_gaussian(0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 45.0f, false, 2);
  pi_0_0->cutoff_percentage_ = 0.5f;
  pi_0_2->cutoff_percentage_ = 0.5f;
  pi_0_0->detection_threshold_ = 0.01f;
  pi_0_1->detection_threshold_ = 0.01f;
  pi_0_2->detection_threshold_ = 0.00001f;
  h->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_1->cast_to_instance());
  h->add_dummy_primitive_instance(pi_0_2->cast_to_instance());

  //: LAYER 1: only one layer 1 part
  bvxm_part_base_sptr p_1_0 = new bvxm_part_base(1, 0);  
  h->add_vertex(p_1_0);
  p_1_0->detection_threshold_ = 0.0000001f;
  //: the first child becomes the central part, create an edge to the central part
  bvxm_hierarchy_edge_sptr e_1_0_to_central = new bvxm_hierarchy_edge(p_1_0, p_0_1);
  p_1_0->add_outgoing_edge(e_1_0_to_central); 
  p_0_1->add_incoming_edge(e_1_0_to_central);
  h->add_edge_no_check(e_1_0_to_central);

  //: create an edge to the second part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_second = new bvxm_hierarchy_edge(p_1_0, p_0_0);     
  p_1_0->add_outgoing_edge(e_1_0_to_second);        
  p_0_0->add_incoming_edge(e_1_0_to_second);
  
  vnl_vector_fixed<float,2> c_0_0_1(34.0f,516.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_0_2(17.0f,529.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_2_1(9.0f,535.0f); // center measured from the image
  vnl_vector_fixed<float,2> c_0_1_1(34.0f,517.0f); // center measured from the image

  //: train this edge 
  vnl_vector_fixed<float,2> sample1 = c_0_0_2 - c_0_1_1; // center difference measured from the image
  //: calculate angle and dists 
  float a1, d1;
  e_1_0_to_second->calculate_dist_angle(pi_0_1->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_second->set_min_stand_dev_dist(1.0f);
  e_1_0_to_second->set_min_stand_dev_angle(2.0f); 
  e_1_0_to_second->update_dist_model(d1);
  e_1_0_to_second->update_angle_model(a1);
  h->add_edge_no_check(e_1_0_to_second);

  //: create an edge to the third part of p_1_0
  bvxm_hierarchy_edge_sptr e_1_0_to_t = new bvxm_hierarchy_edge(p_1_0, p_0_2);     
  p_1_0->add_outgoing_edge(e_1_0_to_t);        
  p_0_2->add_incoming_edge(e_1_0_to_t);

  //: train this edge 
  sample1 = c_0_2_1 - c_0_1_1; // center difference measured from the image
  //: calculate angle and dists 
  e_1_0_to_t->calculate_dist_angle(pi_0_1->cast_to_instance(), sample1, d1, a1);
  e_1_0_to_t->set_min_stand_dev_dist(1.0f);
  e_1_0_to_t->set_min_stand_dev_angle(2.0f); 
  e_1_0_to_t->update_dist_model(d1);
  e_1_0_to_t->update_angle_model(a1);
  h->add_edge_no_check(e_1_0_to_t);

  return h;
}
  





