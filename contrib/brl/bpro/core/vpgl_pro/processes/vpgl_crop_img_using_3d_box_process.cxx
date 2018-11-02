// This is brl/brpo/core/vpgl_pro/processes/vpgl_crop_img_using_3d_box_process.cxx
//:
// \file
//        Take a cam, an uncertainty value in meter unit and a 3D bounding box in wgs84 coordinates to generate a cropped image
//        Note the outputs are the cropped camera and the pixel values use to represent the image region being cropped
//
//
//
#include <iostream>
#include <vil/vil_config.h>
#if HAS_GEOTIFF

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <bprb/bprb_func_process.h>
#include <brip/brip_roi.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


// global variables and functions
namespace vpgl_crop_img_using_3d_box_process_globals
{
  constexpr unsigned n_inputs_ = 11;
  constexpr unsigned n_outputs_ = 5;
}

// === functions ===
bool project_box(const vpgl_rational_camera<double>& rat_cam, const vpgl_lvcs_sptr &lvcs_sptr,
    const vgl_box_3d<double> &scene_bbox, double uncertainty,
    vgl_box_2d<double> &roi_box_2d);

void create_local_rational_camera(const vpgl_rational_camera<double>& rat_cam, const vpgl_lvcs_sptr& lvcs_sptr,
  const vsol_box_2d_sptr& bb, vpgl_local_rational_camera<double>& local_camera);


// initialization
bool vpgl_crop_img_using_3d_box_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_crop_img_using_3d_box_process_globals;
  // process takes 10 inputs
  std::vector<std::string> input_types_(n_inputs_);
  //input_types_[0] = "vil_image_resource_sptr";  // image resource
  input_types_[0] = "unsigned";  // image resource ni
  input_types_[1] = "unsigned";  // image resource nj
  input_types_[2] = "vpgl_camera_double_sptr";  // rational camera
  input_types_[3] = "double";                   // lower_left_lon
  input_types_[4] = "double";                   // lower_left_lat
  input_types_[5] = "double";                   // lower_left_elev
  input_types_[6] = "double";                   // upper_right_lon
  input_types_[7] = "double";                   // upper_right_lat
  input_types_[8] = "double";                   // upper_right_elev
  input_types_[9] = "double";                   // uncertainty value (in meter units)
  input_types_[10] = "vpgl_lvcs_sptr";           // lvcs

  // process takes 5 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";  // crop rational camera
  output_types_[1] = "unsigned";                 // image pixel i0
  output_types_[2] = "unsigned";                 // image pixel j0
  output_types_[3] = "unsigned";                 // image size ni
  output_types_[4] = "unsigned";                 // image size nj

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // set input defaults
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;
  pro.set_input(10, new brdb_value_t<vpgl_lvcs_sptr>(lvcs));

  return good;
}

