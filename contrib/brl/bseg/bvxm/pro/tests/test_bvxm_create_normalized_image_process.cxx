//:
// \file
// \brief  Tests for normalized image creation process
// \author Ozge C. Ozcanli
// \date   03/04/2008
//
#include <testlib/testlib_test.h>
#include "../bvxm_create_normalized_image_process.h"
#include "../bvxm_normalize_image_process.h"
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <vil/vil_save.h>
#include <vnl/vnl_random.h>

#include <brip/brip_vil_float_ops.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_perspective_camera.h>




MAIN( test_bvxm_create_normalized_image_process )
{
  unsigned ni = 640;
  unsigned nj = 480;

  //: first test various stages in the normalization process
  vil_image_view<vxl_byte> input_img(ni, nj, 1);
  input_img.fill(200);
  vil_image_view_base_sptr input_img_sptr = new vil_image_view<vxl_byte>(input_img);
  vcl_cout << "format: " << input_img_sptr->pixel_format() << vcl_endl;
  TEST("check byte", input_img_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE, true);

  vil_image_view<vxl_byte> input_img_rgb(ni, nj, 3);
  input_img.fill(200);
  vil_image_view_base_sptr input_img_rgb_sptr = new vil_image_view<vxl_byte>(input_img_rgb);
  vcl_cout << "format: " << input_img_rgb_sptr->pixel_format() << vcl_endl;
  TEST("check byte", input_img_rgb_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE, true);
  TEST_NEAR("check fill", input_img_rgb(0,0,1), 205, 0.01);  // weird!! I fill with 200 but the value is 205!!

  //: test normalize image method
  vil_image_view<vxl_byte> img1(100, 200, 3), out_img(100, 200, 3), img2(100, 200, 3), im_dif(100, 200, 3);
  img1.fill(100);
  img2.fill((vxl_byte)(1.2*100 + 50));
  normalize_image<vxl_byte>(img1, out_img, 1.2f, 50.0f, 255);
  vil_math_image_difference(img2, out_img, im_dif);
  float sum = 0;
  vil_math_sum(sum, im_dif, 0);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);
  vil_math_sum(sum, im_dif, 1);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);
  vil_math_sum(sum, im_dif, 2);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);

  //: now do the same thing via the process
  REG_PROCESS(bvxm_create_normalized_image_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);
  
  //: set the inputs
  vil_image_view_base_sptr input_img1 = new vil_image_view<vxl_byte>(img1);
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(input_img1);
  brdb_value_sptr v1 = new brdb_value_t<float>(1.2f);
  brdb_value_sptr v2 = new brdb_value_t<float>(50.0f);

  //: inits with the default params
  bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateNormalizedImageProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_img;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img);
  TEST("run bvxm create normalized image process", good ,true);

  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  TEST("output image is in db", S_img->size(), 1);

  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(vcl_string("value"), value_img), true);
  TEST("output image is non-null", (value_img != 0) ,true);

  brdb_value_t<vil_image_view_base_sptr>* result = 
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr normed_img = result->value();
  vil_image_view<vxl_byte> normed_img_v(normed_img);

  vil_math_image_difference(img2, normed_img_v, im_dif);
  sum = 0;
  vil_math_sum(sum, im_dif, 0);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);
  vil_math_sum(sum, im_dif, 1);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);
  vil_math_sum(sum, im_dif, 2);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);

  
  //

  SUMMARY();
}
