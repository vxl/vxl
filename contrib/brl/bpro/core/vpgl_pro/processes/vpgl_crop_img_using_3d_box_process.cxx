// This is brl/bbas/volm/pro/processes/vpgl_crop_img_using_geo_coords.cxx
//:
// \file
//        Take a cam, an uncertainty value in meter unit and a 3D bounding box in wgs84 coordinates to generate a cropped image
//        Note the outputs are the cropped camera and the pixel values use to represent the image region being cropped
//
//
//
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
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>
#include <vcl_iostream.h>

// global variables and functions
namespace vpgl_crop_img_using_3d_box_process_globals
{
  const unsigned n_inputs_ = 9;
  const unsigned n_outputs_ = 5;
  
  // return all 4 corners of the 3d box
  vcl_vector<vgl_point_3d<double> > corner_of_box_3d(vgl_box_3d<double> box);

}

// initialization
bool vpgl_crop_img_using_3d_box_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_crop_img_using_3d_box_process_globals;
  // process takes 7 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_resource_sptr";  // image resource
  input_types_[1] = "vpgl_camera_double_sptr";  // rational camera
  input_types_[2] = "double";                   // lower_left_lon
  input_types_[3] = "double";                   // lower_left_lat
  input_types_[4] = "double";                   // lower_left_elev
  input_types_[5] = "double";                   // upper_right_lon
  input_types_[6] = "double";                   // upper_right_lat
  input_types_[7] = "double";                   // upper_right_elev
  input_types_[8] = "double";                   // uncertainty value (in meter units)

  // process takes 5 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";  // crop rational camera
  output_types_[1] = "unsigned";                 // image pixel i0
  output_types_[2] = "unsigned";                 // image pixel j0
  output_types_[3] = "unsigned";                 // image size ni
  output_types_[4] = "unsigned";                 // image size nj
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

