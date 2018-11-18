#include <iostream>
#include <sstream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brec/brec_part_base.h>
#include <brec/brec_part_base_sptr.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_hierarchy_edge.h>
#include <brec/brec_hierarchy_edge_sptr.h>
#include <brec/brec_part_hierarchy_builder.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

#include <brip/brip_vil_float_ops.h>

#include <brec/brec_part_gaussian.h>
#include <bsta/algo/bsta_gaussian_updater.h>

static void test_brec_parts()
{
  std::string file = "digits_small.png";
  vil_image_resource_sptr img = vil_load_image_resource(file.c_str());

  unsigned ni = img->ni();
  unsigned nj = img->nj();
  vil_image_view<vxl_byte> inp_img = img->get_view(0,ni,1,nj);

  float lambda0 = 2.0f;
  float lambda1 = 1.0f;
  float theta = 0.0f;
  bool bright = false;

  std::vector<brec_part_instance_sptr> parts_0_0;
  TEST("testing brec parts ", extract_gaussian_primitives(img, lambda0, lambda1, theta, bright, 0.01f, 0.1f, 0, parts_0_0), true);
  TEST_NEAR("testing brec parts ", parts_0_0.size(), 25, 0.0001);

  vil_image_view<vxl_byte> out_image(ni, nj, 3);
  out_image.fill(0);
  for (unsigned i = 0; i < parts_0_0.size(); i++) {
    parts_0_0[i]->cast_to_gaussian()->mark_center(out_image, 0);
    parts_0_0[i]->cast_to_gaussian()->mark_receptive_field(out_image, 2);
  }
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      out_image(i,j, 1) = inp_img(i,j);
    }
  vil_save(out_image, "./extracted_horizontal_parts.png");

  theta = 90.0f;
  std::vector<brec_part_instance_sptr> parts_0_1;
  TEST("testing brec parts ", extract_gaussian_primitives(img, lambda0, lambda1, theta, bright, 0.01f, 0.1f, 1, parts_0_1), true);
  TEST_NEAR("testing brec parts ", parts_0_1.size(), 36, 0.0001);
  vil_image_view<vxl_byte> out_image2(ni, nj, 3);
  out_image2.fill(0);
  for (unsigned i = 0; i < parts_0_1.size(); i++) {
    parts_0_1[i]->cast_to_gaussian()->mark_center(out_image2, 0);
    parts_0_1[i]->cast_to_gaussian()->mark_receptive_field(out_image2, 2);
  }
  vil_save(out_image2, "./extracted_ninety_degree_parts_0_1.png");

  std::vector<brec_part_instance_sptr> parts;
  theta = 45.0f;
  TEST("testing brec parts ", extract_gaussian_primitives(img, lambda0, lambda1, theta, bright, 0.01f, 0.1f, 2, parts), true);

  theta = 135.0f;
  TEST("testing brec parts ", extract_gaussian_primitives(img, lambda0, lambda1, theta, bright, 0.01f, 0.1f, 3, parts), true);

  // construct a hierarchy by hand starting from the primitives, construct one to recognize 8
  brec_part_hierarchy_sptr h_8 = new brec_part_hierarchy();

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
  brec_hierarchy_edge_sptr e_1_0_to_central = new brec_hierarchy_edge(p_1_0, p_0_1);
  p_1_0->add_outgoing_edge(e_1_0_to_central);
  p_0_1->add_incoming_edge(e_1_0_to_central);
  h_8->add_edge_no_check(e_1_0_to_central);

  // create an edge to the second part of p_1_0
  brec_hierarchy_edge_sptr e_1_0_to_second = new brec_hierarchy_edge(p_1_0, p_0_1);     //  p_1_0
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
  brec_hierarchy_edge_sptr e_2_0_to_central = new brec_hierarchy_edge(p_2_0, p_1_0);
  p_2_0->add_outgoing_edge(e_2_0_to_central);
  p_1_0->add_incoming_edge(e_2_0_to_central);
  h_8->add_edge_no_check(e_2_0_to_central);

  // create an edge to the second part
  brec_hierarchy_edge_sptr e_2_0_to_second = new brec_hierarchy_edge(p_2_0, p_1_0);
  p_2_0->add_outgoing_edge(e_2_0_to_second);
  p_1_0->add_incoming_edge(e_2_0_to_second);
  h_8->add_edge(e_2_0_to_second);
  vnl_vector_fixed<float,2> sample3(-1.0f,8.0f); // measured from the 8 sample
  e_2_0_to_second->calculate_dist_angle(pi_0_1->cast_to_instance(), sample3, d1, a1); // using pi_0_1 as it is still the center of p_1_0
  std::cout << "p_2_0 edge to second part training d: " << d1 << " angle: " << a1*vnl_math::deg_per_rad << " degrees\n";
  e_2_0_to_second->update_dist_model(d1);
  e_2_0_to_second->update_angle_model(a1);

  h_8->add_edge_no_check(e_2_0_to_second);

  TEST("number of vertices in h8", h_8->number_of_vertices(), 3);
  TEST("number of edges in h8", h_8->number_of_edges(), 4);
  TEST("check for existence", h_8->get_node(0, 1), p_0_1);
  TEST("check for existence", h_8->get_node(1, 0), p_1_0);
  TEST("check for existence", h_8->get_node(2, 0), p_2_0);
  TEST("check for existence", h_8->get_node(3, 0), 0);

  // check helper methods
  //p_1_0->compute_activation_radius();
  double mean_angle = e_1_0_to_second->mean_angle();
  TEST("mean_angle", mean_angle, mean_angle);
  double mean_dist = e_1_0_to_second->mean_dist();
  TEST("mean_dist", mean_dist, mean_dist);
  double var_angle = e_1_0_to_second->var_angle();
  TEST("var_angle", var_angle, var_angle);
  double var_dist = e_1_0_to_second->var_dist();
  TEST("var_dist", var_dist, var_dist);

  // create a prob map of the instantiation of the hierarchy in a given image
  vil_image_view<float> map(ni, nj, 1);  // the second plane will hold the types of the primitives
  vil_image_view<unsigned> type_map(ni, nj, 1);  // the second plane will hold the types of the primitives
  brec_part_hierarchy::generate_map(parts_0_1, map, type_map);

  float min, max;
  vil_math_value_range(map, min, max);
  std::cout << " map 0 1 value range, min: " << min << " max: " << max << std::endl;
  vil_image_view<vxl_byte> map_b(ni, nj);
  vil_convert_stretch_range_limited(map, map_b, 0.0f, max);
  vil_save(map_b, "./map_generated_0_1.png");

  std::vector<brec_part_instance_sptr> dummy(nj, 0);
  std::vector<std::vector<brec_part_instance_sptr> > part_map(ni, dummy);
  brec_part_hierarchy::generate_map(parts_0_1, part_map);

  brec_part_instance_sptr ins = h_8->exists(p_1_0, parts_0_1[0], map, type_map, part_map, 0.1f); // p will be its central part and map will tell if all the other parts exist
  TEST("testing exists", !ins, false);
  std::cout << "strength is: " << ins->strength_ << std::endl;
