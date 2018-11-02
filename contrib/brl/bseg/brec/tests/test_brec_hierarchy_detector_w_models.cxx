#include <iostream>
#include <fstream>
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
#include <brec/brec_part_hierarchy_detector.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view_base.h>

#include <vul/vul_file.h>

#include <brip/brip_vil_float_ops.h>
#include <brec/brec_part_gaussian.h>
#include <bsta/algo/bsta_gaussian_updater.h>
#include <bsta/bsta_histogram.h>

static void test_brec_hierarchy_detector_w_models()
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

  std::string mean_file = "out_mog_0_mean_img_cropped.tiff";
  vil_image_resource_sptr mean_img = vil_load_image_resource(mean_file.c_str());
  TEST("test load img", !mean_img, false);
  if (!mean_img)
    return;
  if (mean_img->ni() != ni || mean_img->nj() != nj) {
    std::cout << "mean img size not compatible!\n";
    return;
  }

  std::string sd_file = "out_mog_0_std_dev_img_cropped.tiff";
  vil_image_resource_sptr sd_img = vil_load_image_resource(sd_file.c_str());
  TEST("test load img", !sd_img, false);
  if (!sd_img)
    return;
  if (sd_img->ni() != ni || sd_img->nj() != nj) {
    std::cout << "std dev img size not compatible!\n";
    return;
  }

  vsl_b_ifstream is(mask_file);
  vil_image_view_base_sptr mask_image;
  vsl_b_read(is, mask_image);
  is.close();
  vil_image_view<bool> mask_img(mask_image);

#if 0
  // abuse the gt image as a background probability image
  vil_image_view<float> prob_map = vil_convert_cast(float(), gt_img->get_view());
  vil_math_scale_values(prob_map,1.0/255.0);
  vil_image_view<float> dummy(ni, nj), back_prob_map(ni, nj);
  dummy.fill(1.0f);
  vil_math_image_difference(dummy, prob_map, back_prob_map);
#else
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
  vil_image_view<float> dummy(ni, nj), prob_map(ni, nj);
  dummy.fill(1.0f);
  vil_math_image_difference(dummy, back_prob_map, prob_map);
