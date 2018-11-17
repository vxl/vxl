// This is brl/bseg/bvxm/pro/processes/bvxm_dem_to_xyz_process.cxx
#include "bvxm_dem_to_xyz_process.h"
//:
// \file

#include <brip/brip_roi.h>
#include <vsol/vsol_box_2d.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

bool bvxm_dem_to_xyz_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_dem_to_xyz_process_globals;
  // process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";   // bvxm voxel world
  input_types_[1] = "vcl_string";              // geotiff image name
  input_types_[2] = "double";                  // the height different used to transfer DEM geoid to lvcs wgs84 ellipsoid,
                                               // pass 0 if this coordinate difference can be ignored or fixed somewhere else
  input_types_[3] = "vpgl_camera_double_sptr"; // geocam if DEM image is not geotiff.  Pass 0 means the camera will be load from geotiff header
  input_types_[4] = "float";                   // height used to fill gap or invalid region in DEM

  // process has 3 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // x image
  output_types_[1] = "vil_image_view_base_sptr";  // y image
  output_types_[2] = "vil_image_view_base_sptr";  // z image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_dem_to_xyz_process(bprb_func_process& pro)
{
  using namespace bvxm_dem_to_xyz_process_globals;

  if (pro.n_inputs() < n_inputs_) {
    std::cout << "ERROR: " << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
    return false;
  }
  // get the input
  unsigned i = 0;
  bvxm_voxel_world_sptr scene = pro.get_input<bvxm_voxel_world_sptr>(i++);
  std::string geotiff_fname = pro.get_input<std::string>(i++);
  auto geoid_height = pro.get_input<double>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto fill_in_value = pro.get_input<float>(i++);

  // get the lvcs coords for current scene
  bvxm_world_params_sptr params = scene->get_params();
  vpgl_lvcs_sptr lvcs = params->lvcs();

  // load the image/camera
  vil_image_resource_sptr dem_res = vil_load_image_resource(geotiff_fname.c_str());

  vpgl_geo_camera* geocam = nullptr;
  if (cam) {
    std::cout << pro.name() << " -- Using the loaded camera!\n";
    geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  }
  else {
    std::cout << pro.name() << " -- Using the camera in geotiff header\n";
    vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);
  }

  if (!geocam) {
    std::cout << "ERROR: " << pro.name() << ": The geocam could not be initialized!\n";
    return false;
  }

  // obtain the world bounding box
  vgl_point_3d<float> lower_left = params->corner();
  vgl_vector_3d<unsigned> num_voxels = params->num_voxels();
  float voxel_length = params->voxel_length();
  double dim_x, dim_y, dim_z;
  dim_x = num_voxels.x()*voxel_length;
  dim_y = num_voxels.y()*voxel_length;
  dim_z = num_voxels.z()*voxel_length;
  vgl_point_3d<float> upper_rght(lower_left.x()+dim_x, lower_left.y()+dim_y, lower_left.z()+dim_z);

  vgl_box_3d<double> scene_bbox((double)lower_left.x(), (double)lower_left.y(), (double)lower_left.z(),
                                (double)upper_rght.x(), (double)upper_rght.y(), (double)upper_rght.z());

  // obtain the intersection
  unsigned orig_dem_ni = dem_res->ni();  unsigned orig_dem_nj = dem_res->nj();
  std::cout << pro.name() << " -- Original dem resoultion: " << orig_dem_ni << ' ' << orig_dem_nj << std::endl;
  brip_roi broi(orig_dem_ni, orig_dem_nj);
  vsol_box_2d_sptr bb = new vsol_box_2d();

  double u, v;
  geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
  bb->add_point(u, v);
  geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
  bb->add_point(u, v);
  bb = broi.clip_to_image_bounds(bb);
  if (bb->width() <= 0 || bb->height() <= 0) {
    std::cout << "ERROR: " << pro.name() << " -- " << geotiff_fname << " does not overlap the scene!\n";
    return false;
  }

  vil_image_view_base_sptr dem_view_base = dem_res->get_view(0, orig_dem_ni, 0, orig_dem_nj);
  auto* dem_view = dynamic_cast<vil_image_view<float>*>(dem_view_base.ptr());
  if (!dem_view) {
    vil_image_view<float> temp(dem_view_base->ni(), dem_view_base->nj(), 1);

    auto* dem_view_int = dynamic_cast<vil_image_view<vxl_int_16>*>(dem_view_base.ptr());
    if (!dem_view_int) {
      auto* dem_view_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dem_view_base.ptr());
      if (!dem_view_byte) {
        std::cerr << "ERROR: " << pro.name() << ": The image pixel format: " << dem_view_base->pixel_format() << " is not supported!\n";
        return false;
      }
      else {
        vil_convert_cast(*dem_view_byte, temp);
      }
    }
    else
      vil_convert_cast(*dem_view_int, temp);
    dem_view = new vil_image_view<float>(temp);
  }

  // prepare an image for the finest resolution
  int ni = (int)num_voxels.x();
  int nj = (int)num_voxels.y();

  // create x y z images
  auto* out_img_x = new vil_image_view<float>(ni, nj, 1);
  auto* out_img_y = new vil_image_view<float>(ni, nj, 1);
  auto* out_img_z = new vil_image_view<float>(ni, nj, 1);

  // initialize the image by scene origin
  out_img_x->fill(-10.0f);
  out_img_y->fill(-10.0f);
  out_img_z->fill(-1.0f);

  std::cout <<   "out img x(0,0): " << ((*out_img_x)(0,0))
           << "\nout img y(0,0): " << ((*out_img_y)(0,0))
           << "\nout img z(0,0): " << ((*out_img_z)(0,0)) << std::endl;

  double lon, lat, gz;
  lvcs->local_to_global(0,0,0,vpgl_lvcs::wgs84, lon, lat, gz);
  std::cout << pro.name() << " -- lvcs origin height: " << gz << std::endl;
  gz += geoid_height;  // correct for the difference to geoid if necessary, geoid_height should have been passed 0 if that is not necessary

  std::cout << pro.name() << " -- scene min z: " << scene_bbox.min_z() << " gz: " << gz << std::endl;
  if (fill_in_value < 0)
    fill_in_value = std::numeric_limits<float>::max();

  for (int i = 0; i < ni; i++) {
    for (int j = 0; j < nj; j++) {
      auto local_x = (float)(i*voxel_length+scene_bbox.min_x()+voxel_length/2.0f);
      auto local_y = (float)(scene_bbox.max_y()-j*voxel_length+voxel_length/2.0f);
      (*out_img_x)(i,j) = local_x;
      (*out_img_y)(i,j) = local_y;
      double u, v;
      geocam->project(local_x, local_y, scene_bbox.min_z(), u, v);
      // for now just cast to the nearest pixel in DEM, might want to sample bi-linearly
      int uu = (int)std::floor(u+0.5);
      int vv = (int)std::floor(v+0.5);
      if (uu >= 0 && vv >= 0 && uu < (int)orig_dem_ni && vv <(int)orig_dem_nj) {
        if ((*dem_view)(uu,vv) < fill_in_value)
          //(*out_img_z)(i,j) = (*dem_view)(uu,vv) - (float)gz;
          (*out_img_z)(i,j) = (*dem_view)(uu,vv);
      }
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_x);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_img_y);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img_z);

  return true;
}

