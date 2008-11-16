#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vul/vul_file.h>

#include <rec/bvxm_part_base.h>
#include <rec/bvxm_part_base_sptr.h>
#include <rec/bvxm_part_hierarchy.h>
#include <rec/bvxm_part_hierarchy_sptr.h>
#include <rec/bvxm_hierarchy_edge.h>
#include <rec/bvxm_hierarchy_edge_sptr.h>
#include <rec/bvxm_part_hierarchy_builder.h>

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

#include <brip/brip_vil_float_ops.h>

#include <rec/bvxm_part_gaussian.h>
#include <bsta/algo/bsta_gaussian_updater.h>

static void test_bvxm_hierarchy_builder()
{
  vcl_string file = "normalized0_cropped.tif";
  vil_image_resource_sptr img = vil_load_image_resource(file.c_str());
  TEST("test load img", !img, false);

  if (!img)
    return;

  unsigned ni = img->ni(); unsigned nj = img->nj();
  if (ni != 523 || nj != 460)
    return;

  vcl_cout << "image ni: " << ni << " nj: " << nj << vcl_endl;

  bvxm_part_hierarchy_sptr h = bvxm_part_hierarchy_builder::construct_vehicle_detector_roi1_0();
  TEST("test hierarchy", !h, false);
  vcl_cout << "constructed: " << h->number_of_vertices() << " vertices in the vehicle detector for roi1\n";
  vcl_cout << "constructed: " << h->number_of_edges() << " edges in the vehicle detector for roi1\n";

  vcl_vector<bvxm_part_instance_sptr> dumm_ins = h->get_dummy_primitive_instances();
  
  vcl_vector<bvxm_part_instance_sptr> parts_prims;
  for (unsigned i = 0; i < dumm_ins.size(); i++) {
    if (dumm_ins[i]->kind_ == bvxm_part_instance_kind::GAUSSIAN) {
      bvxm_part_gaussian_sptr p = dumm_ins[i]->cast_to_gaussian();
      if (!extract_gaussian_primitives(img, p->lambda0_, p->lambda1_, p->theta_, p->bright_, p->cutoff_percentage_, 0.1f, p->type_, parts_prims))
        vcl_cout << "problems in extracting gaussian primitives!!\n";
    }
  }

  vcl_cout << "\t extracted " << parts_prims.size() << " primitives\n";

  unsigned highest = h->highest_layer_id();
  vcl_vector<bvxm_part_instance_sptr> parts_upper_most(parts_prims);
  for (unsigned l = 1; l <= highest; l++) {
    vcl_vector<bvxm_part_instance_sptr> parts_current;
    h->extract_upper_layer(parts_upper_most, ni, nj, 0.1f, parts_current);
    vcl_cout << "extracted " << parts_current.size() << " parts of layer " << l << "\n";
    parts_upper_most.clear();
    parts_upper_most = parts_current;
  }

  vcl_cout << "\t extracted " << parts_upper_most.size() << " of highest layer: " << highest << " parts\n";

  vil_image_view<float> output_map_float(ni, nj);
  bvxm_part_hierarchy::generate_output_map(parts_upper_most, output_map_float);
  float min, max;
  vil_math_value_range(output_map_float, min, max);
  vcl_cout << "\t output map float value range, min: " << min << " max: " << max << vcl_endl;

  vil_image_view<vxl_byte> output_map_byte(ni, nj);
  vil_convert_stretch_range_limited(output_map_float, output_map_byte, 0.0f, 1.0f);
  vil_save(output_map_byte, "./map_output_receptive_field_highest.png");

  vil_image_view<vxl_byte> output_img(ni, nj, 3);
  vil_image_view<vxl_byte> input_img = img->get_view(0, ni, 0, nj);
  bvxm_part_hierarchy::generate_output_img(parts_upper_most, input_img, output_img);
  vil_save(output_img, "./img_output_receptive_field_highest.png");

}

TESTMAIN( test_bvxm_hierarchy_builder );