// execute the process
bool vpgl_crop_img_using_3d_box_process(bprb_func_process& pro)
{
  using namespace vpgl_crop_img_using_3d_box_process_globals;
  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the input
  unsigned i = 0;
  vil_image_resource_sptr img_res_sptr = pro.get_input<vil_image_resource_sptr>(i++);  // image resource
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);      // rational camera
  double lower_left_lon   = pro.get_input<double>(i++);
  double lower_left_lat   = pro.get_input<double>(i++);
  double lower_left_elev  = pro.get_input<double>(i++);
  double upper_right_lon  = pro.get_input<double>(i++);
  double upper_right_lat  = pro.get_input<double>(i++);
  double upper_right_elev = pro.get_input<double>(i++);
  double uncertainty      = pro.get_input<double>(i++);

  vpgl_rational_camera<double>* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    vcl_cout << pro.name() << ": the input camera is not a rational camera" << vcl_endl;
    return false;
  }

  // generate a lvcs coordinates to transfer camera offset coordinates
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lower_left_lat, lower_left_lon, lower_left_elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  
  // project box
  double xoff, yoff, zoff;
  xoff = rat_cam->offset(vpgl_rational_camera<double>::X_INDX);
  yoff = rat_cam->offset(vpgl_rational_camera<double>::Y_INDX);
  zoff = rat_cam->offset(vpgl_rational_camera<double>::Z_INDX);

  // global to lcoal (wgs84 to meter in order to apply uncertainty)
  double lx, ly, lz;
  lvcs->global_to_local(xoff, yoff, zoff, vpgl_lvcs::wgs84, lx, ly, lz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  double center[3];
  center[0] = lx;  center[1] = ly;  center[2] = lz;

  // create a camera box with uncertainty
  vgl_box_3d<double> cam_box(center, 2*uncertainty, 2*uncertainty, 2*uncertainty, vgl_box_3d<double>::centre);
  vcl_vector<vgl_point_3d<double> > cam_corners = corner_of_box_3d(cam_box);

  // create the 3D box given input coordinates (in geo-coordinates)
  vgl_box_3d<double> bbox(lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev);
  vcl_vector<vgl_point_3d<double> > box_corners = corner_of_box_3d(bbox);

  // projection
  vgl_box_2d<double> roi_box_2d;
  double lon, lat, gz;
  for (unsigned i = 0; i < cam_corners.size(); i++)
  {
    lvcs->local_to_global(cam_corners[i].x(), cam_corners[i].y(), cam_corners[i].z(), vpgl_lvcs::wgs84,
                          lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    vpgl_rational_camera<double>* new_cam = rat_cam->clone();
    new_cam->set_offset(vpgl_rational_camera<double>::X_INDX, lon);
    new_cam->set_offset(vpgl_rational_camera<double>::Y_INDX, lat);
    new_cam->set_offset(vpgl_rational_camera<double>::Z_INDX, gz);

    // project the box to image coords
    for (unsigned j = 0; j < box_corners.size(); j++) {
      vgl_point_2d<double> p2d = new_cam->project(vgl_point_3d<double>(box_corners[j].x(), box_corners[j].y(), box_corners[j].z()));
      roi_box_2d.add(p2d);
    }
  }
  vcl_cout << pro.name() << ": projected 2d roi box: " << roi_box_2d << " given uncertainty " << uncertainty << " meters." << vcl_endl;

  // crop the image
  brip_roi broi(img_res_sptr->ni(), img_res_sptr->nj());
  vsol_box_2d_sptr bb = new vsol_box_2d();
  bb->add_point(roi_box_2d.min_x(), roi_box_2d.min_y());
  bb->add_point(roi_box_2d.max_x(), roi_box_2d.max_y());
  bb = broi.clip_to_image_bounds(bb);

  unsigned i0 = (unsigned)bb->get_min_x();
  unsigned j0 = (unsigned)bb->get_min_y();
  unsigned ni = (unsigned)bb->width();
  unsigned nj = (unsigned)bb->height();

  if (ni <= 0 || nj <= 0)
  {
    vcl_cout << pro.name() << ": clipping box is out of image boundary, empty crop image returned" << vcl_endl;
    return false;
  }
  if (i0 > img_res_sptr->ni() || j0 > img_res_sptr->nj())
  {
    vcl_cout << pro.name() << ": clipping box is out of image boundary, empty crop image returned" << vcl_endl;
    return false;
  }

  if ( (i0+ni) > img_res_sptr->ni() && (j0+nj) > img_res_sptr->nj())
  {
    vcl_cout << pro.name() << ": clipping box is out of image boundary, empty crop image returned" << vcl_endl;
    return false;
  }

  // create the local camera
  vpgl_local_rational_camera<double> local_camera(*lvcs, *rat_cam);
  // calculate local camera offset from image bounding box
  double global_u, global_v, local_u, local_v;
  rat_cam->image_offset(global_u, global_v);
  local_u = global_u - bb->get_min_x();
  local_v = global_v - bb->get_min_y();
  local_camera.set_image_offset(local_u, local_v);

  // store output
  unsigned out_j = 0;
  pro.set_output_val<vpgl_camera_double_sptr>(out_j++, new vpgl_local_rational_camera<double>(local_camera));
  pro.set_output_val<unsigned>(out_j++, i0);
  pro.set_output_val<unsigned>(out_j++, j0);
  pro.set_output_val<unsigned>(out_j++, ni);
  pro.set_output_val<unsigned>(out_j++, nj);

  return true;
}

vcl_vector<vgl_point_3d<double> > vpgl_crop_img_using_3d_box_process_globals::corner_of_box_3d(vgl_box_3d<double> box)
{
  
  vcl_vector<vgl_point_3d<double> > corners;
  corners.push_back(box.min_point());
  corners.push_back(vgl_point_3d<double>(box.min_x()+box.width(), box.min_y(),              box.min_z()));
  corners.push_back(vgl_point_3d<double>(box.min_x()+box.width(), box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<double>(box.min_x(),             box.min_y()+box.height(), box.min_z()));
  corners.push_back(vgl_point_3d<double>(box.min_x(),             box.min_y(),              box.max_z()));
  corners.push_back(vgl_point_3d<double>(box.min_x()+box.width(), box.min_y(),              box.max_z()));
  corners.push_back(vgl_point_3d<double>(box.max_point()));
  corners.push_back(vgl_point_3d<double>(box.min_x(),             box.min_y()+box.height(), box.max_z()));
  return corners;
}