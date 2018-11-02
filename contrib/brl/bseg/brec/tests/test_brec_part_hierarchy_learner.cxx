#include <iostream>
#include <fstream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view_base.h>

#include <brec/brec_part_hierarchy_learner.h>
#include <brec/brec_part_hierarchy_learner_sptr.h>

#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>

static void test_brec_part_hierarchy_learner()
{
  std::string file = "test_view_0_cropped.png"; // "normalized0_cropped.png";
  std::string gt_file = "normalized0_gt_cropped.png";
  // load the mask img as well for foreground model construction
  std::string mask_file = "test_view_0_mask_cropped.bin";

  vil_image_resource_sptr img = vil_load_image_resource(file.c_str());
  TEST("test load img", !img, false);
  if (!img)
    return;

  vil_image_resource_sptr gt_img = vil_load_image_resource(gt_file.c_str());
  TEST("test load img", !gt_img, false);
  if (!gt_img)
    return;

  unsigned ni = img->ni(); unsigned nj = img->nj();
  std::cout << "image ni: " << ni << " nj: " << nj << std::endl;

  vsl_b_ifstream is(mask_file);
  vil_image_view_base_sptr mask_image;
  vsl_b_read(is, mask_image);
  is.close();
  vil_image_view<bool> mask_img(mask_image);

  // load the gt image
  vil_image_view<float> gt_map = vil_convert_cast(float(), gt_img->get_view());
  vil_math_scale_values(gt_map,1.0f/255.0f);
  vil_image_view<float> dummy(ni, nj), back_gt_map(ni, nj);
  dummy.fill(1.0f);
  vil_math_image_difference(dummy, gt_map, back_gt_map);

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
  vil_image_view<float> prob_map(ni, nj);
  vil_math_image_difference(dummy, back_prob_map, prob_map);

  vil_image_view<float> inp_img = vil_convert_cast(float(), img->get_view());
  vil_math_scale_values(inp_img,1.0f/255.0f);

  brec_part_hierarchy_learner_sptr collector = new brec_part_hierarchy_learner();
  // 4 directions, lambda's start from 1.0 and increase to 2 with increments of 1
  collector->initialize_layer0_as_gaussians(4, 2.0f, 1.0f, 2);
  TEST("collector created", !collector, false);
  TEST("collector created", collector->stats_layer0().size(), 4*4*2);

  vul_timer t;
  t.mark();
  // no need to convert prob map when passing gt_map cause vehicles have "high probs" (in background map, vehicles have low probs)
  //collector->collect_layer0_stats(inp_img, gt_map, mask_img);
  std::cout << " update of one image took: " << t.real()/1000 << " seconds " << t.real()/ (60*1000) << " mins.\n";
  //collector->print();
  std::string m_file = "out_figure.m";
  collector->print_to_m_file_layer0(m_file);
  collector = 0;

  // test whether the stats from histogram are computed exactly the same
  vnl_random rng;
  float probs[100];
  for (unsigned i = 0; i < 100; i++) {
    probs[i] = 1.0f; // was: float(rng.drand32());
  }
  float numbers[100];
  float std_dev = 0.1f; float mean = 0.5f;
  for (unsigned i = 0; i < 100; i++) {
    numbers[i] = std_dev*float(rng.normal()) + mean;
  }

  bsta_histogram<float> h(3.0f, 1000);
  for (unsigned i = 0; i < 100; i++) {
    h.upcount(numbers[i], probs[i]);
  }

  std::cout << "mean from histogram: " << h.mean() << " variance: " << h.variance() << std::endl;

  // calculate yourself
  float x_sum = 0.0f, xsq_sum = 0.0f, p_sum = 0.0f;
  for (unsigned i = 0; i < 100; i++) {
    x_sum += probs[i]*numbers[i];
    xsq_sum += probs[i]*numbers[i]*numbers[i];
    p_sum += probs[i];
  }
  mean = x_sum/p_sum; // estimate of mean
  float total_var = xsq_sum/p_sum; //estimate of total variance
  float var = total_var - mean*mean;
  std::cout << "mean calculated: " << mean << " variance: " << var << std::endl;
}

TESTMAIN( test_brec_part_hierarchy_learner );