// execute the process
bool vpgl_crop_img_using_3d_box_process(bprb_func_process& pro)
{
  using namespace vpgl_crop_img_using_3d_box_process_globals;
  // sanity check
  //if (pro.n_inputs() != n_inputs_) {
  //  std::cout << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
  //  return false;
  //}
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << ": The input is wrong!!!" << std::endl;
    return false;
  }

  // get the input
  unsigned i = 0;
  //vil_image_resource_sptr img_res_sptr = pro.get_input<vil_image_resource_sptr>(i++);  // image resource
  auto img_res_ni = pro.get_input<unsigned>(i++);
  auto img_res_nj = pro.get_input<unsigned>(i++);
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);      // rational camera
  auto lower_left_lon = pro.get_input<double>(i++);
  auto lower_left_lat = pro.get_input<double>(i++);
  auto lower_left_elev = pro.get_input<double>(i++);
  auto upper_right_lon = pro.get_input<double>(i++);
  auto upper_right_lat = pro.get_input<double>(i++);
  auto upper_right_elev = pro.get_input<double>(i++);
  auto uncertainty = pro.get_input<double>(i++);
  vpgl_lvcs_sptr lvcs_sptr= pro.get_input<vpgl_lvcs_sptr>(i++);

  auto* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    std::cout << pro.name() << ": the input camera is not a rational camera" << std::endl;
    return false;
  }

  // generate a lvcs coordinates to transfer camera offset coordinates
  double ori_lon, ori_lat, ori_elev;
  lvcs_sptr->get_origin(ori_lat, ori_lon, ori_elev);
  if ( (ori_lat+ori_lon+ori_elev)*(ori_lat+ori_lon+ori_elev) < 1E-7) {
    lvcs_sptr = new vpgl_lvcs(lower_left_lat, lower_left_lon, lower_left_elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  }

  vgl_box_3d<double> scene_bbox(lower_left_lon, lower_left_lat, lower_left_elev,
                        upper_right_lon, upper_right_lat, upper_right_elev);

  vgl_box_2d<double> roi_box_2d;
  bool good = project_box(*rat_cam, lvcs_sptr, scene_bbox, uncertainty, roi_box_2d);
  if(!good) {
    return false;
  }
  std::cout << pro.name() << ": projected 2d roi box: " << roi_box_2d << " given uncertainty " << uncertainty << " meters." << std::endl;

  // crop the image
  //brip_roi broi(img_res_sptr->ni(), img_res_sptr->nj());
  brip_roi broi(img_res_ni, img_res_nj);
  vsol_box_2d_sptr bb = new vsol_box_2d();
  bb->add_point(roi_box_2d.min_x(), roi_box_2d.min_y());
  bb->add_point(roi_box_2d.max_x(), roi_box_2d.max_y());
  bb = broi.clip_to_image_bounds(bb);

  // store output
  auto i0 = (unsigned)bb->get_min_x();
  auto j0 = (unsigned)bb->get_min_y();
  auto ni = (unsigned)bb->width();
  auto nj = (unsigned)bb->height();

  if (ni <= 0 || nj <= 0)
  {
    std::cout << pro.name() << ": clipping box is out of image boundary, empty crop image returned" << std::endl;
    return true;
  }

  // create the local camera
  vpgl_local_rational_camera<double> local_camera;
  create_local_rational_camera(*rat_cam, lvcs_sptr, bb, local_camera);

  // store output
  unsigned out_j = 0;
  pro.set_output_val<vpgl_camera_double_sptr>(out_j++, new vpgl_local_rational_camera<double>(local_camera));
  pro.set_output_val<unsigned>(out_j++, i0);
  pro.set_output_val<unsigned>(out_j++, j0);
  pro.set_output_val<unsigned>(out_j++, ni);
  pro.set_output_val<unsigned>(out_j++, nj);
  return true;
}

// global variables and functions
namespace vpgl_offset_cam_using_3d_box_process_globals
{
  constexpr unsigned n_inputs_ = 9;
  constexpr unsigned n_outputs_ = 5;
}

// initialization
bool vpgl_offset_cam_using_3d_box_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_offset_cam_using_3d_box_process_globals;
  // process takes 9 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";  // rational camera
  input_types_[1] = "double";                   // lower_left_lon
  input_types_[2] = "double";                   // lower_left_lat
  input_types_[3] = "double";                   // lower_left_elev
  input_types_[4] = "double";                   // upper_right_lon
  input_types_[5] = "double";                   // upper_right_lat
  input_types_[6] = "double";                   // upper_right_elev
  input_types_[7] = "double";                   // uncertainty value (in meter units)
  input_types_[8] = "vpgl_lvcs_sptr";           // lvcs

  // process takes 5 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";  // crop rational camera
  output_types_[1] = "unsigned";                 // image pixel i0
  output_types_[2] = "unsigned";                 // image pixel j0
  output_types_[3] = "unsigned";                 // image size ni
  output_types_[4] = "unsigned";                 // image size nj

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // set input defaults
  brdb_value_sptr lvcs = new brdb_value_t<vpgl_lvcs_sptr>;
  pro.set_input(8, lvcs);

  return good;
}

