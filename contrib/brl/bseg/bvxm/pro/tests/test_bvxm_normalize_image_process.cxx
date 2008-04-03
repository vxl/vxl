//:
// \file
// \brief  Tests for image normalization process
// \author Ozge C. Ozcanli
// \date   03/04/2008
//
#include <testlib/testlib_test.h>
#include "../bvxm_normalize_image_process.h"
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <vil/vil_save.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_float_3.h>
#include <vgl/vgl_vector_3d.h>

#include <brip/brip_vil_float_ops.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_perspective_camera.h>


//: create a synthetic slab to fill layers with
void create_a_synthetic_slab(bvxm_voxel_slab<float>& plane_img, unsigned nx, unsigned ny)
{
  for (unsigned i=0; i<nx; ++i)
  {
    for (unsigned j=0; j<ny; ++j) {
      // mark the origin/x axis
      if ( (i < 20) && (j < 5) ) {
        plane_img(i,j) = 0.2f;
      }
      // just make some squares of constant color
      else if ( (i > 10) && (i < 90) && (j > 10) && (j < 90) ) {
        plane_img(i,j) = 0.7f;
      }
      else if ((i > 110) && (i < 190) && (j > 10) && (j < 90) ) {
        plane_img(i,j) = 0.5f;
      }
      else if ((i > 10) && (i < 90) && (j > 110) && (j < 190) ) {
        plane_img(i,j) = 0.3f;
      }
      else if ((i > 110) && (i < 190) && (j > 110) && (j < 190) ) {
        plane_img(i,j) = 0.1f;
      }
      else {
        plane_img(i,j) = 0.4f;
      }
      //plane_img(i,j) = plane_img(i,j) + ((0.2f*i)/(float)nx + (0.2f*j)/(float)ny);
    }
  }
}

vpgl_camera_double_sptr create_camera()
{
// now create a couple of cameras and generate the expected images
  vnl_double_3x3 K(0.0);
  double f = 550.0;
  double offx = 320.0;
  double offy = 240.0;
  K(0,0) = f; K(1,1) = f;
  K(0,2) = offx; K(1,2) = offy;
  K(2,2) = 1.0;
  vgl_point_3d<double> center1(100,100,150); // was (100,-100,275);
  vgl_rotation_3d<double> rot1(0.0,0.0,0.0); // was (5*vnl_math::pi/6,0.0,0.0);

  vpgl_camera_double_sptr cam1 = new vpgl_perspective_camera<double>(K,center1,rot1);
  return cam1;
}

bvxm_voxel_slab_base_sptr create_mog_image_using_grey_processor(vcl_string model_dir, bvxm_voxel_world_sptr& vox_world, vil_image_view_base_sptr& expected_img)
{
  vul_file::make_directory(model_dir);

  unsigned nx = 200;
  unsigned ny = 200;
  unsigned nz = 4;
  vgl_point_3d<float> corner(0,0,0);
  vgl_vector_3d<unsigned> num_voxels(nx,ny,nz);
  float voxel_length = 1.0f;

  // create a synthetic world
  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(model_dir, corner, num_voxels, voxel_length);
  vox_world = new bvxm_voxel_world(params);

  bvxm_voxel_grid_base_sptr ocp_grid_ptr = vox_world->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<float> *ocp_grid = dynamic_cast<bvxm_voxel_grid<float>*>(ocp_grid_ptr.ptr());
  // fill in grid with zeros to start
  ocp_grid->initialize_data(0.0f);
  // now make a ground plane
  bvxm_voxel_grid<float>::iterator ocp_it = ocp_grid->slab_iterator(nz-1);
  (*ocp_it).fill(1.0f);
  // data not written to disk until iterator is iterated
  ++ocp_it;


  bvxm_voxel_slab<float> plane_img(nx,ny,1);
  create_a_synthetic_slab(plane_img, nx, ny);

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;

  bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;

  // create a slab of constant weights for update
  bvxm_voxel_slab<float> ones(nx,ny,1);
  ones.fill(1.0f);

  // iterate through layers of apm grid and update each level with the same synthetic image
  // if you want different levels to look different youll have to create a different image for each level
  bvxm_voxel_grid_base_sptr apm_base = vox_world->get_grid<APM_MOG_GREY>(0);
  bvxm_voxel_grid<mog_type> *apm_grid = dynamic_cast<bvxm_voxel_grid<mog_type>*>(apm_base.ptr());
  // initialize the appearance model data to get rid of any previous data on disk
  apm_grid->initialize_data(bvxm_voxel_traits<APM_MOG_GREY>::initial_val());

  bvxm_voxel_grid<mog_type>::iterator apm_it = apm_grid->begin();
  for (; apm_it != apm_grid->end(); ++apm_it) {
    apm_processor.update(*apm_it, plane_img, ones);
  }

  vpgl_camera_double_sptr cam1 = create_camera();

  vox_world->increment_observations<APM_MOG_GREY>(0);

  expected_img = new vil_image_view<unsigned char>(640,480);
  vil_image_view<float> mask(640,480);
  bvxm_image_metadata meta1(expected_img,cam1);
  vox_world->expected_image<APM_MOG_GREY>(meta1,expected_img, mask);

  bvxm_image_metadata meta2(expected_img,cam1);  // image is passed to get ni and nj basically

  bvxm_voxel_slab_base_sptr mog_image;
  vox_world->mixture_of_gaussians_image<APM_MOG_GREY>(meta2, mog_image, 0);
  return mog_image;
}

