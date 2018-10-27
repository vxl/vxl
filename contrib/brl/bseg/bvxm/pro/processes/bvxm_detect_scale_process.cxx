//This is brl/bseg/bvxm/pro/processes/ bvxm_detect_scale_process.cxx
#include "bvxm_detect_scale_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>

#include <vil/vil_image_view_base.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>

//: set input and output types
bool bvxm_detect_scale_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_detect_scale_process_globals;
  //inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";   // world
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]= "unsigned";      // output an expected image of the object at the highest prob location overlayed
  return pro.set_output_types(output_types_);
}

//: detects the scale of a voxel world
bool bvxm_detect_scale_process(bprb_func_process& pro)
{
  using namespace bvxm_detect_scale_process_globals;
  // Sanity check
 //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() <<" : The input number should be "<< n_inputs_ << std::endl;
    return false;
  }

  //get the inputs
  unsigned i =0;
  bvxm_voxel_world_sptr main_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr input_img = pro.get_input<vil_image_view_base_sptr>(i++);

    //check input's validity
  if (!main_world) {
    std::cout << pro.name() <<" :--  Input0  is not valid!\n";
    return false;
  }
  if (!camera) {
    std::cout << pro.name() <<" :--  Input1  is not valid!\n";
    return false;
  }
  if (!input_img) {
    std::cout << pro.name() <<" :--  Input2  is not valid!\n";
    return false;
  }


  auto ni = static_cast<double>(input_img->ni());
  auto nj = static_cast<double>(input_img->nj());

  double image_diag = std::sqrt(ni*ni + nj*nj);
  if (image_diag == 0)
    return false;

  vpgl_lvcs_sptr lvcs=main_world->get_params()->lvcs();

  vgl_point_3d<double> world_point=main_world->get_params()->world_box_local().centroid();
  vgl_plane_3d<double> world_plane(0,0,1,-world_point.z());

  //get rough diagonal in world coordinates
  vgl_point_2d<double> ul(0,0), lr(ni,nj);
  vgl_point_3d<double> wul, wlr;

  bool success = vpgl_backproject::bproj_plane(camera.ptr(), ul,
                                               world_plane,
                                               world_point,
                                               wul);

  std::cout<<"\n Success one";

  if (!success)
    return false;
  success = vpgl_backproject::bproj_plane(camera.ptr(), lr,
                                          world_plane,
                                          world_point,
                                          wlr);
  std::cout<<"\n Success two";
  if (!success)
    return false;

  //  // convert upper left position to meters
  //  double xul, yul, zul;
  //  lvcs->global_to_local(wul.x(), wul.y(), wul.z(),
  //                     vpgl_lvcs::wgs84,
  //                     xul,yul,zul,
  //                     vpgl_lvcs::DEG,vpgl_lvcs::METERS);
  //// convert lower right position to meters
  //double xlr, ylr, zlr;
  //lvcs->global_to_local(wlr.x(), wlr.y(), wlr.z(),
  //                     vpgl_lvcs::wgs84,
  //                     xlr,ylr,zlr,
  //                     vpgl_lvcs::DEG,vpgl_lvcs::METERS);

  double world_diag = std::sqrt((wlr.x()-wul.x())*(wlr.x()-wul.x())+(wlr.y()-wul.y())*(wlr.y()-wul.y()));
  //shouldn't happen
  if (world_diag==0)
    return false;
  double diag_gsd = world_diag/(image_diag*main_world->get_params()->voxel_length());
  std::cout<<"\n Success three";
  // scale should always be greater than 0
  if (diag_gsd<0)
      return false;
  unsigned int scale=0;
  if (diag_gsd>=1)
      scale=(unsigned) std::ceil((std::log(diag_gsd)/std::log(2.0))-0.5);
  std::cout<<"The scale of the current image is "<<scale<< "and  "<<diag_gsd << std::endl;

  pro.set_output_val<unsigned>(0, scale);

  return true;
}
