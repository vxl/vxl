//This is brl/bseg/bvxm/pro/processes/bvxm_lidar_init_process.cxx
#include "bvxm_lidar_init_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_util.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vsol/vsol_box_2d.h>

#include <vil/vil_config.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/file_formats/vil_tiff.h>

#include <brip/brip_roi.h>

//: set input and output types
bool bvxm_lidar_init_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_lidar_init_process_globals;


  //this process takes 3 input:
  //the filename of the image, the camera and the voxel world
  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "vcl_string";             // first ret. image path (geotiff)
  input_types_[i++] = "vcl_string";             // second ret. image path (geotiff)
  input_types_[i++] = "bvxm_voxel_world_sptr";  // rational camera
  if (!pro.set_input_types(input_types_))
    return false;

    // output
  std::vector<std::string> output_types_(n_outputs_);
  unsigned j =0;
  output_types_[j++]= "vpgl_camera_double_sptr";   // lidar local camera
  output_types_[j++]= "vil_image_view_base_sptr";  // first ret image ROI
  output_types_[j++]= "vil_image_view_base_sptr";  // second ret image ROI
  output_types_[j++]= "vil_image_view_base_sptr";  // mask
  return (bool) pro.set_output_types(output_types_);
}


bool bvxm_lidar_init_process(bprb_func_process& pro)
{
  using namespace bvxm_lidar_init_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs:
  // image
  unsigned i = 0;
  std::string first = pro.get_input<std::string>(i++);
  std::string second = pro.get_input<std::string>(i++);
  //voxel_world
  bvxm_voxel_world_sptr voxel_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  //get parameters:
  // threshold (meters)
  float thresh=10.0f;
  pro.parameters()->get_value(param_mask_thresh_, thresh);

  bvxm_world_params_sptr world_params = voxel_world->get_params();
  vil_image_resource_sptr first_ret = vil_load_image_resource(first.c_str());
  if (!first_ret) {
    std::cout << "bvxm_lidar_init_process -- First return image path is not valid!\n";
    return false;
  }

  // second return may be null, in that case only first return will be considered
  vil_image_resource_sptr second_ret = vil_load_image_resource(second.c_str());

  vpgl_lvcs_sptr lvcs = world_params->lvcs();
  if (!lvcs) {
    std::cout << "bvxm_lidar_init_process -- LVCS is not set!\n";
    return false;
  }

  vil_image_view_base_sptr roi_first=nullptr, roi_second=nullptr;
  vpgl_geo_camera *cam_first=nullptr, *cam_second=nullptr;

  if (!lidar_init(first_ret, world_params, roi_first, cam_first)) {
    std::cout << "bvxm_lidar_init_process -- The process has failed!\n";
    return false;
  }

  if (second_ret) {
    if (!lidar_init(second_ret, world_params, roi_second, cam_second)) {
      std::cout << "bvxm_lidar_init_process -- The process has failed!\n";
      return false;
    }
  }

  vil_image_view_base_sptr mask=nullptr;
  if (!gen_mask(roi_first, cam_first, roi_second, cam_second, mask, thresh)) {
    std::cout << "bvxm_lidar_init_process -- The process has failed!\n";
    return false;
  }

  unsigned j = 0;
  // store the camera (camera of first return is sufficient)
  pro.set_output_val<vpgl_camera_double_sptr >(j++, cam_first);
   // store image output (first return roi)
  pro.set_output_val<vil_image_view_base_sptr>(j++,roi_first);
  // store image output (second return roi)
  pro.set_output_val<vil_image_view_base_sptr>(j++,roi_second);
  // store the mask
  pro.set_output_val<vil_image_view_base_sptr>(j++,mask);
  return true;
}


