// This is brl/bseg/boxm2/pro/processes/boxm2_dem_to_xyz_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to prepare x y z images from a geotiff dem, the resolutions are adjusted with respect to scene resolution.
// E.g. if DEM is a 10 m resolution and scene is of 1m resolution, then the output images are resampled versions of DEM
// given x,y,z images use ingest_dem process to initialize model
//
//  If a camera is passed as input (e.g. given by a previous process that reads it from tfw file then use it, 
//  otherwise try reading it from geotiff header
//
//
// \author Ozge C. Ozcanli
// \date May 02, 2012

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>

#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vcl_algorithm.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

#include <brip/brip_roi.h>


namespace boxm2_dem_to_xyz_process_globals
{
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 5;
}

bool boxm2_dem_to_xyz_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_dem_to_xyz_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "unsigned"; // scene refine level, 0-> original, 1->refined once, ..,3-> refined 3 times, at max resolution so dem must be re-sampled accordingly
  input_types_[2] = "vcl_string";  // geotiff image of DEM
  input_types_[3] = "double"; // lvcs is using wgs84 so wrt ellipsoid, however some DEMs are using geoid,
                              // in that case pass the distance between ellipsoid and geiod in the region
                              // to convert DEM heights to heights wrt to ellipsoid
  input_types_[4] = "bool";   // resample bilinearly
  input_types_[5] = "vpgl_camera_double_sptr";  // geocam if available, otherwise pass 0, camera will be constructed using info in geotiff header
  input_types_[6] = "float";  // some DEMs have gaps or invalid regions, pass the value in the DEM imagery that is used to fill those areas.

  // process has 1 outputs:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // x image
  output_types_[1] = "vil_image_view_base_sptr";  // y image
  output_types_[2] = "vil_image_view_base_sptr";  // z image
  output_types_[3] = "vil_image_view_base_sptr";  // cropped dem view
  output_types_[4] = "vil_image_view_base_sptr";  // cropped and resampled dem view

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool upsample_dem(vil_image_resource_sptr& out_img_res, unsigned ni, unsigned nj, vil_image_view<float>* dem_view, unsigned dem_ni, unsigned dem_nj)
{
  if (dem_ni == ni && dem_nj == nj) {
    out_img_res->put_view(*dem_view, 0,0);
    return true;
  }
  else if (dem_ni > ni || dem_nj > nj) {
    vcl_cerr << "in upsample_dem() -- cannot downsample image, image sizes are incompatible!\n";
    return false;
  }
  int block_ni = ni/dem_ni;
  int block_nj = nj/dem_nj;
  vcl_cout << "ni: " << ni << " dem_ni: " << dem_ni << " block_ni: " << block_ni << '\n'
           << "nj: " << nj << " dem_nj: " << dem_nj << " block_nj: " << block_nj << vcl_endl;
  for (unsigned i = 0; i < dem_ni; i++) {
    for (unsigned j = 0; j < dem_nj; j++) {
      vil_image_view<float> block(block_ni, block_nj, 1);
      block.fill((*dem_view)(i,j));
      out_img_res->put_view(block, i*block_ni, j*block_nj);
    }
  }
  return true;
}