// execute the process
bool vpgl_offset_cam_using_3d_box_process(bprb_func_process& pro)
{
  using namespace vpgl_offset_cam_using_3d_box_process_globals;
  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the input
  unsigned i = 0;
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(i++); // rational camera
  auto lower_left_lon = pro.get_input<double>(i++);
  auto lower_left_lat = pro.get_input<double>(i++);
  auto lower_left_elev = pro.get_input<double>(i++);
  auto upper_right_lon = pro.get_input<double>(i++);
  auto upper_right_lat = pro.get_input<double>(i++);
  auto upper_right_elev = pro.get_input<double>(i++);
  auto uncertainty = pro.get_input<double>(i++);
  vpgl_lvcs_sptr lvcs_sptr= pro.get_input<vpgl_lvcs_sptr>(i++);

  auto* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    std::cout << pro.name() << ": the input camera is not a rational camera" << std::endl;
    return false;
  }

  // generate a lvcs coordinates to transfer camera offset coordinates
  if(!lvcs_sptr) {
    lvcs_sptr = new vpgl_lvcs(lower_left_lat, lower_left_lon, lower_left_elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  }

  vgl_box_3d<double> scene_bbox(lower_left_lon, lower_left_lat, lower_left_elev,
                        upper_right_lon, upper_right_lat, upper_right_elev);

  vgl_box_2d<double> roi_box_2d;
  bool good = project_box(*rat_cam, lvcs_sptr, scene_bbox, uncertainty, roi_box_2d);
  if(!good) {
    return false;
  }
  std::cout << pro.name() << ": projected 2d roi box: " << roi_box_2d << " given uncertainty " << uncertainty << " meters." << std::endl;

  // crop the image
  vsol_box_2d_sptr bb = new vsol_box_2d();
  bb->add_point(roi_box_2d.min_x(), roi_box_2d.min_y());
  bb->add_point(roi_box_2d.max_x(), roi_box_2d.max_y());

  // store output
  int i0 = bb->get_min_x();
  auto ni = (unsigned)bb->width();
  if(i0 < 0) {
    ni += i0;
    i0 = 0;
  }

  int j0 = bb->get_min_y();
  auto nj = (unsigned)bb->height();
  if(j0 < 0) {
    nj += j0;
    j0 = 0;
  }

  if (ni <= 0 || nj <= 0) {
    std::cout << pro.name() << ": projected box too small" << std::endl;
    return false;
  }

  // create the local camera
  vpgl_local_rational_camera<double> local_camera;
  create_local_rational_camera(*rat_cam, lvcs_sptr, bb, local_camera);

  // store output
  unsigned out_j = 0;
  pro.set_output_val<vpgl_camera_double_sptr>(out_j++, new vpgl_local_rational_camera<double>(local_camera));
  pro.set_output_val<unsigned>(out_j++, i0);
  pro.set_output_val<unsigned>(out_j++, j0);
  pro.set_output_val<unsigned>(out_j++, ni);
  pro.set_output_val<unsigned>(out_j++, nj);
  return true;
}

void create_local_rational_camera(const vpgl_rational_camera<double>& rat_cam, const vpgl_lvcs_sptr& lvcs_sptr,
  const vsol_box_2d_sptr& bb, vpgl_local_rational_camera<double>& local_camera)
{
  // calculate local camera offset from image bounding box
  double global_u, global_v, local_u, local_v;
  rat_cam.image_offset(global_u, global_v);
  local_u = std::floor(global_u - bb->get_min_x());  // the image was cropped by pixel
  local_v = std::floor(global_v - bb->get_min_y());
  // create the local camera
  local_camera = vpgl_local_rational_camera<double>(*lvcs_sptr, rat_cam);
  local_camera.set_image_offset(local_u, local_v);
}

