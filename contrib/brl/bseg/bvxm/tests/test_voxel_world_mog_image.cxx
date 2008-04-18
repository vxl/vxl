#include <testlib/testlib_test.h>
#include <vul/vul_file.h>

#include <vgl/vgl_vector_3d.h>

#include "../bvxm_voxel_world.h"
#include "../bvxm_world_params.h"
#include "../bvxm_mog_grey_processor.h"
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>


static void test_voxel_world_mog_image()
{
  START("bvxm_voxel_world_mog_image test");

  vcl_string model_dir("./test_world_dir");
  if (vul_file::exists(model_dir)) {
    vcl_string command = "rm -rf " + model_dir;
    system(command.c_str());
  }

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
  bvxm_voxel_world_sptr vox_world = new bvxm_voxel_world(params);

  bvxm_voxel_grid_base_sptr ocp_grid_ptr = vox_world->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<float> *ocp_grid = dynamic_cast<bvxm_voxel_grid<float>*>(ocp_grid_ptr.ptr());
  // fill in grid with zeros to start
  ocp_grid->initialize_data(0.0f);
  // now make a ground plane
  bvxm_voxel_grid<float>::iterator ocp_it = ocp_grid->slab_iterator(nz-1);
  (*ocp_it).fill(1.0f);
  // data not written to disk until iterator is iterated
  ++ocp_it;

  // create a synthetic image to fill layers with
  bvxm_voxel_slab<float> plane_img(nx,ny,1);
  for (unsigned i=0; i<nx; ++i) {
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
        plane_img(i,j) = 1.0;
      }
      //plane_img(i,j) = plane_img(i,j) + ((0.2f*i)/(float)nx + (0.2f*j)/(float)ny);
    }
  }
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

 // now create a couple of cameras and generate the expected images
  vnl_matrix_fixed<double,3,3> K(0.0);
  double f = 550.0;
  double offx = 320.0;
  double offy = 240.0;
  K(0,0) = f; K(1,1) = f;
  K(0,2) = offx; K(1,2) = offy;
  K(2,2) = 1.0;
  vgl_point_3d<double> center1(100,-100,275);
  vgl_rotation_3d<double> rot1(5*vnl_math::pi/6,0.0,0.0);

  vpgl_camera_double_sptr cam1 = new vpgl_perspective_camera<double>(K,center1,rot1);
  bvxm_image_metadata meta1(vil_image_view_base_sptr(0),cam1);
  vil_image_view<float> mask(640,480,1);
  vil_image_view_base_sptr img1 = new vil_image_view<unsigned char>(640,480);

  vox_world->expected_image<APM_MOG_GREY>(meta1,img1,mask);

  // debug: write out images
  //vil_save(*img1,"./expected1.png");

  bvxm_image_metadata meta2(img1,cam1);

  bvxm_voxel_slab_base_sptr mog_image;
  TEST("testing mixture of gaussian image creation", vox_world->mixture_of_gaussians_image<APM_MOG_GREY>(meta2, mog_image, 0), true);
  TEST("testing mixture of gaussian image creation", !mog_image, false);

  bvxm_voxel_slab<mog_type>* mog_image_ptr = dynamic_cast<bvxm_voxel_slab<mog_type>*>(mog_image.ptr());
  TEST("testing mixture of gaussian image creation", !mog_image_ptr, false);

  bvxm_voxel_slab<float> prob = apm_processor.expected_color(*mog_image_ptr);

  vil_image_view_base_sptr expected_img = new vil_image_view<vxl_byte>(640,480,1);
  bvxm_util::slab_to_img(prob, expected_img);
  //vil_save(*expected_img,"./expected2.png");

  // we want the two expected images to be exactly the same
  vil_image_view<float> im_dif;
  vil_image_view<vxl_byte> expected_img_r(*expected_img);
  vil_image_view<vxl_byte> img1_r(*img1);
  vil_math_image_difference(expected_img_r, img1_r, im_dif);
  float sum;
  vil_math_sum(sum, im_dif, 0);
  TEST_NEAR("image dif should sum to 0", sum, 0.0, 5.0);  // imgs look exactly the same but there is dif of 4

  return;
}

TESTMAIN( test_voxel_world_mog_image );