bool bvxm_lidar_init_process_globals::lidar_init( const vil_image_resource_sptr& lidar,
                                                  const bvxm_world_params_sptr& params,
                                                  vil_image_view_base_sptr& roi,
                                                  vpgl_geo_camera*& camera)
{
  // the file should be a geotiff
  std::cout << "FORMAT=" << lidar->file_format();
  if (std::strcmp(lidar->file_format(),"tiff") != 0) {
    std::cout << "bvxm_lidar_init_process::lidar_init -- The image should be a TIFF!\n";
    return false;
  }

#if HAS_GEOTIFF
  auto* tiff_img = static_cast<vil_tiff_image*> (lidar.as_pointer());

  if (vpgl_geo_camera::init_geo_camera(tiff_img, params->lvcs(), camera))
  {
    vgl_box_2d<double> roi_box;

    double lon, lat, elev;

    // backproject the 3D world coordinates on the image
    vgl_box_3d<double> world = params->world_box_local();
    std::vector<vgl_point_3d<double> > corners = bvxm_util::corners_of_box_3d<double>(world);
    for (auto & corner : corners) {
      double x = corner.x();
      double y = corner.y();
      double z = corner.z();
      double u,v;
      camera->project(x,y,z,u,v);
      vgl_point_2d<double> p(u,v);
      roi_box.add(p);
    }

    std::cout << *(camera->lvcs()) << std::endl;
    camera->lvcs()->local_to_global(200,200,0,vpgl_lvcs::wgs84,lon,lat,elev);
    std::cout << "corner--> lon=" << lon << "  lat=" << lat << " gz=" << elev << std::endl;
    brip_roi broi(tiff_img->ni(), tiff_img->nj());
    vsol_box_2d_sptr bb = new vsol_box_2d();
    bb->add_point(roi_box.min_x(), roi_box.min_y());
    bb->add_point(roi_box.max_x(), roi_box.max_y());

    bb = broi.clip_to_image_bounds(bb);
    if ((bb->width() > 0) && (bb->height() > 0)) {
      roi = tiff_img->get_copy_view((unsigned int)bb->get_min_x(),
                                    (unsigned int)bb->width(),
                                    (unsigned int)bb->get_min_y(),
                                    (unsigned int)bb->height());
      //add the translation to the camera
      camera->translate(bb->get_min_x(), bb->get_min_y(),0);
    }

    if (!roi) {
      std::cout << "bvxm_lidar_init_process::lidar_init()-- clipping box is out of image boundaries\n";
      return false;
    }
  }
  else {
    std::cout << "bvxm_lidar_init_process::lidar_init()-- Only ProjectedCSTypeGeoKey=PCS_WGS84_UTM_zoneXX_X is defined rigth now, please define yours!!" << std::endl;
    return false;
  }

  return true;
#else // if !HAS_GEOTIFF
  std::cout << "bvxm_lidar_init_process::lidar_init()-- GEOTIFF lib is needed to run bvxm_lidar_init_process--\n";
  return false;
#endif // HAS_GEOTIFF
}

bool bvxm_lidar_init_process_globals::gen_mask( const vil_image_view_base_sptr& roi_first,
                                                vpgl_geo_camera* cam_first,
                                                const vil_image_view_base_sptr& roi_second,
                                                vpgl_geo_camera* cam_second,
                                                vil_image_view_base_sptr& mask,
                                                double thresh)
{
  // compare the cameras, if the second one existed
  if (!cam_first) {
    std::cout << "bvxm_lidar_init_process::gen_mask -- camera not found!\n";
    return false;
  }

  if (!roi_first) {
    std::cout << "bvxm_lidar_init_process::gen_mask -- image not found!\n";
    return false;
  }

  auto* view = new vil_image_view<bool>(roi_first->ni(), roi_first->nj());
  // if there is no second camera and image, just use one
  if (!roi_second || !cam_second) {
    view->fill(false);
    mask = view;
  }
  else {
    assert(roi_first->ni() == roi_second->ni());
    assert(roi_first->nj() == roi_second->nj());

    if (roi_first->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
      auto* view1 = static_cast<vil_image_view<float>*> (roi_first.as_pointer());
      auto* view2 = static_cast<vil_image_view<float>*> (roi_second.as_pointer());
      // compare the cameras, they should be the same
      for (unsigned i=0; i<roi_first->ni(); i++)
        for (unsigned j=0; j<roi_first->nj(); j++) {
        double diff = (*view1)(i,j)-(*view2)(i,j);
        if (diff > thresh)
          (*view)(i,j) = false;
        else
          (*view)(i,j) = true;
        }
    }
    mask = view;
  }
  return true;
}