bool project_box(const vpgl_rational_camera<double>& rat_cam, const vpgl_lvcs_sptr &lvcs_sptr,
    const vgl_box_3d<double> &scene_bbox, double uncertainty,
    vgl_box_2d<double> &roi_box_2d)
{
  // project box
  double xoff, yoff, zoff;
  xoff = rat_cam.offset(vpgl_rational_camera<double>::X_INDX);
  yoff = rat_cam.offset(vpgl_rational_camera<double>::Y_INDX);
  zoff = rat_cam.offset(vpgl_rational_camera<double>::Z_INDX);

  // global to lcoal (wgs84 to meter in order to apply uncertainty)
  double lx, ly, lz;
  lvcs_sptr->global_to_local(xoff, yoff, zoff, vpgl_lvcs::wgs84, lx, ly, lz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double center[3];
  center[0] = lx;  center[1] = ly;  center[2] = lz;

  // create a camera box with uncertainty
  vgl_box_3d<double> cam_box(center, 2*uncertainty, 2*uncertainty, 2*uncertainty, vgl_box_3d<double>::centre);
  std::vector<vgl_point_3d<double> > cam_corners = cam_box.vertices();

  // create the 3D box given input coordinates (in geo-coordinates)
  std::vector<vgl_point_3d<double> > box_corners = scene_bbox.vertices();

  // projection
  double lon, lat, gz;
  for (auto & cam_corner : cam_corners)
  {
    lvcs_sptr->local_to_global(cam_corner.x(), cam_corner.y(), cam_corner.z(), vpgl_lvcs::wgs84,
                          lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    vpgl_rational_camera<double>* new_cam = rat_cam.clone();
    new_cam->set_offset(vpgl_rational_camera<double>::X_INDX, lon);
    new_cam->set_offset(vpgl_rational_camera<double>::Y_INDX, lat);
    new_cam->set_offset(vpgl_rational_camera<double>::Z_INDX, gz);

    // project the box to image coords
    for (auto & box_corner : box_corners) {
      vgl_point_2d<double> p2d = new_cam->project(vgl_point_3d<double>(box_corner.x(), box_corner.y(), box_corner.z()));
      roi_box_2d.add(p2d);
    }
    delete new_cam;
  }

  return true;
}

// global variables and functions
namespace vpgl_crop_ortho_using_3d_box_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 5;
}

// initialization
bool vpgl_crop_ortho_using_3d_box_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_crop_ortho_using_3d_box_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_resource_sptr";  // ortho image resource
  input_types_[1] = "vpgl_camera_double_sptr";  // ortho camera as a vpgl_geo_camera
  input_types_[2] = "double";                   // lower_left_lon
  input_types_[3] = "double";                   // lower_left_lat
  input_types_[4] = "double";                   // lower_left_elev
  input_types_[5] = "double";                   // upper_right_lon
  input_types_[6] = "double";                   // upper_right_lat
  input_types_[7] = "double";                   // upper_right_elev

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr"; // geocam of cropped image
  output_types_[1] = "unsigned";                 // image pixel i0
  output_types_[2] = "unsigned";                 // image pixel j0
  output_types_[3] = "unsigned";                 // image size ni
  output_types_[4] = "unsigned";                 // image size nj
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

// execute the process
bool vpgl_crop_ortho_using_3d_box_process(bprb_func_process& pro)
{
  using namespace vpgl_crop_ortho_using_3d_box_process_globals;
  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the input
  unsigned i = 0;
  vil_image_resource_sptr img_res_sptr = pro.get_input<vil_image_resource_sptr>(i++);  // image resource
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());

  auto lower_left_lon = pro.get_input<double>(i++);
  auto lower_left_lat = pro.get_input<double>(i++);
  auto lower_left_elev = pro.get_input<double>(i++);
  auto upper_right_lon = pro.get_input<double>(i++);
  auto upper_right_lat = pro.get_input<double>(i++);
  auto upper_right_elev = pro.get_input<double>(i++);

  // create the 3D box given input coordinates (in geo-coordinates)
  vgl_box_3d<double> bbox(lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev);
  std::vector<vgl_point_3d<double> > box_corners = bbox.vertices();

  // projection
  vgl_box_2d<double> roi_box_2d;

  // project the box to image coords
  for (auto & box_corner : box_corners) {
    double u, v;
    geocam->global_to_img(box_corner.x(), box_corner.y(), box_corner.z(), u, v);
    vgl_point_2d<double> p2d(u, v);
    roi_box_2d.add(p2d);
  }

  std::cout << pro.name() << ": projected 2d roi box: " << roi_box_2d << std::endl;

  // crop the image
  brip_roi broi(img_res_sptr->ni(), img_res_sptr->nj());
  vsol_box_2d_sptr bb = new vsol_box_2d();
  bb->add_point(roi_box_2d.min_x(), roi_box_2d.min_y());
  bb->add_point(roi_box_2d.max_x(), roi_box_2d.max_y());
  bb = broi.clip_to_image_bounds(bb);

  auto i0 = (unsigned)bb->get_min_x();
  auto j0 = (unsigned)bb->get_min_y();
  auto ni = (unsigned)bb->width();
  auto nj = (unsigned)bb->height();

  if (ni <= 0 || nj <= 0)
  {
    std::cout << pro.name() << ": clipping box is out of image boundary, empty crop image returned" << std::endl;
    return false;
  }
  if (i0 > img_res_sptr->ni() || j0 > img_res_sptr->nj())
  {
    std::cout << pro.name() << ": clipping box is out of image boundary, empty crop image returned" << std::endl;
    return false;
  }

  if ( (i0+ni) > img_res_sptr->ni() && (j0+nj) > img_res_sptr->nj())
  {
    std::cout << pro.name() << ": clipping box is out of image boundary, empty crop image returned" << std::endl;
    return false;
  }

  // create an ortho geocam for the cropped image  -- CAUTION: assumes that the image is aligned East-North (no rotation)
  double lon0, lat0, lonn, latn;
  geocam->img_to_global(i0, j0, lon0, lat0);
  geocam->img_to_global(i0+ni-1, j0+nj-1, lonn, latn);
  double scalingx = (lonn - lon0)/(ni-1);
  double scalingy = (latn - lat0)/(nj-1);

  vnl_matrix<double> trans_matrix(4,4,0.0);
  trans_matrix[0][0] = scalingx;
  trans_matrix[0][1] = 0.0;
  trans_matrix[1][0] = 0.0;
  trans_matrix[1][1] = scalingy;
  trans_matrix[0][3] = lon0;
  trans_matrix[1][3] = lat0;
  trans_matrix[3][3] = 1.0;

  vpgl_geo_camera* camera = new vpgl_geo_camera(trans_matrix, geocam->lvcs());
  camera->set_scale_format(true);

  // store output
  unsigned out_j = 0;
  pro.set_output_val<vpgl_camera_double_sptr>(out_j++, camera);
  pro.set_output_val<unsigned>(out_j++, i0);
  pro.set_output_val<unsigned>(out_j++, j0);
  pro.set_output_val<unsigned>(out_j++, ni);
  pro.set_output_val<unsigned>(out_j++, nj);

  return true;
}