bool bvxm_dem_to_xyz_process2_cons(bprb_func_process& pro)
{
  using namespace bvxm_dem_to_xyz_process2_globales;
  // process takes 5 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";   // bvxm voxel world
  input_types_[1] = "vcl_string";              // directory where all available DEM resources are stored
  input_types_[2] = "double";                  // the height different used to transfer DEM geoid to lvcs wgs84 ellipsoid,
                                               // pass 0 if this coordinate difference can be ignored or fixed somewhere else
  input_types_[3] = "float";                   // height used to fill gap or invalid region in DEM

  // process has 3 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // x image
  output_types_[1] = "vil_image_view_base_sptr";  // y image
  output_types_[2] = "vil_image_view_base_sptr";  // z image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_dem_to_xyz_process2(bprb_func_process& pro)
{
  using namespace bvxm_dem_to_xyz_process2_globales;
  if (pro.n_inputs() < n_inputs_) {
    std::cout << "ERROR: " << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
    return false;
  }
  // get the input
  unsigned i = 0;
  bvxm_voxel_world_sptr scene = pro.get_input<bvxm_voxel_world_sptr>(i++);
  std::string geotiff_folder = pro.get_input<std::string>(i++);
  auto geoid_height = pro.get_input<double>(i++);
  auto fill_in_value = pro.get_input<float>(i++);
  vpgl_camera_double_sptr cam = nullptr;

  std::string file_glob = geotiff_folder + "//*.tif";
  std::vector<std::string> geotiff_img_names;
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn)
    geotiff_img_names.emplace_back(fn());
  if (geotiff_img_names.empty()) {
    std::cout << "ERROR: " << pro.name() << " -- NO dem images in given folder: " << geotiff_folder << std::endl;
    return false;
  }

  // use previous process to generate output image
  std::vector<vil_image_view<float> > x_imgs;
  std::vector<vil_image_view<float> > y_imgs;
  std::vector<vil_image_view<float> > z_imgs;

  // get the lvcs coords for current scene
  bvxm_world_params_sptr params = scene->get_params();
  vpgl_lvcs_sptr lvcs = params->lvcs();

  // obtain the world bounding box
  vgl_point_3d<float> lower_left = params->corner();
  vgl_vector_3d<unsigned> num_voxels = params->num_voxels();
  float voxel_length = params->voxel_length();
  double dim_x, dim_y, dim_z;
  dim_x = num_voxels.x()*voxel_length;
  dim_y = num_voxels.y()*voxel_length;
  dim_z = num_voxels.z()*voxel_length;
  vgl_point_3d<float> upper_rght(lower_left.x()+dim_x, lower_left.y()+dim_y, lower_left.z()+dim_z);

  vgl_box_3d<double> scene_bbox((double)lower_left.x(), (double)lower_left.y(), (double)lower_left.z(),
                                (double)upper_rght.x(), (double)upper_rght.y(), (double)upper_rght.z());

  // prepare an image for the finest resolution
  int ni = (int)num_voxels.x();
  int nj = (int)num_voxels.y();
  double lon, lat, gz;
  lvcs->local_to_global(0,0,0,vpgl_lvcs::wgs84, lon, lat, gz);
  //gz += geoid_height;  // correct for the difference to geoid if necessary, geoid_height should have been passed 0 if that is not necessary
  if (fill_in_value < 0)
    fill_in_value = std::numeric_limits<float>::max();
  std::cout << pro.name() << " -- there are " << geotiff_img_names.size() << " dem images available for current bvxm_scene" << std::endl;
  std::cout << pro.name() << " -- the geo camera will be loaded from image headers\n";
  std::cout << pro.name() << " -- scene_info:" << std::endl;
  std::cout << "\t\t scene bounding box: " << scene_bbox << std::endl;
  std::cout << "\t\t scene voxel resoultion: " << voxel_length << std::endl;
  std::cout << pro.name() << " -- output image resolution: " << ni << "x" << nj << std::endl;
  std::cout << pro.name() << " -- invalid pixel values (-10.0, -10.0, -1.0)" << std::flush << std::endl;
  // loop over each dem image in the image folder to create x, y, z image
  std::cout << pro.name() << " -- Start to generate xyz images from " << geotiff_img_names.size() << " dem images" << std::endl;

  for (const auto& dem_img_name : geotiff_img_names)
  {
    // load the camera from image header
    vil_image_resource_sptr dem_res = vil_load_image_resource(dem_img_name.c_str());
    vpgl_geo_camera* geocam = nullptr;
    std::cout << '\t' << pro.name() << " -- Generate xyz images from " << dem_img_name << "..." << std::flush << std::endl;
    vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);
    if (!geocam) {
      std::cout << "ERROR: " << pro.name() << ": The geocam could not be initialized for DEM image header: " << dem_img_name << "!\n";
      return false;
    }
    // check the intersection
    unsigned orig_dem_ni = dem_res->ni();  unsigned orig_dem_nj = dem_res->nj();
    std::cout << pro.name() << " -- Original dem resolution: " << orig_dem_ni << ' ' << orig_dem_nj << std::endl;
    brip_roi broi(orig_dem_ni, orig_dem_nj);
    vsol_box_2d_sptr bb = new vsol_box_2d();
    double u, v;
    geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
    bb->add_point(u, v);
    geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
    bb->add_point(u, v);
    bb = broi.clip_to_image_bounds(bb);
    if (bb->width() <= 0 || bb->height() <= 0) {
      std::cout << pro.name() << " -- " << dem_img_name << " does not overlap the scene, ignored\n";
      continue;
    }

    // load the dem image
    vil_image_view_base_sptr dem_view_base = dem_res->get_view(0, orig_dem_ni, 0, orig_dem_nj);
    auto* dem_view = dynamic_cast<vil_image_view<float>*>(dem_view_base.ptr());
    if (!dem_view) {
      vil_image_view<float> temp(dem_view_base->ni(), dem_view_base->nj(), 1);

      auto* dem_view_int = dynamic_cast<vil_image_view<vxl_int_16>*>(dem_view_base.ptr());
      if (!dem_view_int) {
        auto* dem_view_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dem_view_base.ptr());
        if (!dem_view_byte) {
          std::cerr << "ERROR: " << pro.name() << ": The image pixel format: " << dem_view_base->pixel_format() << " is not supported!\n";
          return false;
        }
        else {
          vil_convert_cast(*dem_view_byte, temp);
        }
      }
      else
        vil_convert_cast(*dem_view_int, temp);
      dem_view = new vil_image_view<float>(temp);
    }

    // create x y z images
    vil_image_view<float> img_x(ni, nj, 1);
    vil_image_view<float> img_y(ni, nj, 1);
    vil_image_view<float> img_z(ni, nj, 1);

    // initialize the image by scene origin
    img_x.fill(-10.0f);
    img_y.fill(-10.0f);
    img_z.fill(-1.0f);

    if (fill_in_value < 0)
      fill_in_value = std::numeric_limits<float>::max();

    for (int i = 0; i < ni; i++) {
      for (int j = 0; j < nj; j++) {
        auto local_x = (float)(i*voxel_length+scene_bbox.min_x()+voxel_length/2.0f);
        auto local_y = (float)(scene_bbox.max_y()-j*voxel_length+voxel_length/2.0f);
        (img_x)(i,j) = local_x;
        (img_y)(i,j) = local_y;
        double u, v;
        geocam->project(local_x, local_y, scene_bbox.min_z(), u, v);
        // for now just cast to the nearest pixel in DEM, might want to sample bi-linearly
        int uu = (int)std::floor(u+0.5);
        int vv = (int)std::floor(v+0.5);
        if (uu >= 0 && vv >= 0 && uu < (int)orig_dem_ni && vv <(int)orig_dem_nj) {
          if ((*dem_view)(uu,vv) < fill_in_value)
            (img_z)(i,j) = (*dem_view)(uu,vv);
        }
      }
    }
    x_imgs.push_back(img_x);
    y_imgs.push_back(img_y);
    z_imgs.push_back(img_z);
  }
  // combine all the images
  unsigned num_imgs = x_imgs.size();
  if (num_imgs != y_imgs.size() || num_imgs != z_imgs.size()) {
    std::cout << "ERROR: " << pro.name() << " -- inconsistency of number of generate x y z images\n";
    return false;
  }
  std::cout << num_imgs << " are created from " << geotiff_img_names.size() << " ASTER DEM images, combine them now ... " << std::endl;

  auto* out_img_x = new vil_image_view<float>(ni, nj, 1);
  auto* out_img_y = new vil_image_view<float>(ni, nj, 1);
  auto* out_img_z = new vil_image_view<float>(ni, nj, 1);
  out_img_x->fill(-10.0f);
  out_img_y->fill(-10.0f);
  out_img_z->fill(-1.0f);

  for (unsigned img_idx = 0; img_idx < num_imgs; img_idx++)
  {
    vil_image_view<float> x_img = x_imgs[img_idx];
    vil_image_view<float> y_img = y_imgs[img_idx];
    vil_image_view<float> z_img = z_imgs[img_idx];

    for (unsigned ix = 0; ix < ni; ix++)
      for (unsigned jx = 0; jx < nj; jx++)
        if ( (*out_img_x)(ix,jx) == -10.0f )  (*out_img_x)(ix,jx) = (x_img)(ix,jx);

    for (unsigned iy = 0; iy < ni; iy++)
      for (unsigned jy = 0; jy < nj; jy++)
        if ( (*out_img_y)(iy,jy) == -10.0f )  (*out_img_y)(iy,jy) = (y_img)(iy,jy);

    for (unsigned iz = 0; iz < ni; iz++)
      for (unsigned jz = 0; jz < nj; jz++)
        if ( (*out_img_z)(iz,jz) == -1.0f  )  (*out_img_z)(iz,jz) = (z_img)(iz,jz);
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_x);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_img_y);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img_z);

  return true;
}