bool boxm2_dem_to_xyz_process(bprb_func_process& pro)
{
  using namespace boxm2_dem_to_xyz_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  unsigned refine_cnt = pro.get_input<unsigned>(1);
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
  vcl_string geotiff_fname = pro.get_input<vcl_string>(2);
  double geoid_height = pro.get_input<double>(3);
  bool bilinear = pro.get_input<bool>(4);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(5);
  float fill_in_value = pro.get_input<float>(6);

  vil_image_resource_sptr dem_res = vil_load_image_resource(geotiff_fname.c_str());
  
  vpgl_geo_camera* geocam = 0;
  if (cam) {
    vcl_cout << "Using the loaded camera!\n";
    geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  } else
    vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);

  if (!geocam) {
    vcl_cerr << "In boxm2_dem_to_xyz_process() - the geocam could not be initialized!\n";
    return false;
  }

  vgl_box_3d<double> scene_bbox = scene->bounding_box();
  vcl_cout << "scene bbox: " << scene_bbox << vcl_endl;
  vgl_vector_3d<unsigned> dims = scene->scene_dimensions();
  vcl_vector<boxm2_block_id> blks = scene->get_block_ids();
  if (blks.size() < 1)
    return false;

   //: crop the image from the DEM
  brip_roi broi(dem_res->ni(), dem_res->nj());
  vsol_box_2d_sptr bb = new vsol_box_2d();
  
  double u,v;
  geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
  bb->add_point(u,v);
  geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
  bb->add_point(u,v);
  
  bb = broi.clip_to_image_bounds(bb);
  if (bb->width() <= 0 || bb->height() <= 0) {
    vcl_cout << "In boxm2_dem_to_xyz_process() -- " << geotiff_fname << " does not overlap the scene!\n";
    return false;
  }
  
  unsigned dem_ni = (unsigned)bb->width(); unsigned dem_nj = (unsigned)bb->height();
  vcl_cout << "dem resolution is: " << dem_ni << " by " << dem_nj << vcl_endl;

  vil_image_view_base_sptr dem_view_base = dem_res->get_view((unsigned)bb->get_min_x(), dem_ni, (unsigned)bb->get_min_y(), dem_nj);
  vil_image_view<float>* dem_view = dynamic_cast<vil_image_view<float>*>(dem_view_base.ptr());
  if (!dem_view) {
    vil_image_view<float> temp(dem_view_base->ni(), dem_view_base->nj(), 1);

    vil_image_view<vxl_int_16>* dem_view_int = dynamic_cast<vil_image_view<vxl_int_16>*>(dem_view_base.ptr());
    if (!dem_view_int) {
      vil_image_view<vxl_byte>* dem_view_byte = dynamic_cast<vil_image_view<vxl_byte>*>(dem_view_base.ptr());
      if (!dem_view_byte) {
        vcl_cerr << "Error: boxm2_dem_to_xyz_process: The image pixel format: " << dem_view_base->pixel_format() << " is not supported!\n";
        return false;
      } else
        vil_convert_cast(*dem_view_byte, temp);
    } else
      vil_convert_cast(*dem_view_int, temp);
    dem_view = new vil_image_view<float>(temp);
  }

  boxm2_scene_info* info = scene->get_blk_metadata(blks[0]);
  boxm2_block_metadata meta = scene->get_block_metadata(blks[0]);
  // each sub block is an octree, there are 8 voxels along one dimension of each sub block at the finest scale
  float sb_length = info->block_len;
  float vox_length = sb_length/8.0f;

  // prepare an image for the finest resolution
  int ni = (int)vcl_ceil((scene_bbox.max_x()-scene_bbox.min_x())/vox_length);
  int nj = (int)vcl_ceil((scene_bbox.max_y()-scene_bbox.min_y())/vox_length);
  vcl_cout << "scene image resolution needs to be at least: " << ni << " by " << nj << vcl_endl;
  // make the resolution a multiple of dem resolution
  ni = int(vcl_ceil((float)ni/dem_ni)*dem_ni);
  nj = int(vcl_ceil((float)nj/dem_nj)*dem_nj);
  vcl_cout << "made res multiple of dem_res: " << ni << " by " << nj << vcl_endl;
  vox_length = (float)((scene_bbox.max_x()-scene_bbox.min_x())/ni);

  vil_image_view<float>* out_img = new vil_image_view<float>(ni, nj, 1);
  vil_image_resource_sptr out_img_res = vil_new_image_resource_of_view(*out_img);

  // but fill it up according to the number of refinement if upsampling bilinearly
  if (bilinear) {
    // determine the level of bilinear interpolation
    int level = meta.max_level_-1;
    if (level >= int(refine_cnt))
      level -= refine_cnt;
    double scale = vcl_pow(2.0, level);
    int nib = int(ni/scale);
    int njb = int(nj/scale);
    vil_image_view<float>* out_img_temp = new vil_image_view<float>(nib, njb, 1);
    vil_resample_bilin(*dem_view, *out_img_temp, nib, njb);
    if (!upsample_dem(out_img_res, ni, nj, out_img_temp, nib, njb))
      return false;
  }
  else { // just upsample the dem
    if (!upsample_dem(out_img_res, ni, nj, dem_view, dem_ni, dem_nj))
      return false;
  }

  // create x y z images
  vil_image_view<float>* out_img_x = new vil_image_view<float>(ni, nj, 1);
  vil_image_view<float>* out_img_y = new vil_image_view<float>(ni, nj, 1);
  vil_image_view<float>* out_img_z = new vil_image_view<float>(ni, nj, 1);
  double lon,lat,gz;
  lvcs->local_to_global(0,0,0,vpgl_lvcs::wgs84,lon, lat, gz);
  vcl_cout << "lvcs origin height: " << gz << " dem height at that point: " << (*out_img)(0,0) << " adding local height of scene: " << scene_bbox.min_z() << vcl_endl;
  gz += scene_bbox.min_z();

  if (fill_in_value <= 0) 
    fill_in_value = vcl_numeric_limits<float>::max();
  
  for (int i = 0; i < ni; ++i)
    for (int j = 0; j < nj; ++j) {
      (*out_img_x)(i,j) = (float)(i*vox_length+scene_bbox.min_x()+vox_length/2.0);
      (*out_img_y)(i,j) = (float)(scene_bbox.max_y()-j*vox_length+vox_length/2.0);
      if ((*out_img)(i,j) < fill_in_value)  // otherwise it remains at local height = 0
        (*out_img_z)(i,j) = (*out_img)(i,j)-(float)gz;  // we need local height
    }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_x);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_img_y);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img_z);
  pro.set_output_val<vil_image_view_base_sptr>(3, dem_view);
  pro.set_output_val<vil_image_view_base_sptr>(4, out_img);
  return true;
}

//: Craete x y z images from the shadow height map of an ortho aerial image