//: process to crop image using its rational camera and a given region.  Note that the elevation values are retrieved from
//  ASTER DEM height maps, which are Geotiff images
// global variables and functions
namespace vpgl_crop_img_using_3d_box_dem_process_globals
{
  constexpr unsigned n_inputs_ = 10;
  constexpr unsigned n_outputs_ = 5;
  //: find the min and max height in a given region from height map resources
  bool find_min_max_height(double const& ll_lon, double const& ll_lat, double const& uu_lon, double const& uu_lat,
                           std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& infos,
                           double& min, double& max);
  void crop_and_find_min_max(std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& infos,
                             unsigned const& img_id, int const& i0, int const& j0, int const& crop_ni, int const& crop_nj,
                             double& min, double& max);
}

// initialization
bool vpgl_crop_img_using_3d_box_dem_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_crop_img_using_3d_box_dem_process_globals;
  // process takes 10 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_resource_sptr";  // image resource
  input_types_[1] = "vpgl_camera_double_sptr";  // rational camera
  input_types_[2] = "double";                   // lower left lon
  input_types_[3] = "double";                   // lower left lat
  input_types_[4] = "double";                   // upper right lon
  input_types_[5] = "double";                   // upper right lat
  input_types_[6] = "vcl_string";               // ASTER DEM image folder
  input_types_[7] = "double";                   // the amount to be added on top of the terrain height
  input_types_[8] = "double";                   // uncertainty values (in meter units)
  input_types_[9] = "vpgl_lvcs_sptr";           // lvcs

  // process takes 5 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";  // local crop rational camera
  output_types_[1] = "unsigned";                 // image pixel i0
  output_types_[2] = "unsigned";                 // image pixel j0
  output_types_[3] = "unsigned";                 // image size ni
  output_types_[4] = "unsigned";                 // image size nj
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // set input default
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs;
  pro.set_input(9, new brdb_value_t<vpgl_lvcs_sptr>(lvcs));
  return good;
}

