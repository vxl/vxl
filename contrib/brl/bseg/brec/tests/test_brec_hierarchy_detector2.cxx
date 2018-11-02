#include <iostream>
#include <fstream>
#include <vector>
#include <testlib/testlib_test.h>

#include <brec/brec_part_base.h>
#include <brec/brec_part_base_sptr.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_hierarchy_edge.h>
#include <brec/brec_hierarchy_edge_sptr.h>
#include <brec/brec_part_hierarchy_builder.h>
#include <brec/brec_part_hierarchy_detector.h>
#include <brec/brec_part_hierarchy_detector_sptr.h>
#include <brec/brec_part_gaussian.h>

#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view_base.h>

#include <brip/brip_vil_float_ops.h>
#include <bsta/algo/bsta_gaussian_updater.h>
#include <bsta/bsta_histogram.h>

#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_brec_hierarchy_detector2_simple()
{
  // create two hierarchies for two classes, detect instance of one, use same two primitives for both
  brec_part_hierarchy_sptr h1 = new brec_part_hierarchy();
  brec_part_hierarchy_sptr h2 = new brec_part_hierarchy();

  brec_part_base_sptr p_0_0 = new brec_part_base(0, 0);
  brec_part_gaussian_sptr pi_0_0 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, false, 0);
  brec_part_base_sptr p_0_1 = new brec_part_base(0, 1);
  brec_part_gaussian_sptr pi_0_1 = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 3.0f, 1.0f, 45.0f, false, 1);
  h1->add_vertex(p_0_0); h1->add_vertex(p_0_1); h2->add_vertex(p_0_0); h2->add_vertex(p_0_1);
  h1->add_dummy_primitive_instance(pi_0_0->cast_to_instance()); h2->add_dummy_primitive_instance(pi_0_0->cast_to_instance());
  h1->add_dummy_primitive_instance(pi_0_1->cast_to_instance()); h2->add_dummy_primitive_instance(pi_0_1->cast_to_instance());

  brec_part_base_sptr p_1_1 = new brec_part_base(1, 1); p_1_1->prior_prob_ = 10.0;
  brec_part_base_sptr p_1_2 = new brec_part_base(1, 2); p_1_2->prior_prob_ = 10.0;
  brec_part_base_sptr p_1_3 = new brec_part_base(1, 3); p_1_3->prior_prob_ = 10.0;

  float n_b_0 = 50.0f;
  float n_f_0 = 50.0f;
  float radius = 10.0f;

  h1->add_vertex(p_1_1); h1->add_vertex(p_1_2); h2->add_vertex(p_1_3);

  brec_hierarchy_edge_sptr e;  bsta_gaussian_sphere<double, 1> dm(5.0, 0.5);

  e = new brec_hierarchy_edge(p_1_1, p_0_0, true);
  h1->add_edge_no_check(e); p_1_1->add_outgoing_edge(e); p_0_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_1_1, p_0_1, false);
  h1->add_edge_no_check(e); p_1_1->add_outgoing_edge(e); p_0_1->add_incoming_edge(e);
  bsta_gaussian_sphere<double, 1> am(0.0, 0.5);
  e->set_model(dm, am, 1.0);

  e = new brec_hierarchy_edge(p_1_2, p_0_0, true);
  h1->add_edge_no_check(e); p_1_2->add_outgoing_edge(e); p_0_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_1_2, p_0_1, false);
  h1->add_edge_no_check(e); p_1_2->add_outgoing_edge(e); p_0_1->add_incoming_edge(e);
  am.set_mean(vnl_math::pi/2.0);
  e->set_model(dm, am, 1.0);

  e = new brec_hierarchy_edge(p_1_3, p_0_0, true);
  h2->add_edge_no_check(e); p_1_3->add_outgoing_edge(e); p_0_0->add_incoming_edge(e);
  e = new brec_hierarchy_edge(p_1_3, p_0_1, false);
  h2->add_edge_no_check(e); p_1_3->add_outgoing_edge(e); p_0_1->add_incoming_edge(e);
  am.set_mean(vnl_math::pi);
  e->set_model(dm, am, 1.0);

  h1->draw_to_ps(1, "./h1.ps", 10);
  h2->draw_to_ps(1, "./h2.ps", 10);

  brec_part_hierarchy_detector_sptr d = new brec_part_hierarchy_detector(h1);

  d->add_to_class_hierarchies(h1);
  d->add_to_class_hierarchies(h2);

  d->prior_non_c_f_ = n_f_0;
  //d->prior_non_c_b_ = n_b_0;
  d->prior_c_b_ = 1.0f - n_b_0 - n_f_0 - d->prior_c_f_;

  std::vector<brec_part_instance_sptr> prims;
  brec_part_gaussian_sptr p_img = new brec_part_gaussian(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, false, 0);
  p_img->rho_c_b_ = 1.0f;  prims.push_back(p_img->cast_to_instance());

  brec_part_gaussian_sptr p_img2 = new brec_part_gaussian(5.0f, 1.0f, 0.0f, 3.0f, 1.0f, 45.0f, false, 1);
  p_img2->rho_c_b_ = 1.0f;  prims.push_back(p_img2->cast_to_instance());

  brec_part_gaussian_sptr p_img3 = new brec_part_gaussian(1.0f, 5.0f, 0.0f, 3.0f, 1.0f, 45.0f, false, 1);
  p_img3->rho_c_b_ = 0.7f;  prims.push_back(p_img3->cast_to_instance());

  //brec_part_gaussian_sptr p_img4 = new brec_part_gaussian(5.0f, 2.0f, 0.0f, 3.0f, 1.0f, 45.0f, false, 1);
  //p_img4->rho_c_b_ = 1.0f;  prims.push_back(p_img4->cast_to_instance());


  Rtree_type *rt = new Rtree_type();
  for (unsigned i = 0; i < prims.size(); i++)
    rt->add(prims[i]);

  std::vector<brec_part_instance_sptr> upper_parts;
  d->extract_upper_layer(prims, rt, upper_parts, brec_detector_methods::POSTERIOR, radius);

  for (unsigned i = 0; i < upper_parts.size(); i++)
    std::cout << "detected a part at center: (" << upper_parts[i]->x_ << ", " << upper_parts[i]->y_ << ") type: " << upper_parts[i]->type_ << " posterior: " << upper_parts[i]->rho_c_b_ << std::endl;
}