namespace boxm2_shadow_heights_to_xyz_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 3;
}

bool boxm2_shadow_heights_to_xyz_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_shadow_heights_to_xyz_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vil_image_view_base_sptr";  // shadow height map image of the cropped ortho aerial image, height values are in pixels with respect to a horizontal surface at the base of the object
  input_types_[2] = "vpgl_camera_double_sptr"; // generic cam of cropped ortho aerial image (to be given by roi_init_geotiff_process)
  input_types_[3] = "vcl_string"; // dem_fname, height image values will be added to these values
  input_types_[4] = "double"; // pixel scaling

  // process has 1 outputs:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // x image
  output_types_[1] = "vil_image_view_base_sptr";  // y image
  output_types_[2] = "vil_image_view_base_sptr";  // z image

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_shadow_heights_to_xyz_process(bprb_func_process& pro)
{
  using namespace boxm2_shadow_heights_to_xyz_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(1);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(2);
  vcl_string geotiff_fname = pro.get_input<vcl_string>(3);
  double scale = pro.get_input<double>(4);

  vpgl_generic_camera<double>* gcam=dynamic_cast<vpgl_generic_camera<double>*> (cam.ptr());

  vil_image_view<float> height_img(img);
  unsigned ni = height_img.ni();
  unsigned nj = height_img.nj();

  if (gcam->rows() != nj || gcam->cols() !=ni) {
    vcl_cerr << "In boxm2_shadow_heights_to_xyz_process() - generic cam and height image sizes are not compatible!\n"
             << "gcam rows: " << gcam->rows() << " cols: " << gcam->cols() << " ni: " << ni << " nj: " << nj << '\n';
    return false;
  }

  vil_image_resource_sptr dem_res = vil_load_image_resource(geotiff_fname.c_str());
  vpgl_geo_camera* geocam = 0;
  vpgl_geo_camera::init_geo_camera(dem_res, lvcs, geocam);

  vgl_box_3d<double> scene_bbox = scene->bounding_box();
  vgl_vector_3d<unsigned> dims = scene->scene_dimensions();
  vcl_vector<boxm2_block_id> blks = scene->get_block_ids();
  if (blks.size() < 1)
    return false;

  // crop the image from the DEM
  vgl_box_2d<double> proj_bbox;
  double u,v;
  geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
  proj_bbox.add(vgl_point_2d<double>(u,v));
  geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
  proj_bbox.add(vgl_point_2d<double>(u,v));
  int min_i = int(vcl_max(0.0, vcl_floor(proj_bbox.min_x())));
  int min_j = int(vcl_max(0.0, vcl_floor(proj_bbox.min_y())));
  int max_i = int(vcl_min(dem_res->ni()-1.0, vcl_ceil(proj_bbox.max_x())));
  int max_j = int(vcl_min(dem_res->nj()-1.0, vcl_ceil(proj_bbox.max_y())));
  vcl_cout << "scene projected in the image mini: " << min_i << " minj: " << min_j << " maxi: " << max_i << " maxj: " << max_j << vcl_endl;
  unsigned int dem_ni = max_i - min_i + 1;
  unsigned int dem_nj = max_j - min_j + 1;
  vil_image_view_base_sptr dem_view_base = dem_res->get_view((unsigned int)min_i, dem_ni, (unsigned int)min_j, dem_nj);
  vil_image_view<float>* dem_view = dynamic_cast<vil_image_view<float>*>(dem_view_base.ptr());
  if (!dem_view) {
      vcl_cerr << "Error: boxm2_dem_to_xyz_process: could not cast first return image to a vil_image_view<float>\n";
      return false;
  }
  vcl_cout << "dem resolution is: " << dem_ni << " by " << dem_nj << vcl_endl;

  vil_image_view<float>* out_img = new vil_image_view<float>(ni, nj, 1);
  vil_image_resource_sptr out_img_res = vil_new_image_resource_of_view(*out_img);

  if (!upsample_dem(out_img_res, ni, nj, dem_view, dem_ni, dem_nj))
    return false;

  // create x y z images
  vil_image_view<float>* out_img_x = new vil_image_view<float>(ni, nj, 1);
  vil_image_view<float>* out_img_y = new vil_image_view<float>(ni, nj, 1);
  vil_image_view<float>* out_img_z = new vil_image_view<float>(ni, nj, 1);

  double lon,lat,gz;
  lvcs->local_to_global(0,0,0,vpgl_lvcs::wgs84,lon, lat, gz);

  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      vgl_ray_3d<double> ray = gcam->ray(i,j);
      vgl_point_3d<double> o = ray.origin();
      (*out_img_x)(i,j) = (float)o.x();
      (*out_img_y)(i,j) = (float)o.y();
      if (height_img(i,j) >= 0) {
        (*out_img_z)(i,j) = (float)(height_img(i,j)*scale+(*out_img)(i,j)-gz);  // we need local height
      }
      else
        (*out_img_z)(i,j) = (*out_img)(i,j)-(float)gz;  // we need local height
    }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_x);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_img_y);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img_z);
  return true;
}

