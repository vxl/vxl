#include "bvxm_detect_scale_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <brip/brip_vil_float_ops.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>

bvxm_detect_scale_process::bvxm_detect_scale_process()
{
  //inputs
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0] = "bvxm_voxel_world_sptr";   // world
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "vil_image_view_base_sptr";

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "unsigned";      // output an expected image of the object at the highest prob location overlayed
}


bool bvxm_detect_scale_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<bvxm_voxel_world_sptr>* input0 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[0].ptr());
  bvxm_voxel_world_sptr main_world = input0->value();

  brdb_value_t<vpgl_camera_double_sptr>* input1 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr>* >(input_data_[1].ptr());
  vpgl_camera_double_sptr camera = input1->value();

   brdb_value_t<vil_image_view_base_sptr>* input2 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());
  vil_image_view_base_sptr input_img = input2->value();

  double ni = static_cast<double>(input_img->ni());
  double nj = static_cast<double>(input_img->nj());

  double image_diag = vcl_sqrt(ni*ni + nj*nj);
  if (image_diag == 0)
    return false;

  bgeo_lvcs_sptr lvcs=main_world->get_params()->lvcs();

  vgl_point_3d<double> world_point=main_world->get_params()->world_box_local().centroid();
  vgl_plane_3d<double> world_plane(0,0,1,-world_point.z());

  //get rough diagonal in world coordinates
  vgl_point_2d<double> ul(0,0), lr(ni,nj);
  vgl_point_3d<double> wul, wlr;

  bool success = vpgl_backproject::bproj_plane(camera.ptr(), ul,
                                               world_plane,
                                               world_point,
                                               wul);

  vcl_cout<<"\n Success one";

  if (!success)
    return false;
  success = vpgl_backproject::bproj_plane(camera.ptr(), lr,
                                               world_plane,
                                               world_point,
                                               wlr);
  vcl_cout<<"\n Success two";
  if (!success)
    return false;

  //  // convert upper left position to meters
  //  double xul, yul, zul;
  //  lvcs->global_to_local(wul.x(), wul.y(), wul.z(),
  //                     bgeo_lvcs::wgs84,
  //                     xul,yul,zul,
  //                     bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  //// convert lower right position to meters
  //double xlr, ylr, zlr;
  //lvcs->global_to_local(wlr.x(), wlr.y(), wlr.z(),
  //                     bgeo_lvcs::wgs84,
  //                     xlr,ylr,zlr,
  //                     bgeo_lvcs::DEG,bgeo_lvcs::METERS);

  double world_diag = vcl_sqrt((wlr.x()-wul.x())*(wlr.x()-wul.x())+(wlr.y()-wul.y())*(wlr.y()-wul.y()));
  //shouldn't happen
  if (world_diag==0)
    return false;
  double diag_gsd = world_diag/(image_diag*main_world->get_params()->voxel_length());
  vcl_cout<<"\n Success three";
  // scale should always be greater than 0
  if (diag_gsd<0)
      return false;
  unsigned int scale=0;
  if (diag_gsd>=1)
      scale=(unsigned) vcl_ceil((vcl_log(diag_gsd)/vcl_log(2.0))-0.5);
  vcl_cout<<"The scale of the current image is "<<scale<< "and  "<<diag_gsd << vcl_endl;
  brdb_value_sptr output0 = new brdb_value_t<unsigned>(scale);
  output_data_[0] = output0;

  return true;
}