static void test_brec_hierarchy_detector2_img()
{
  std::string file = "test_view_0_cropped.png"; // "normalized0_cropped.png";
  std::string gt_file = "normalized0_gt_cropped.png";
  std::string mask_file = "test_view_0_mask_cropped.bin";

  vil_image_resource_sptr img = vil_load_image_resource(file.c_str());
  TEST("test load img", !img, false);
  if (!img)
    return;
  vil_image_view<float> img_v = *vil_convert_cast(float(), img->get_view());
  vil_math_scale_values(img_v,1.0/255.0);

  vil_image_resource_sptr gt_img = vil_load_image_resource(gt_file.c_str());
  TEST("test load img", !gt_img, false);
  if (!gt_img)
    return;

  unsigned ni = img->ni(); unsigned nj = img->nj();
  std::cout << "image ni: " << ni << " nj: " << nj << std::endl;

  std::string prob_map_file = "test_view_0_prob_map_cropped.tiff";
  vil_image_resource_sptr prob_map_img = vil_load_image_resource(prob_map_file.c_str());
  TEST("test load img", !prob_map_img, false);
  if (!prob_map_img)
    return;
  if (prob_map_img->ni() != ni || prob_map_img->nj() != nj) {
    std::cout << "std dev img size not compatible!\n";
    return;
  }
  vil_image_view<float> back_prob_map = prob_map_img->get_view();
  vil_image_view<float> dummy(ni, nj), fg_prob_map(ni, nj);
  dummy.fill(1.0f);
  vil_math_image_difference(dummy, back_prob_map, fg_prob_map);

  std::string h_name = "C:\\projects\\roi_1\\sewage_and_vehicles_learning\\output_learning\\hierarchy_0_layer_2.xml";
  std::string model_dir = "C:\\projects\\roi_1\\sewage_and_vehicles_learning\\output_learning\\";
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  std::ifstream is(h_name.c_str());
  h->read_xml(is);
  is.close();

  h->set_model_dir(model_dir);

  brec_part_hierarchy_detector_sptr hd = new brec_part_hierarchy_detector(h);
  hd->detect(img_v, fg_prob_map, 0.0f, brec_detector_methods::POSTERIOR, 5.0);
}

static void test_brec_hierarchy_detector2()
{
  test_brec_hierarchy_detector2_simple();
  test_brec_hierarchy_detector2_img();
}

TESTMAIN( test_brec_hierarchy_detector2 );