// execute the process
bool vpgl_crop_img_using_3d_box_dem_process(bprb_func_process& pro)
{
  using namespace vpgl_crop_img_using_3d_box_dem_process_globals;
  // sanity check
  if (!pro.verify_inputs())
  {
    std::cerr << pro.name() << ": Wrong inputs!!!" << std::endl;
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_resource_sptr img_res_sptr = pro.get_input<vil_image_resource_sptr>(in_i++);  // image resource
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);      // rational camera
  auto lower_left_lon = pro.get_input<double>(in_i++);
  auto lower_left_lat = pro.get_input<double>(in_i++);
  auto upper_right_lon = pro.get_input<double>(in_i++);
  auto upper_right_lat = pro.get_input<double>(in_i++);
  std::string dem_folder = pro.get_input<std::string>(in_i++);
  auto box_height = pro.get_input<double>(in_i++);
  auto uncertainty = pro.get_input<double>(in_i++);
  vpgl_lvcs_sptr lvcs_sptr = pro.get_input<vpgl_lvcs_sptr>(in_i++);

  auto* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    std::cerr << pro.name() << ": the input camera is not a rational camera!\n";
    return false;
  }

  // load the height map resources
  std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> > infos;
  std::string file_glob = dem_folder + "/*.tif";
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn)
  {
    std::string filename = fn();
    vil_image_view_base_sptr img_r = vil_load(filename.c_str());
    vpgl_geo_camera* cam;
    vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
    vil_image_resource_sptr img_res = vil_load_image_resource(filename.c_str());
    if (!vpgl_geo_camera::init_geo_camera(img_res, lvcs_dummy, cam)) {
      std::cerr << pro.name() << ": Given height map " << filename << " is NOT a GeoTiff!\n";
      return false;
    }
    infos.emplace_back(img_r, cam);
  }
  if (infos.empty()) {
    std::cerr << pro.name() << ": No image in the folder: " << dem_folder << std::endl;
    return false;
  }

  // obtain the height values from height maps
  double min = 10000.0, max = -10000.0;
  if (!find_min_max_height(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, infos, min, max)) {
    std::cerr << pro.name() << ": find min and max height failed!!!\n";
    return false;
  }
  double lower_left_elev = min;
  double upper_right_elev = max + box_height;

  std::cout << pro.name() << " lower_left_elev: " << lower_left_elev << ", upper_right_elev: " << upper_right_elev << std::endl;
  // generate local lvcs to transfer camera offset coordinates
  double ori_lon, ori_lat, ori_elev;
  lvcs_sptr->get_origin(ori_lat, ori_lon, ori_elev);
  if ( (ori_lat+ori_lon+ori_elev)*(ori_lat+ori_lon+ori_elev) < 1E-7) {
    lvcs_sptr = new vpgl_lvcs(lower_left_lat, lower_left_lon, lower_left_elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  }
  vgl_box_3d<double> scene_bbox(lower_left_lon, lower_left_lat, lower_left_elev,
                                upper_right_lon, upper_right_lat, upper_right_elev);

  vgl_box_2d<double> roi_box_2d;
  bool good = project_box(*rat_cam, lvcs_sptr, scene_bbox, uncertainty, roi_box_2d);
  if(!good) {
    return false;
  }
  std::cout << pro.name() << ": projected 2d roi box: " << roi_box_2d << " given uncertainty " << uncertainty << " meters." << std::endl;
  // crop the image
  brip_roi broi(img_res_sptr->ni(), img_res_sptr->nj());
  vsol_box_2d_sptr bb = new vsol_box_2d();
  bb->add_point(roi_box_2d.min_x(), roi_box_2d.min_y());
  bb->add_point(roi_box_2d.max_x(), roi_box_2d.max_y());
  bb = broi.clip_to_image_bounds(bb);
  // store output
  auto i0 = (unsigned)bb->get_min_x();
  auto j0 = (unsigned)bb->get_min_y();
  auto ni = (unsigned)bb->width();
  auto nj = (unsigned)bb->height();

  if (ni <= 0 || nj <= 0)
  {
    std::cout << pro.name() << ": clipping box is out of image boundary, empty crop image returned" << std::endl;
    return false;
  }
  // create the local camera
  vpgl_local_rational_camera<double> local_camera;
  create_local_rational_camera(*rat_cam, lvcs_sptr, bb, local_camera);

  // store output
  unsigned out_j = 0;
  pro.set_output_val<vpgl_camera_double_sptr>(out_j++, new vpgl_local_rational_camera<double>(local_camera));
  pro.set_output_val<unsigned>(out_j++, i0);
  pro.set_output_val<unsigned>(out_j++, j0);
  pro.set_output_val<unsigned>(out_j++, ni);
  pro.set_output_val<unsigned>(out_j++, nj);
  return true;
}