#if 0
  // find the central part
  unsigned ind = 21;
  for (unsigned i = 0; i < parts_0_1.size(); i++) {
    if (parts_0_1[i]->x_ == 62 && parts_0_1[i]->y_ == 47) {
      ind = i;
      break;
    }
  }

  std::vector<brec_part_instance_sptr> parts_1_0;
  ins = h_8->exists(p_1_0, parts_0_1[ind], map, type_map, 0.1f); // p will be its central part and map will tell if all the other parts exist
  TEST("testing exists", !ins, false);
  parts_1_0.push_back(ins);
  std::cout << "strength is: " << ins->strength_ << std::endl;

  brec_part_hierarchy::generate_map(parts_1_0, map, type_map);
  vil_math_value_range(map, min, max);
  std::cout << " map 1 0 value range, min: " << min << " max: " << max << std::endl;
  vil_convert_stretch_range_limited(map, map_b, 0.0f, max);
  vil_save(map_b, "./map_generated_1_0.png");

  ins = h_8->exists(p_1_0, parts_0_1[1], map, type_map, 0.1f); // p will be its central part and map will tell if all the other parts exist
  if (ins != 0)
    parts_1_0.push_back(ins);
  TEST(***);
  parts_1_0.clear();
#endif // 0

  std::vector<brec_part_instance_sptr> parts_1_0;
  h_8->extract_upper_layer(parts_0_1, ni, nj, parts_1_0);
  //TEST_NEAR("extracting layer 1", parts_1_0.size(), 54, 0.1);

  brec_part_hierarchy::generate_map(parts_1_0, map, type_map);
  vil_math_value_range(map, min, max);
  std::cout << " map 1 0 value range, min: " << min << " max: " << max << std::endl;
  vil_convert_stretch_range_limited(map, map_b, 0.0f, max);
  vil_save(map_b, "./map_generated_1_0.png");

  vil_image_view<vxl_byte> output_map(ni, nj, 1);
  output_map.fill(0);
  for (unsigned i = 0; i < parts_1_0.size(); i++) {
    if (parts_1_0[i]->strength_ == max)
      parts_1_0[i]->mark_receptive_field(output_map, 0);
  }
  vil_save(output_map, "./map_receptive_field_1_0.png");

  std::vector<brec_part_instance_sptr> parts_2_0;
  h_8->extract_upper_layer(parts_1_0, ni, nj, parts_2_0);
  //TEST_NEAR("extracting layer 1", parts_2_0.size(), 40, 0.1);

  brec_part_hierarchy::generate_map(parts_2_0, map, type_map);
  vil_math_value_range(map, min, max);
  std::cout << " map 2 0 value range, min: " << min << " max: " << max << std::endl;
  vil_convert_stretch_range_limited(map, map_b, 0.0f, max);
  vil_save(map_b, "./map_generated_2_0.png");

  //vil_image_view<float> output_map(ni, nj);
  for (unsigned i = 0; i < parts_2_0.size(); i++) {
    if (parts_2_0[i]->strength_ == max)
      parts_2_0[i]->mark_receptive_field(output_map, 0);
  }
  vil_save(output_map, "./map_receptive_field_2_0.png");

  vil_image_view<float> output_map_float(ni, nj);
  brec_part_hierarchy::generate_output_map(parts_2_0, output_map_float);
  vil_math_value_range(output_map_float, min, max);
  std::cout << " output map float value range, min: " << min << " max: " << max << std::endl;

  vil_image_view<vxl_byte> output_map_byte(ni, nj);
  vil_convert_stretch_range_limited(output_map_float, output_map_byte, 0.0f, 1.0f);
  vil_save(output_map_byte, "./map_output_receptive_field_2_0.png");

  for (unsigned i = 0; i < parts_2_0.size(); i++) {
    output_map_float.fill(0.0f);
    parts_2_0[i]->mark_receptive_field(output_map_float, parts_2_0[i]->strength_/max);
    vil_convert_stretch_range_limited(output_map_float, output_map_byte, 0.0f, 1.0f);
    std::stringstream ss; ss << i;
    std::string name = "./map_output_receptive_field_2_0_"+ss.str()+".png";
    vil_save(output_map_byte, name.c_str());
  }
}

TESTMAIN( test_brec_parts );