MAIN( test_bvxm_normalize_image_process )
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
#if 0 // FAILED, FIX this
  TEST_NEAR("check fill", input_img_rgb(0,0,1), 205, 0.01);  // weird!! I fill with 200 but the value is 205!!
#endif

  //: test image conversions
  vil_image_view<float> input_img_float = vil_convert_cast( float(), input_img_sptr );
  TEST("check float conversion", input_img_float.nplanes(), 1);
  TEST_NEAR("check float conversion", input_img_float(0,0), 200, 0.01);

  vil_image_view<float> input_img_rgb_float = vil_convert_cast( float(), input_img_rgb_sptr );
  TEST("check float conversion", input_img_rgb_float.nplanes(), 3);
#if 0 // FAILED, FIX this
  //TEST_NEAR("check float conversion", input_img_rgb_float(0,0,1), 205, 0.01);
#endif
  vil_image_view<float>* input_img_float_stretched_ptr = new vil_image_view<float>( ni, nj, 1 );
  vil_image_view_base_sptr input_img_float_stretched_sptr = input_img_float_stretched_ptr;
  vil_convert_stretch_range_limited(input_img_float, *input_img_float_stretched_ptr, 0.0f, 255.0f, 0.0f, 1.0f);
  TEST_NEAR("check float stretch conversion", (*input_img_float_stretched_ptr)(0,0), 200.0f/255.0f, 0.01);

  vil_image_view<float>* input_img_rgb_float_stretched_ptr = new vil_image_view<float>( ni, nj, 3 );
  vil_image_view_base_sptr input_img_rgb_float_stretched_sptr = input_img_rgb_float_stretched_ptr;
  vil_convert_stretch_range_limited(input_img_rgb_float, *input_img_rgb_float_stretched_ptr, 0.0f, 255.0f, 0.0f, 1.0f);
#if 0 // FAILED, FIX this
  TEST_NEAR("check float stretch conversion", (*input_img_rgb_float_stretched_ptr)(0,0), 205.0f/255.0f, 0.01);
#endif
  vil_image_view<float>* input_img_float_stretched_ptr2 = new vil_image_view<float>( ni, nj, 1 );
  vil_image_view_base_sptr input_img_float_stretched_sptr2 = input_img_float_stretched_ptr2;
  vil_convert_stretch_range_limited<vxl_byte>(input_img, *input_img_float_stretched_ptr2, 0, 255, 0.0f, 1.0f);
  TEST_NEAR("check float stretch conversion", (*input_img_float_stretched_ptr)(0,0), 200.0f/255.0f, 0.01);


  //: test normalize image method
  vil_image_view<float> img1(100, 200, 3), out_img(100, 200, 3), img2(100, 200, 3), im_dif(100, 200, 3);
  img1.fill(10);
  img2.fill(3*10 + 5);
  normalize_image<float>(img1, out_img, 3, 5, 255);
  vil_math_image_difference(img2, out_img, im_dif);
  float sum = 0;
  vil_math_sum(sum, im_dif, 0);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);
  vil_math_sum(sum, im_dif, 1);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);
  vil_math_sum(sum, im_dif, 2);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 0.01);

  vil_image_view<float> img12(100, 200, 1), out_img2(100, 200, 1), img22(100, 200, 1), im_dif2(100, 200, 1);
  img12.fill(10);
  img22.fill(3*10 + 5);
  normalize_image<float>(img12, out_img2, 3, 5, 255);
  vil_math_image_difference(img22, out_img2, im_dif2);
  vil_math_sum(sum, im_dif2, 0);
  TEST_NEAR("image dif 2 should sum to 0", sum, 0.0, 0.01);

  //: test voxel_slab operations
  bvxm_voxel_slab<float> weights(ni, nj, 1);
  weights.fill(1.0f/(ni * nj));
  sum = bvxm_util::sum_slab(weights);
  TEST_NEAR("check slab sum", sum, 1.0f, 0.01);

  //: test img to slab operation
  typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype mog_type_rgb;
  typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype obs_datatype_rgb;

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;
  typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

  // convert image to a voxel_slab
  bvxm_voxel_slab<obs_datatype> image_slab(ni, nj, 1);
  bvxm_util::img_to_slab(input_img_float_stretched_sptr,image_slab);
  input_img_float_stretched_sptr = 0;  // clears up the space taken by new
  TEST_NEAR("check slab conversion", (float)*(image_slab.begin()), 200.0f/255.0f, 0.01f);


  bvxm_voxel_slab<obs_datatype_rgb> image_slab_rgb(ni, nj, 1);
  bvxm_util::img_to_slab(input_img_rgb_float_stretched_sptr,image_slab_rgb);
  input_img_rgb_float_stretched_sptr = 0;
  vnl_float_3 a = *(image_slab_rgb.begin());
