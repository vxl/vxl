// This is brl/bseg/bvxm/pro/processes/bvxm_orthorectify_process.cxx
#include "bvxm_orthorectify_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_voxel_world.h>
#include <vil/vil_image_view.h>

//: set input and output types
bool bvxm_orthorectify_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_orthorectify_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "vil_image_view_base_sptr";  // ortho height map of the scene
  input_types_[i++] = "vpgl_camera_double_sptr";  // camera of the ortho height image
  input_types_[i++] = "vil_image_view_base_sptr";  // image to be made ortho
  input_types_[i++] = "vpgl_camera_double_sptr";  // camera of the input image
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";   // output image
  return pro.set_output_types(output_types_);
}

// generates a height map from a given camera viewpoint
bool bvxm_orthorectify_process(bprb_func_process& pro)
{
  using namespace bvxm_orthorectify_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs:

  // camera
  unsigned i = 0;
  vil_image_view_base_sptr ortho_height_img_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr ortho_height_cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr input_img_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr input_img_cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);

  //check inputs validity
  if (!input_img_cam || !ortho_height_cam) {
    std::cout << pro.name() <<" :--  Input camera is not valid!\n";
    return false;
  }
   //check inputs validity
  if (!world) {
    std::cout << pro.name() <<" :--  Input world is not valid!\n";
    return false;
  }

  if (ortho_height_img_sptr->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    std::cout << pro.name() << " -- input ortho height image needs to be a float image with ABSOLUTE heights of the scene!\n";
    return false;
  }
  unsigned ni = ortho_height_img_sptr->ni();
  unsigned nj = ortho_height_img_sptr->nj();

  //if (input_img_sptr->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
  //  std::cout << pro.name() << " -- only input images of type BYTE are supported for now! (easy to add support for others)\n";
  //  return false;
  //}
  // out image will be the ortho version of input image, it will have the same size as the ortho height map
  //vil_image_view<vxl_byte> outimg(ni, nj, 1);
  //vil_image_view<vxl_byte> input_img(input_img_sptr);

  vil_image_view<float> ortho_height_img(ortho_height_img_sptr);

  // turn the absolute height image into a height from the scene ceiling
  // subtract from the scene height to get the height from scene floor
  bvxm_world_params_sptr params = world->get_params();
  vgl_box_3d<double> box = params->world_box_local();
  float h = box.depth();
  vpgl_lvcs_sptr lvcs = params->lvcs();
  double lat, lon, elev;
  lvcs->get_origin(lat, lon, elev);
  auto base_elev = (float)elev;

  std::cout << "Using scene height: " << h << " and scene floor absolute height: " << base_elev << " to convert the absolute heights to heights from scene ceiling!\n";
  vil_image_view<float> ortho_depth_img(ni, nj, 1);
  ortho_depth_img.fill(0.0f);

  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      ortho_depth_img(i,j) = h-(ortho_height_img(i,j)-base_elev);

  vil_image_view_base_sptr ortho_depth_sptr = new vil_image_view<float>(ortho_depth_img);
  if (input_img_sptr->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> input_img(input_img_sptr);
    vil_image_view<vxl_byte> outimg(ni, nj, 1);
    world->orthorectify<vxl_byte>(ortho_depth_sptr, ortho_height_cam, input_img, input_img_cam, outimg);
    vil_image_view_base_sptr outimg_sptr = new vil_image_view<vxl_byte>(outimg);
    pro.set_output_val<vil_image_view_base_sptr>(0, outimg_sptr);
    return true;
  }
  else if (input_img_sptr->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> input_img(input_img_sptr);
    vil_image_view<float> outimg(ni, nj, 1);
    world->orthorectify<float>(ortho_depth_sptr, ortho_height_cam, input_img, input_img_cam, outimg);
    vil_image_view_base_sptr outimg_sptr = new vil_image_view<float>(outimg);
    pro.set_output_val<vil_image_view_base_sptr>(0, outimg_sptr);
    return true;
  }
  else {
    std::cout << pro.name() << " -- only input images of type BYTE and FLOAT are supported for now! (easy to add support for others)\n";
    return false;
  }

}
