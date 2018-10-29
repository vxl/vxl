//:
// \file
// \brief  Tests for image normalization process
// \author Ozge C. Ozcanli
// \date   March 04, 2008
//
#include <string>
#include <iostream>
#include <testlib/testlib_test.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/pro/processes/bvxm_normalization_util.h>

#include <vcl_compiler.h>
#include <vcl_compiler.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

#include <vil/vil_save.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <brip/brip_vil_float_ops.h>
#include <vul/vul_file.h>
#include <vpgl/vpgl_perspective_camera.h>


//: create a synthetic slab to fill layers with
void create_a_synthetic_slab(bvxm_voxel_slab<float>& plane_img, unsigned nx, unsigned ny)
{
  for (unsigned i=0; i<nx; ++i)
  {
    for (unsigned j=0; j<ny; ++j)
    {
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

bvxm_voxel_slab_base_sptr create_mog_image_using_grey_processor(std::string model_dir, bvxm_voxel_world_sptr& vox_world, vil_image_view_base_sptr& expected_img)
{
  vul_file::make_directory(model_dir);

  unsigned nx = 200;
  unsigned ny = 200;
  unsigned nz = 4;
  vgl_point_3d<float> corner(0,0,0);
  vgl_vector_3d<unsigned> num_voxels(nx,ny,nz);
  float voxel_length = 1.0f;
  unsigned scale=0;
  // create a synthetic world
  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(model_dir, corner, num_voxels, voxel_length);
  vox_world = new bvxm_voxel_world(params);

  bvxm_voxel_grid_base_sptr ocp_grid_ptr = vox_world->get_grid<OCCUPANCY>(0,scale);
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
  bvxm_voxel_grid_base_sptr apm_base = vox_world->get_grid<APM_MOG_GREY>(0,scale);
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

static void test_bvxm_normalize_image_process()
{
  DECLARE_FUNC_CONS(bvxm_normalize_image_process);
  DECLARE_FUNC_CONS(bvxm_create_mog_image_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_normalize_image_process, "bvxmNormalizeImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_create_mog_image_process,"bvxmCreateMOGImageProcess");
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE( bvxm_voxel_slab_base_sptr );
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);
  REGISTER_DATATYPE_LONG_FORM(std::string,vcl_string);
  REGISTER_DATATYPE(float);
  REGISTER_DATATYPE(unsigned);

  unsigned ni = 640, nj = 480;

  // first test various stages in the normalization process
  vil_image_view<vxl_byte> input_img(ni, nj, 1);
  input_img.fill(200);
  vil_image_view_base_sptr input_img_sptr = new vil_image_view<vxl_byte>(input_img);
  std::cout << "format: " << input_img_sptr->pixel_format() << std::endl;
  TEST("check byte", input_img_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE, true);

  vil_image_view<vxl_byte> input_img_rgb(ni, nj, 3);
  input_img.fill(200);
  vil_image_view_base_sptr input_img_rgb_sptr = new vil_image_view<vxl_byte>(input_img_rgb);
  std::cout << "format: " << input_img_rgb_sptr->pixel_format() << std::endl;
  TEST("check byte", input_img_rgb_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE, true);

  // test image conversions
  vil_image_view<float> input_img_float = vil_convert_cast( float(), input_img_sptr );
  TEST("check float conversion", input_img_float.nplanes(), 1);
  TEST_NEAR("check float conversion", input_img_float(0,0), 200, 0.01);

  vil_image_view<float> input_img_rgb_float = vil_convert_cast( float(), input_img_rgb_sptr );
  TEST("check float conversion", input_img_rgb_float.nplanes(), 3);
  vil_image_view<float>* input_img_float_stretched_ptr = new vil_image_view<float>( ni, nj, 1 );
  vil_image_view_base_sptr input_img_float_stretched_sptr = input_img_float_stretched_ptr;
  vil_convert_stretch_range_limited(input_img_float, *input_img_float_stretched_ptr, 0.0f, 255.0f, 0.0f, 1.0f);
  TEST_NEAR("check float stretch conversion", (*input_img_float_stretched_ptr)(0,0), 200.0f/255.0f, 0.01);

  vil_image_view<float>* input_img_rgb_float_stretched_ptr = new vil_image_view<float>( ni, nj, 3 );
  vil_image_view_base_sptr input_img_rgb_float_stretched_sptr = input_img_rgb_float_stretched_ptr;
  vil_convert_stretch_range_limited(input_img_rgb_float, *input_img_rgb_float_stretched_ptr, 0.0f, 255.0f, 0.0f, 1.0f);
  vil_image_view<float>* input_img_float_stretched_ptr2 = new vil_image_view<float>( ni, nj, 1 );
  vil_image_view_base_sptr input_img_float_stretched_sptr2 = input_img_float_stretched_ptr2;
  vil_convert_stretch_range_limited<vxl_byte>(input_img, *input_img_float_stretched_ptr2, 0, 255, 0.0f, 1.0f);

  // test normalize image method
  vil_image_view<float> img1(100, 200, 3), out_img(100, 200, 3), img2(100, 200, 3), im_dif(100, 200, 3);
  img1.fill(10);
  img2.fill(3*10 + 5);
  bvxm_normalization_util::normalize_image(img1, out_img, 3, 5, 255);
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
  bvxm_normalization_util::normalize_image(img12, out_img2, 3, 5, 255);
  vil_math_image_difference(img22, out_img2, im_dif2);
  vil_math_sum(sum, im_dif2, 0);
  TEST_NEAR("image dif 2 should sum to 0", sum, 0.0, 0.01);

  // test voxel_slab operations
  bvxm_voxel_slab<float> weights(ni, nj, 1);
  weights.fill(1.0f/float(ni * nj));
  sum = bvxm_util::sum_slab(weights);
  TEST_NEAR("check slab sum", sum, 1.0f, 0.01);

  // test img to slab operation
  typedef bvxm_voxel_traits<APM_MOG_RGB>::voxel_datatype mog_type_rgb;
  typedef bvxm_voxel_traits<APM_MOG_RGB>::obs_datatype obs_datatype_rgb;

  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;
  typedef bvxm_voxel_traits<APM_MOG_GREY>::obs_datatype obs_datatype;

  // convert image to a voxel_slab
  bvxm_voxel_slab<obs_datatype> image_slab(ni, nj, 1);
  bvxm_util::img_to_slab(input_img_float_stretched_sptr,image_slab);
  input_img_float_stretched_sptr = nullptr;  // clears up the space taken by new
  TEST_NEAR("check slab conversion", (float)*(image_slab.begin()), 200.0f/255.0f, 0.01f);


  bvxm_voxel_slab<obs_datatype_rgb> image_slab_rgb(ni, nj, 1);
  bvxm_util::img_to_slab(input_img_rgb_float_stretched_sptr,image_slab_rgb);
  input_img_rgb_float_stretched_sptr = nullptr;

  // create a GREY mog image from a known world
  std::string model_dir("test_world_dir");
  if (vul_file::is_directory(model_dir))
    vul_file::delete_file_glob(model_dir+"/*");
  else {
    if (vul_file::exists(model_dir))
      vul_file::delete_file_glob(model_dir);
    vul_file::make_directory(model_dir);
  }

  vil_image_view_base_sptr expected_image = new vil_image_view<unsigned char>(ni,nj);
  bvxm_voxel_world_sptr vox_world;
  bvxm_voxel_slab_base_sptr mog_image = create_mog_image_using_grey_processor(model_dir.c_str(), vox_world, expected_image);
  TEST("testing world creation", vox_world->get_params()->num_voxels().z(), 4);
  TEST("testing mixture of gaussian image creation", !mog_image, false);
  bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>*>(mog_image.ptr());
  TEST("testing mixture of gaussian image creation", !mog_image_ptr, false);
  vil_image_view<unsigned char> expected_i(*expected_image);
  vil_save(expected_i, "./expected.png");
  //TEST_NEAR("testing expected img", expected_i(146,332), (int)std::floor(0.2*255 + 0.5), 0.01);
  //TEST_NEAR("testing expected img", expected_i(400,250), (int)std::floor(0.5*255 + 0.5), 0.01);

  bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;
  bvxm_voxel_slab<float> prob = apm_processor.prob_density(*mog_image_ptr,image_slab); //prob( nimg );
  // find the total prob
  bvxm_voxel_slab<float> product(ni, nj, 1);
  bvxm_util::multiply_slabs(prob, weights, product);
  float this_prob = bvxm_util::sum_slab(product);
  std::cout << "this prob: " << this_prob << std::endl;

  // create a test image
  float aa = 1.2f;
  //float aa = 1.0f;
  float bb = 20.0f;
  //float bb = 0.0f;

  vil_image_view<unsigned char> expected_i_norm(*expected_image);
  bvxm_normalization_util::normalize_image(expected_i, expected_i_norm, aa, bb, 255);
  vil_save(expected_i_norm, "./expected_normed.png");

  // set the inputs
  vil_image_view_base_sptr expected_i_norm_sptr = new vil_image_view<unsigned char>(expected_i_norm);
  brdb_value_sptr v0 = new brdb_value_t<vil_image_view_base_sptr>(expected_i_norm_sptr);
  vpgl_camera_double_sptr cam1 = create_camera();
  brdb_value_sptr v1 = new brdb_value_t<vpgl_camera_double_sptr>(cam1);
  brdb_value_sptr v2 = new brdb_value_t<bvxm_voxel_world_sptr>(vox_world);
  brdb_value_sptr v3 = new brdb_value_t<std::string>("apm_mog_grey");
  brdb_value_sptr v4 = new brdb_value_t<unsigned>(0);
  brdb_value_sptr v5 = new brdb_value_t<unsigned>(0);
  brdb_value_sptr v6 = new brdb_value_t<unsigned>(ni);
  brdb_value_sptr v7 = new brdb_value_t<unsigned>(nj);

  bool good = bprb_batch_process_manager::instance()->init_process("bvxmCreateMOGImageProcess")
           && bprb_batch_process_manager::instance()->set_input(0, v2) // voxel world
           && bprb_batch_process_manager::instance()->set_input(1, v3) // apm type
           && bprb_batch_process_manager::instance()->set_input(2, v4) // scale and bin ids
           && bprb_batch_process_manager::instance()->set_input(3, v5)
           && bprb_batch_process_manager::instance()->set_input(4, v1)  // camera
           && bprb_batch_process_manager::instance()->set_input(5, v6)  // ni of MOG image
           && bprb_batch_process_manager::instance()->set_input(6, v7); // n7 of MOG image
  TEST("bprb_batch_process_manager init_process", good, true);

  good = bprb_batch_process_manager::instance()->run_process();
  TEST("bprb_batch_process_manager run_process", good, true);

  unsigned id_slab;
  good = bprb_batch_process_manager::instance()->commit_output(0, id_slab);
  TEST("bprb_batch_process_manager commit_output", good, true);

  brdb_query_aptr Q_slab = brdb_query_comp_new("id", brdb_query::EQ, id_slab);
  brdb_selection_sptr S_slab = DATABASE->select("bvxm_voxel_slab_base_sptr_data", std::move(Q_slab));
  TEST("output slab is in db", S_slab->size(), 1);

  brdb_value_sptr value_slab;
  TEST("output slab is in db", S_slab->get_value(std::string("value"), value_slab), true);
  TEST("output slab is non-null", (value_slab != nullptr) ,true);

  good = bprb_batch_process_manager::instance()->init_process("bvxmNormalizeImageProcess")
      && bprb_batch_process_manager::instance()->set_input(0, v0)
      && bprb_batch_process_manager::instance()->set_input(1, value_slab)
      && bprb_batch_process_manager::instance()->set_input(2, v3);
  TEST("bprb_batch_process_manager init_process", good, true);

  good = bprb_batch_process_manager::instance()->run_process();
  TEST("run bvxm normalize image process", good ,true);

  unsigned id_img, id_a, id_b;
  good = bprb_batch_process_manager::instance()->commit_output(0, id_img)
      && bprb_batch_process_manager::instance()->commit_output(1, id_a)
      && bprb_batch_process_manager::instance()->commit_output(2, id_b);
  TEST("bprb_batch_process_manager commit_output", good, true);

  brdb_query_aptr Q_img = brdb_query_comp_new("id", brdb_query::EQ, id_img);
  brdb_selection_sptr S_img = DATABASE->select("vil_image_view_base_sptr_data", std::move(Q_img));
  TEST("output image is in db", S_img->size(), 1);

  brdb_value_sptr value_img;
  TEST("output image is in db", S_img->get_value(std::string("value"), value_img), true);
  TEST("output image is non-null", (value_img != nullptr) ,true);

  brdb_value_t<vil_image_view_base_sptr>* result =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(value_img.ptr());
  vil_image_view_base_sptr denormed_img = result->value();
  vil_image_view<vxl_byte> denormed_img_v(denormed_img);
  bool saved = vil_save(denormed_img_v, "./expected_denormed.png");
  TEST("saved", saved, true);

  // get a
  brdb_query_aptr Q_a = brdb_query_comp_new("id", brdb_query::EQ, id_a);
  brdb_selection_sptr S_a = DATABASE->select("float_data", std::move(Q_a));
  TEST("output a is in db", S_a->size(), 1);

  brdb_value_sptr value_a;
  TEST("output a is in db", S_a->get_value(std::string("value"), value_a), true);
  TEST("output a is non-null", (value_a != nullptr) ,true);

  brdb_value_t<float>* resulta = static_cast<brdb_value_t<float>* >(value_a.ptr());
  float result_a = resulta->value();

  // get b
  brdb_query_aptr Q_b = brdb_query_comp_new("id", brdb_query::EQ, id_b);
  brdb_selection_sptr S_b = DATABASE->select("float_data", std::move(Q_b));
  TEST("output b is in db", S_b->size(), 1);

  brdb_value_sptr value_b;
  TEST("output b is in db", S_b->get_value(std::string("value"), value_b), true);
  TEST("output b is non-null", (value_b != nullptr) ,true);

  brdb_value_t<float>* resultb = static_cast<brdb_value_t<float>* >(value_b.ptr());
  float result_b = resultb->value();

  TEST_NEAR("testing a", result_a, 1.0f/aa, 0.1);
  TEST_NEAR("testing b", result_b, -bb, 7);
}

TESTMAIN(test_bvxm_normalize_image_process);