#endif

  vil_image_view<float> inp_img = vil_convert_cast(float(), img->get_view());
  vil_math_scale_values(inp_img,1.0/255.0);
  vil_save(inp_img, "./inp_img_scaled.tiff");

  vil_image_view<float> inp_mean_img(mean_img->get_view());
  vil_image_view<float> inp_sd_img(sd_img->get_view());

  // collect stats from the images, find the intensity histogram
  bsta_histogram<float> hi(0.0f, 1.0f, 255);
  for (unsigned i = 0; i < inp_img.ni(); i++) {
    for (unsigned j = 0; j < inp_img.nj(); j++) {
      if (mask_img(i,j))
        hi.upcount(inp_img(i,j), 1.0f);
    }
  }
  bsta_histogram<float> hi2(0.0f, 1.0f, 255);
  for (unsigned i = 0; i < inp_mean_img.ni(); i++) {
    for (unsigned j = 0; j < inp_mean_img.nj(); j++) {
      if (mask_img(i,j))
        hi2.upcount(inp_mean_img(i,j), 1.0f);
    }
  }
  std::ofstream ofs("hist.out");
  ofs << "image intensities:\n"
      << hi << std::endl
      << "mean image intensities:\n"
      << hi2 << std::endl;
  ofs.close();

  brec_part_hierarchy_sptr h = brec_part_hierarchy_builder::construct_test_detector(); // brec_part_hierarchy_builder::construct_detector_roi1_0();

  std::cout << "h: # of nodes: " << h->number_of_vertices() << " # of edges: " << h->number_of_edges() << " # of prims: " << h->get_dummy_primitive_instances().size() << std::endl;

  // train the response models
  std::string model_dir("./train_dir/");
  vul_file::make_directory(model_dir);
  h->set_model_dir(model_dir);

  vil_image_view<float> lambda_img(ni, nj);
  vil_image_view<float> k_img(ni, nj);

  // learn a background & foreground response model for each primitive
  std::vector<brec_part_instance_sptr> ins = h->get_dummy_primitive_instances();
  for (unsigned i = 0; i < ins.size(); i++) {
    if (ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = ins[i]->cast_to_gaussian();
#if 0  // using weibull model for background responses
      if (!p->construct_bg_response_model(inp_mean_img, inp_sd_img, lambda_img, k_img)) {
        std::cout << "problems in constructing background model for gaussian primitives!!\n";
        return;
      }
      // write the model parameter images
      std::string name = model_dir+p->string_identifier()+"_bg_lambda_img.tiff";
      bool result = vil_save(lambda_img,name.c_str());
      if ( !result ) {
        std::cerr << "Failed to save image to" << name << std::endl;
        return;
      }

      name = model_dir+p->string_identifier()+"_bg_k_img.tiff";
      result = vil_save(k_img,name.c_str());
      if ( !result ) {
        std::cerr << "Failed to save image to" << name << std::endl;
        return;
      }
#else  // using gaussian model at every pixel
      if (!p->construct_bg_response_model_gauss(inp_mean_img, inp_sd_img, lambda_img, k_img)) {
        std::cout << "problems in constructing background model for gaussian primitives!!\n";
        return;
      }
      // write the model parameter images
      std::string name = model_dir+p->string_identifier()+"_bg_mu_img.tiff";
      bool result = vil_save(lambda_img,name.c_str());
      if ( !result ) {
        std::cerr << "Failed to save image to" << name << std::endl;
        return;
      }

      name = model_dir+p->string_identifier()+"_bg_sigma_img.tiff";
      result = vil_save(k_img,name.c_str());
      if ( !result ) {
        std::cerr << "Failed to save image to" << name << std::endl;
        return;
      }
#endif
#if 0
      double lambda, k;
      if (!p->construct_fg_response_model(inp_img, prob_map, mask_img, false, lambda, k)) {
        std::cout << "problems in constructing foreground response model parameters for gaussian primitives!!\n";
        return;
      }
      // write the model parameters into a file in the output directory
      name = model_dir+p->string_identifier()+"_fg_params.txt";
      std::ofstream of(name.c_str());
      of << k << ' ' << lambda << std::endl;
      of.close();
#endif
    }
  }

  brec_part_hierarchy_detector hd(h);
  hd.detect(inp_img, back_prob_map, 0.0f, brec_detector_methods::POSTERIOR, 3.0);

  std::vector<brec_part_instance_sptr> parts_prims = hd.get_parts(0);

  vil_image_view<float> output_map_float(ni, nj);
  vil_image_view<vxl_byte> output_map_byte(ni, nj);
  vil_image_view<vxl_byte> output_img(ni, nj, 3);
  vil_image_view<vxl_byte> input_img = img->get_view(0, ni, 0, nj);

  float min, max;
  brec_part_hierarchy::generate_output_map3(parts_prims, output_map_float);
  vil_math_value_range(output_map_float, min, max);
  std::cout << "\toutput map for layer 0, float value range, min: " << min << " max: " << max << std::endl;
  vil_save(output_map_float, "./map_output_receptive_field_layer_0.tiff");
  vil_convert_stretch_range_limited(output_map_float, output_map_byte, 0.0f, 1.0f);
  vil_save(output_map_byte, "./map_output_receptive_field_layer_0.png");
  brec_part_hierarchy::generate_output_img(parts_prims, input_img, output_img, brec_posterior_types::CLASS_FOREGROUND);
  vil_save(output_img, "./img_output_receptive_field_layer_0.png");

  unsigned highest = h->highest_layer_id();
  std::vector<brec_part_instance_sptr> parts_upper_most = hd.get_parts(highest);

  brec_part_hierarchy::generate_output_map3(parts_upper_most, output_map_float);
  vil_math_value_range(output_map_float, min, max);
  std::cout << "\toutput map highest layer, float value range, min: " << min << " max: " << max << std::endl;
  vil_save(output_map_float, "./map_output_receptive_field_highest_detector.tiff");
  vil_convert_stretch_range_limited(output_map_float, output_map_byte, 0.0f, 1.0f);
  vil_save(output_map_byte, "./map_output_receptive_field_highest_detector.png");

  brec_part_hierarchy::generate_output_img(parts_upper_most, input_img, output_img, brec_posterior_types::CLASS_FOREGROUND);
  vil_save(output_img, "./img_output_receptive_field_highest_detector.png");
}

TESTMAIN( test_brec_hierarchy_detector_w_models );
