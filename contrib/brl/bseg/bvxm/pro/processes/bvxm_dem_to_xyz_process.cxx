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
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";   // bvxm voxel world
  input_types_[1] = "vcl_string";              // geotiff image name
  input_types_[2] = "double";                  // the height different used to transfer DEM geoid to lvcs wgs84 ellipsoid,
                                               // pass 0 if this coordinate difference can be ignored or fixed somewhere else
  input_types_[3] = "vpgl_camera_double_sptr"; // geocam if DEM image is not geotiff.  Pass 0 means the camera will be load from geotiff header
  input_types_[4] = "float";                   // height used to fill gap or invalid region in DEM
  
  // process has 3 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // x image
  output_types_[1] = "vil_image_view_base_sptr";  // y image
  output_types_[2] = "vil_image_view_base_sptr";  // z image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_dem_to_xyz_process(bprb_func_process& pro)
{
  using namespace bvxm_dem_to_xyz_process_globals;

  if (pro.n_inputs() < n_inputs_) {
    vcl_cout << "ERROR: " << pro.name() << ": The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }
  // get the input
  unsigned i = 0;
  bvxm_voxel_world_sptr scene = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string geotiff_fname = pro.get_input<vcl_string>(i++);
  double geoid_height = pro.get_input<double>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  float fill_in_value = pro.get_input<float>(i++);

  // get the lvcs coords for current scene
  bvxm_world_params_sptr params = scene->get_params();
  vpgl_lvcs_sptr lvcs = params->lvcs();

  // load the image/camera
  vil_image_resource_sptr dem_res = vil_load_image_resource(geotiff_fname.c_str());

  vpgl_geo_camera* geocam = 0;
  if (cam) {
    vcl_cout << pro.name() << " -- Using the loaded camera!\n";
    geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  }
  else {
    vcl_cout << pro.name() << " -- Using the camera in geotiff header\n";
    vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);
  }

  if (!geocam) {
    vcl_cout << "ERROR: " << pro.name() << ": The geocam could not be initialized!\n";
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
  vcl_cout << pro.name() << " -- Original dem resoultion: " << orig_dem_ni << ' ' << orig_dem_nj << vcl_endl;
  brip_roi broi(orig_dem_ni, orig_dem_nj);
  vsol_box_2d_sptr bb = new vsol_box_2d();

  double u, v;
  geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
  bb->add_point(u, v);
  geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
  bb->add_point(u, v);
  bb = broi.clip_to_image_bounds(bb);
  if (bb->width() <= 0 || bb->height() <= 0) {
    vcl_cout << "ERROR: " << pro.name() << " -- " << geotiff_fname << " does not overlap the scene!\n";
    return false;
  }

  vil_image_view_base_sptr dem_view_base = dem_res->get_view(0, orig_dem_ni, 0, orig_dem_nj);
  vil_image_view<float>* dem_view = dynamic_cast<vil_image_view<float>*>(dem_view_base.ptr());
  if (!dem_view) {
    vil_image_view<float> temp(dem_view_base->ni(), dem_view_base->nj(), 1);

    vil_image_view<vxl_int_16>* dem_view_int = dynamic_cast<vil_image_view<vxl_int_16>*>(dem_view_base.ptr());
    if (!dem_view_int) {
      vil_image_view<vxl_byte>* dem_view_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dem_view_base.ptr());
      if (!dem_view_byte) {
        vcl_cerr << "ERROR: " << pro.name() << ": The image pixel format: " << dem_view_base->pixel_format() << " is not supported!\n";
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
  vil_image_view<float>* out_img_x = new vil_image_view<float>(ni, nj, 1);
  vil_image_view<float>* out_img_y = new vil_image_view<float>(ni, nj, 1);
  vil_image_view<float>* out_img_z = new vil_image_view<float>(ni, nj, 1);

  // initialize the image by scene origin
  out_img_x->fill(-10.0f);
  out_img_y->fill(-10.0f);
  out_img_z->fill(-1.0f);

  vcl_cout <<   "out img x(0,0): " << ((*out_img_x)(0,0))
           << "\nout img y(0,0): " << ((*out_img_y)(0,0))
           << "\nout img z(0,0): " << ((*out_img_z)(0,0)) << vcl_endl;

  double lon, lat, gz;
  lvcs->local_to_global(0,0,0,vpgl_lvcs::wgs84, lon, lat, gz);
  vcl_cout << pro.name() << " -- lvcs origin height: " << gz << vcl_endl;
  gz += geoid_height;  // correct for the difference to geoid if necessary, geoid_height should have been passed 0 if that is not necessary

  vcl_cout << pro.name() << " -- scene min z: " << scene_bbox.min_z() << " gz: " << gz << vcl_endl;
  if (fill_in_value < 0)
    fill_in_value = vcl_numeric_limits<float>::max();

  for (int i = 0; i < ni; i++) {
    for (int j = 0; j < nj; j++) {
      float local_x = (float)(i*voxel_length+scene_bbox.min_x()+voxel_length/2.0f);
      float local_y = (float)(scene_bbox.max_y()-j*voxel_length+voxel_length/2.0f);
      (*out_img_x)(i,j) = local_x;
      (*out_img_y)(i,j) = local_y;
      double u, v;
      geocam->project(local_x, local_y, scene_bbox.min_z(), u, v);
      // for now just cast to the nearest pixel in DEM, might want to sample bi-linearly
      int uu = (int)vcl_floor(u+0.5);
      int vv = (int)vcl_floor(v+0.5);
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
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";   // bvxm voxel world
  input_types_[1] = "vcl_string";              // directory where all available DEM resources are stored
  input_types_[2] = "double";                  // the height different used to transfer DEM geoid to lvcs wgs84 ellipsoid,
                                               // pass 0 if this coordinate difference can be ignored or fixed somewhere else
  input_types_[3] = "float";                   // height used to fill gap or invalid region in DEM
  
  // process has 3 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // x image
  output_types_[1] = "vil_image_view_base_sptr";  // y image
  output_types_[2] = "vil_image_view_base_sptr";  // z image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_dem_to_xyz_process2(bprb_func_process& pro)
{
  using namespace bvxm_dem_to_xyz_process2_globales;
  if (pro.n_inputs() < n_inputs_) {
    vcl_cout << "ERROR: " << pro.name() << ": The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }
  // get the input
  unsigned i = 0;
  bvxm_voxel_world_sptr scene = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string geotiff_folder = pro.get_input<vcl_string>(i++);
  double geoid_height = pro.get_input<double>(i++);
  float fill_in_value = pro.get_input<float>(i++);
  vpgl_camera_double_sptr cam = 0;

  vcl_string file_glob = geotiff_folder + "//ASTGTM2_*.tif";
  vcl_vector<vcl_string> geotiff_img_names;
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn)
    geotiff_img_names.push_back(fn());
  if (geotiff_img_names.empty()) {
    vcl_cout << "ERROR: " << pro.name() << " -- NO dem images in given folder: " << geotiff_folder << vcl_endl;
    return false;
  }
  vcl_cout << pro.name() << " -- there are " << geotiff_img_names.size() << " dem images available for current bvxm_scene\n";

  // use previous process to generate output image
  vcl_vector<vil_image_view_base_sptr> x_imgs;
  vcl_vector<vil_image_view_base_sptr> y_imgs;
  vcl_vector<vil_image_view_base_sptr> z_imgs;

  DECLARE_FUNC_CONS(bvxm_dem_to_xyz_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvxm_dem_to_xyz_process, "bvxmDemToXYZProcess");
  REGISTER_DATATYPE(bvxm_voxel_world_sptr);
  REGISTER_DATATYPE(vpgl_camera_double_sptr);

  brdb_value_sptr v0 = new brdb_value_t<bvxm_voxel_world_sptr>(scene);
  brdb_value_sptr v2 = new brdb_value_t<double>(geoid_height);
  brdb_value_sptr v3 = new brdb_value_t<vpgl_camera_double_sptr>(cam);
  brdb_value_sptr v4 = new brdb_value_t<float>(fill_in_value);

  for (unsigned i = 0; i < geotiff_img_names.size(); i++)
  {
    vcl_string dem_img_name = geotiff_img_names[i];
    brdb_value_sptr v1 = new brdb_value_t<vcl_string>(dem_img_name);
    // run the process
    bool good = bprb_batch_process_manager::instance()->init_process("bvxmDemToXYZProcess");
    good = good && bprb_batch_process_manager::instance()->set_input(0, v0)
                && bprb_batch_process_manager::instance()->set_input(1, v1)
                && bprb_batch_process_manager::instance()->set_input(2, v2)
                && bprb_batch_process_manager::instance()->set_input(3, v3)
                && bprb_batch_process_manager::instance()->set_input(4, v4);

    bool run_good = bprb_batch_process_manager::instance()->run_process();

    if (!run_good)
      continue;

    // get the output
    unsigned id_x_img, id_y_img, id_z_img;
    good = good && bprb_batch_process_manager::instance()->commit_output(0, id_x_img)
                && bprb_batch_process_manager::instance()->commit_output(1, id_y_img)
                && bprb_batch_process_manager::instance()->commit_output(2, id_z_img);
    if (!good)  {
      vcl_cout << "ERROR: " << pro.name() << " run process for image " << dem_img_name << " failed!\n";
      return false;
    }
    brdb_query_aptr Q_x_img = brdb_query_comp_new("id", brdb_query::EQ, id_x_img);
    brdb_selection_sptr S_x_img = DATABASE->select("vil_image_view_base_sptr_data", Q_x_img);
    if (S_x_img->size() != 1) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }
    brdb_value_sptr value_x_img;
    if (!S_x_img->get_value(vcl_string("value"), value_x_img)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    if (value_x_img == 0) {
      vcl_cout << "ERROR: " << pro.name() << " obtain output x image for image " << dem_img_name << " failed!\n";
      return false;
    }
    brdb_query_aptr Q_y_img = brdb_query_comp_new("id", brdb_query::EQ, id_y_img);
    brdb_selection_sptr S_y_img = DATABASE->select("vil_image_view_base_sptr_data", Q_y_img);
    if (S_y_img->size() != 1) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }
    brdb_value_sptr value_y_img;
    if (!S_y_img->get_value(vcl_string("value"), value_y_img)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    if (value_y_img == 0) {
      vcl_cout << "ERROR: " << pro.name() << " obtain output y image for image " << dem_img_name << " failed!\n";
      return false;
    }

    brdb_query_aptr Q_z_img = brdb_query_comp_new("id", brdb_query::EQ, id_z_img);
    brdb_selection_sptr S_z_img = DATABASE->select("vil_image_view_base_sptr_data", Q_z_img);
    if (S_z_img->size() != 1) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " no selections\n";
    }
    brdb_value_sptr value_z_img;
    if (!S_z_img->get_value(vcl_string("value"), value_z_img)) {
      vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
               << " didn't get value\n";
    }
    if (value_z_img == 0) {
      vcl_cout << "ERROR: " << pro.name() << " obtain output y image for image " << dem_img_name << " failed!\n";
      return false;
    }

    brdb_value_t<vil_image_view_base_sptr>* result_x = static_cast<brdb_value_t<vil_image_view_base_sptr>*>(value_x_img.ptr());
    brdb_value_t<vil_image_view_base_sptr>* result_y = static_cast<brdb_value_t<vil_image_view_base_sptr>*>(value_y_img.ptr());
    brdb_value_t<vil_image_view_base_sptr>* result_z = static_cast<brdb_value_t<vil_image_view_base_sptr>*>(value_z_img.ptr());
    vil_image_view_base_sptr x_img = result_x->value();
    vil_image_view_base_sptr y_img = result_y->value();
    vil_image_view_base_sptr z_img = result_z->value();

    x_imgs.push_back(x_img);
    y_imgs.push_back(y_img);
    z_imgs.push_back(z_img);
  }
  unsigned num_imgs = x_imgs.size();
  if (num_imgs != y_imgs.size() || num_imgs != z_imgs.size()) {
    vcl_cout << "ERROR: " << pro.name() << " -- inconsistency of number of generate x y z images\n";
    return false;
  }

  // consistency check
  int ni_x = x_imgs[0]->ni();
  int nj_x = x_imgs[0]->nj();
  for (unsigned i = 1; i < x_imgs.size(); i++)
    if (x_imgs[i]->ni() != ni_x || x_imgs[i]->nj() != nj_x) {
      vcl_cout << "ERROR: " << pro.name() << " -- generated x imgs from various dem images are different in size\n";
      return false;
    }
  int ni_y = y_imgs[0]->ni();
  int nj_y = y_imgs[0]->nj();
  for (unsigned i = 1; i < y_imgs.size(); i++)
    if (y_imgs[i]->ni() != ni_y || y_imgs[i]->nj() != nj_y) {
      vcl_cout << "ERROR: " << pro.name() << " generated y imgs from various dem images are different in size\n";
      return false;
    }
  int ni_z = z_imgs[0]->ni();
  int nj_z = z_imgs[0]->nj();
  for (unsigned i = 1; i < z_imgs.size(); i++)
    if (z_imgs[i]->ni() != ni_z || z_imgs[i]->nj() != nj_z) {
      vcl_cout << "ERROR: " << pro.name() << " generated z imgs from various dem images are different in size\n";
      return false;
    }

  vil_image_view<float>* out_img_x = new vil_image_view<float>(ni_x, nj_x, 1);
  vil_image_view<float>* out_img_y = new vil_image_view<float>(ni_y, nj_y, 1);
  vil_image_view<float>* out_img_z = new vil_image_view<float>(ni_z, nj_z, 1);
  out_img_x->fill(-10.0f);
  out_img_y->fill(-10.0f);
  out_img_z->fill(-1.0f);

  for (unsigned img_idx = 0; img_idx < num_imgs; img_idx++)
  {
    vil_image_view<float>* x_img = dynamic_cast<vil_image_view<float>*>(x_imgs[img_idx].ptr());
    vil_image_view<float>* y_img = dynamic_cast<vil_image_view<float>*>(y_imgs[img_idx].ptr());
    vil_image_view<float>* z_img = dynamic_cast<vil_image_view<float>*>(z_imgs[img_idx].ptr());

    for (int ix = 0; ix < ni_x; ix++)
      for (int jx = 0; jx < nj_y; jx++)
        if ( (*out_img_x)(ix,jx) == -10.0f )  (*out_img_x)(ix,jx) = (*x_img)(ix,jx);

    for (int iy = 0; iy < ni_y; iy++)
      for (int jy = 0; jy < nj_y; jy++)
        if ( (*out_img_y)(iy,jy) == -10.0f )  (*out_img_y)(iy,jy) = (*y_img)(iy,jy);

    for (int iz = 0; iz < ni_z; iz++)
      for (int jz = 0; jz < nj_z; jz++)
        if ( (*out_img_z)(iz,jz) == -1.0f  )  (*out_img_z)(iz,jz) = (*z_img)(iz,jz);
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_x);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_img_y);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img_z);

  return true;
}