bool vpgl_crop_img_using_3d_box_dem_process_globals::find_min_max_height(double const& ll_lon, double const& ll_lat, double const& ur_lon, double const& ur_lat,
                                                                         std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& infos,
                                                                         double& min, double& max)
{
  // find the corner points
  std::vector<std::pair<unsigned, std::pair<int, int> > > corners;
  std::vector<vgl_point_2d<double> > pts;
  pts.emplace_back(ll_lon, ur_lat);
  pts.emplace_back(ur_lon, ll_lat);
  pts.emplace_back(ll_lon, ll_lat);
  pts.emplace_back(ur_lon, ur_lat);
  for (auto & pt : pts)
  {
    // find the image
    for (unsigned j = 0; j < (unsigned)infos.size(); j++)
    {
      double u, v;
      infos[j].second->global_to_img(pt.x(), pt.y(), 0, u, v);
      int uu = (int)std::floor(u+0.5);
      int vv = (int)std::floor(v+0.5);
      if (uu < 0 || vv < 0 || uu >= (int)infos[j].first->ni() || vv >= (int)infos[j].first->nj())
        continue;
      std::pair<unsigned, std::pair<int, int> > pp(j, std::pair<int, int>(uu, vv));
      corners.push_back(pp);
      break;
    }
  }
  if (corners.size() != 4) {
    std::cerr << "Cannot locate all 4 corners among given DEM tiles!\n";
    return false;
  }
  // case 1 all corners are in the same image
  if (corners[0].first == corners[1].first) {
    // crop the image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = corners[1].second.first-corners[0].second.first+1;
    int crop_nj = corners[1].second.second-corners[0].second.second+1;
    crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 2: two corners are in the same image
  if (corners[0].first == corners[2].first && corners[1].first == corners[3].first) {
    // crop the first image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = infos[corners[0].first].first->ni() - corners[0].second.first;
    int crop_nj = corners[2].second.second-corners[0].second.second+1;
    crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

    // crop the second image
    i0 = 0;
    j0 = corners[3].second.second;
    crop_ni = corners[3].second.first + 1;
    crop_nj = corners[1].second.second-corners[3].second.second+1;
    crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 3: two corners are in the same image
  if (corners[0].first == corners[3].first && corners[1].first == corners[2].first) {
    // crop the first image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = corners[3].second.first - corners[0].second.first + 1;
    int crop_nj = infos[corners[0].first].first->nj() - corners[0].second.second;
    crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

    // crop the second image
    i0 = corners[2].second.first;
    j0 = 0;
    crop_ni = corners[1].second.first - corners[2].second.first + 1;
    crop_nj = corners[2].second.second + 1;
    crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 4: all corners are in a different image
  // crop the first image, image of corner 0
  int i0 = corners[0].second.first;
  int j0 = corners[0].second.second;
  int crop_ni = infos[corners[0].first].first->ni() - corners[0].second.first;
  int crop_nj = infos[corners[0].first].first->nj() - corners[0].second.second;
  crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the second image, image of corner 1
  i0 = 0;
  j0 = 0;
  crop_ni = corners[1].second.first + 1;
  crop_nj = corners[1].second.second + 1;
  crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the third image, image of corner 2
  i0 = corners[2].second.first;
  j0 = 0;
  crop_ni = infos[corners[2].first].first->ni() - corners[2].second.first;
  crop_nj = corners[2].second.second + 1;
  crop_and_find_min_max(infos, corners[2].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the fourth image, image of corner 3
  i0 = 0;
  j0 = corners[3].second.second;
  crop_ni = corners[3].second.first + 1;
  crop_nj = infos[corners[3].first].first->nj() - corners[3].second.second;
  crop_and_find_min_max(infos, corners[3].first, i0, j0, crop_ni, crop_nj, min, max);
  return true;
}

void vpgl_crop_img_using_3d_box_dem_process_globals::crop_and_find_min_max(std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& infos,
                                                                           unsigned const& img_id, int const& i0, int const& j0, int const& crop_ni, int const& crop_nj,
                                                                           double& min, double& max)
{
  if (auto* img = dynamic_cast<vil_image_view<vxl_int_16>*>(infos[img_id].first.ptr()))
  {
    vil_image_view<vxl_int_16> img_crop = vil_crop(*img, i0, crop_ni, j0, crop_nj);
    for (unsigned ii = 0; ii < img_crop.ni(); ii++) {
      for (unsigned jj = 0; jj < img_crop.nj(); jj++) {
        if (min > img_crop(ii, jj)) min = img_crop(ii,jj);
        if (max < img_crop(ii, jj)) max = img_crop(ii,jj);
      }
    }
  }
  else if (auto* img = dynamic_cast<vil_image_view<float>*>(infos[img_id].first.ptr()))
  {
    vil_image_view<float> img_crop = vil_crop(*img, i0, crop_ni, j0, crop_nj);
    for (unsigned ii = 0; ii < img_crop.ni(); ii++) {
      for (unsigned jj = 0; jj < img_crop.nj(); jj++) {
        if (min > img_crop(ii, jj)) min = img_crop(ii,jj);
        if (max < img_crop(ii, jj)) max = img_crop(ii,jj);
      }
    }
  }
  return;
}
#endif