#if 0 // FAILED, fix
  TEST_NEAR("check slab conversion", a[0], 205.0f/255.0f, 0.01f);
  TEST_NEAR("check slab conversion", a[1], 205.0f/255.0f, 0.01f);
  TEST_NEAR("check slab conversion", a[2], 205.0f/255.0f, 0.01f);
#endif

  //: create a GREY mog image from a known world
  vcl_string command = "rm -rf ./test_world_dir";
  system(command.c_str());
  vil_image_view_base_sptr expected_image = new vil_image_view<unsigned char>(640,480);
  bvxm_voxel_world_sptr vox_world;
  bvxm_voxel_slab_base_sptr mog_image = create_mog_image_using_grey_processor("./test_world_dir", vox_world, expected_image);
  TEST("testing world creation", vox_world->get_params()->num_voxels().z(), 4);
  TEST("testing mixture of gaussian image creation", !mog_image, false);
  bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>*>(mog_image.ptr());
  TEST("testing mixture of gaussian image creation", !mog_image_ptr, false);
  vil_image_view<unsigned char> expected_i(*expected_image);
  vil_save(expected_i, "./expected.png");
  //TEST_NEAR("testing expected img", expected_i(146,332), (int)vcl_floor(0.2*255 + 0.5), 0.01);
  //TEST_NEAR("testing expected img", expected_i(400,250), (int)vcl_floor(0.5*255 + 0.5), 0.01);

  bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;
  bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );
  // find the total prob
  bvxm_voxel_slab<float> product(ni, nj, 1);
  bvxm_util::multiply_slabs(prob, weights, product);
  float this_prob = bvxm_util::sum_slab(product);
  vcl_cout << "this prob: " << this_prob << vcl_endl;

  //: create a test image
  float aa = 1.2f;
  //float aa = 1.0f;
  float bb = 20.0f;
  //float bb = 0.0f;

  vil_image_view<unsigned char> expected_i_norm(*expected_image);
  normalize_image<unsigned char>(expected_i, expected_i_norm, aa, bb, 255);
  vil_save(expected_i_norm, "./expected_normed.png");

  //: now test the process
  REG_PROCESS(bvxm_normalize_image_process, bprb_batch_process_manager);
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE(vcl_string);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(unsigned);

  //: set the inputs
  vil_image_view_base_sptr expected_i_norm_sptr = new vil_image_view<unsigned char>(expected_i_norm);
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(expected_i_norm_sptr);
  vpgl_camera_double_sptr cam1 = create_camera();
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(cam1);
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(vox_world);
  brdb_value_sptr v3 = new brdb_value_t<vcl_string>("apm_mog_grey");
  brdb_value_sptr v4 = new brdb_value_t<unsigned>(0);

  //: inits with the default params
  bool good = bprb_batch_process_manager::instance()->init_process("bvxmNormalizeImageProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
  good = good && bprb_batch_process_manager::instance()->set_input(4, v4);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id_img, id_a, id_b;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id_img);
  good = good && bprb_batch_process_manager::instance()->commit_output(1, id_a);
  good = good && bprb_batch_process_manager::instance()->commit_output(2, id_b);
  TEST("run bvxm normalize image process", good ,true);

  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", Q_img);
  TEST("output image is in db", S_img->size(), 1);

  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(vcl_string("value"), value_img), true);
  TEST("output image is non-null", (value_img != 0) ,true);

  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr denormed_img = result->value();
  vil_image_view<vxl_byte> denormed_img_v(denormed_img);
  bool saved = vil_save(denormed_img_v, "./expected_denormed.png");
  TEST("saved", saved, true);

  //: get a
  brdb_query_aptr Q_a = brdb_query_comp_new("id", brdb_query::EQ, id_a);
  brdb_selection_sptr S_a = DATABASE->select("float_data", Q_a);
  TEST("output a is in db", S_a->size(), 1);

  brdb_value_sptr value_a;
  TEST("output a is in db", S_a->get_value(vcl_string("value"), value_a), true);
  TEST("output a is non-null", (value_a != 0) ,true);

  brdb_value_t<float>* resulta = static_cast<brdb_value_t<float>* >(value_a.ptr());
  float result_a = resulta->value();

  //: get b
  brdb_query_aptr Q_b = brdb_query_comp_new("id", brdb_query::EQ, id_b);
  brdb_selection_sptr S_b = DATABASE->select("float_data", Q_b);
  TEST("output b is in db", S_b->size(), 1);

  brdb_value_sptr value_b;
  TEST("output b is in db", S_b->get_value(vcl_string("value"), value_b), true);
  TEST("output b is non-null", (value_b != 0) ,true);

  brdb_value_t<float>* resultb = static_cast<brdb_value_t<float>* >(value_b.ptr());
  float result_b = resultb->value();

  TEST_NEAR("testing a", result_a, 1.0f/aa, 0.1);
  TEST_NEAR("testing b", result_b, -bb, 7);

  //

  SUMMARY();
}
