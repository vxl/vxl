#ifndef boxm_rpc_registration_h_
#define boxm_rpc_registration_h_
//:
// \file
// This part contains the correction rpc camera parameters using the expected
// edge image obtained from the world (scene) and edge map of the current image.
// if the camera parameters are manually corrected by the user, this part should
// be omitted by setting the "rpc_correction_flag" parameter to 0 (false) in the
// boxm_rpc_registration_process.
//
// \author Ibrahim Eden
// \date   Jan, 2009
//
// \verbatim
//  Modifications
//     Feb 1st, 2010 Gamze Tunali
//         This piece of code is moved from bvxm_rpc_registration process to this file
//         as a standalone function to be used in boxm/opt/boxm_rpc_registration_process
// \endverbatim


#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <brip/brip_vil_float_ops.h>

#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_utils.h>
#include <vpgl/bgeo/bgeo_lvcs_sptr.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_limits.h>
#include <vil/vil_save.h>

int convert_uncertainty_from_meters_to_pixels(float uncertainty,
                                              bgeo_lvcs lvcs,
                                              vpgl_camera_double_sptr camera)
{
  // estimate the offset search size in the image space
  vgl_box_3d<double> box_uncertainty(-uncertainty,-uncertainty,-uncertainty,uncertainty,uncertainty,uncertainty);
  vcl_vector<vgl_point_3d<double> > box_uncertainty_corners = boxm_utils::corners_of_box_3d(box_uncertainty);
  vgl_box_2d<double>* roi_uncertainty = new vgl_box_2d<double>();

  for (unsigned i=0; i<box_uncertainty_corners.size(); i++) {
    vgl_point_3d<double> curr_corner = box_uncertainty_corners[i];
    vgl_point_3d<double> curr_pt;
    if (camera->type_name()=="vpgl_local_rational_camera") {
      curr_pt.set(curr_corner.x(),curr_corner.y(),curr_corner.z());
    }
    else if (camera->type_name()=="vpgl_rational_camera") {
      double lon, lat, gz;
      lvcs.local_to_global(curr_corner.x(), curr_corner.y(), curr_corner.z(),
                           bgeo_lvcs::wgs84, lon, lat, gz, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
      curr_pt.set(lon, lat, gz);
    }
    else // dummy initialisation, to avoid compiler warning
      curr_pt.set(-1e99, -1e99, -1.0);

    double curr_u,curr_v;
    camera->project(curr_pt.x(),curr_pt.y(),curr_pt.z(),curr_u,curr_v);
    vgl_point_2d<double> p2d_uncertainty(curr_u,curr_v);
    roi_uncertainty->add(p2d_uncertainty);
  }

  return vnl_math_ceil(0.5*vnl_math_max(roi_uncertainty->width(),roi_uncertainty->height()));
}

template<class T_loc, class T_data>
bool boxm_rpc_registration(boxm_scene_base_sptr scene_base,//<boct_tree<T_loc, T_data > &scene,
                           vil_image_view<vxl_byte> edge_image,
                           vil_image_view<vxl_byte> expected_edge_image,
                           vpgl_camera_double_sptr camera,
                           vpgl_camera_double_sptr camera_out,
                           bool rpc_shift_3d_flag,
                           float uncertainty,
                           float n_normal,
                           unsigned num_observations)
{
  double max_prob = vcl_numeric_limits<double>::min();
  double best_offset_u = 0.0, best_offset_v = 0.0;

  typedef boct_tree<T_loc,T_data> tree_type;
  boxm_scene<tree_type> *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());

  int offset_search_size = convert_uncertainty_from_meters_to_pixels(uncertainty, scene->lvcs(), camera);
  int ni = edge_image.ni();
  int nj = edge_image.nj();

  // this is the two level offset search algorithm
  int offset_lower_limit_u = -offset_search_size;
  int offset_lower_limit_v = -offset_search_size;
  int offset_upper_limit_u =  offset_search_size;
  int offset_upper_limit_v =  offset_search_size;
  vcl_cout << "Estimating image offsets:" << vcl_endl;
  for (int u=offset_lower_limit_u; u<=offset_upper_limit_u; u++) {
    vcl_cout << '.';
    for (int v=offset_lower_limit_v; v<=offset_upper_limit_v; v++) {
      // for each offset pair (u,v)
      double prob = 0.0;
      double norm = 0.0;
      // find the total probability of the edge image given the expected edge image
      for (int m=offset_search_size; m<ni-offset_search_size; m++) {
        for (int n=offset_search_size; n<nj-offset_search_size; n++) {
          if (edge_image(m,n)==255) {
            prob += expected_edge_image(m-u,n-v);
          }
          norm += expected_edge_image(m-u,n-v);
        }
      }

      if (norm>0.0)
          prob/=norm;

      // if maximum is found
      if (prob > max_prob) {
        max_prob = prob;
        best_offset_u = (double)u;
        best_offset_v = (double)v;
      }
    }
  }
  vcl_cout << vcl_endl;

  vcl_cout << "Estimated changes in offsets (u,v)=(" << best_offset_u << ',' << best_offset_v << ')' << vcl_endl;
   //local variables
  vcl_ifstream file_inp;
  vcl_ofstream file_out;
  file_out.clear();
  file_out.open("offsets.txt",vcl_ofstream::app);
  file_out << best_offset_u << ' ' << best_offset_v << '\n';
  file_out.close();

  double nlx=0.f,nly=0.f,nlz=0.f;
  double motion_mult = 0.0;
  if (rpc_shift_3d_flag)
  {
    vpgl_local_rational_camera<double> *cam_input_temp = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr());

    vgl_point_3d<double> origin_3d(0.0,0.0,0.0);
    vgl_point_2d<double> origin_2d = cam_input_temp->project(origin_3d);

    vgl_plane_3d<double> curr_plane(0.0,0.0,1.0,-0.001);
    vgl_point_3d<double> init_point(0.0,0.0,0.001);
    vgl_point_3d<double> world_point;
    vpgl_backproject::bproj_plane(cam_input_temp,origin_2d,curr_plane,init_point,world_point);

    vgl_vector_3d<double> motion_plane_normal = world_point - origin_3d;
    motion_plane_normal = normalize(motion_plane_normal);
    vgl_plane_3d<double> motion_plane(motion_plane_normal,origin_3d);

    init_point.set(origin_3d.x(),origin_3d.y(),origin_3d.z());
    vgl_point_3d<double> moved_origin;
    vgl_point_2d<double> origin_2d_shift(origin_2d.x()+best_offset_u,origin_2d.y()+best_offset_v);
    vpgl_backproject::bproj_plane(cam_input_temp,origin_2d_shift,motion_plane,init_point,moved_origin);

    vgl_vector_3d<double> motion = moved_origin - origin_3d;

    motion_mult = 1.0/((double)(num_observations+1));
    motion = motion*motion_mult;

    double lx,ly,lz;
    vgl_point_3d<double> rpc_origin = scene->rpc_origin();
    lx = rpc_origin.x();
    ly = rpc_origin.y();
    lz = rpc_origin.z();

    lx = lx*(1.0f-(double)motion_mult);
    ly = ly*(1.0f-(double)motion_mult);
    lz = lz*(1.0f-(double)motion_mult);

    vgl_point_3d<double> pt_3d_est_vec(lx,ly,lz);
    vgl_point_3d<double> pt_3d_updated = pt_3d_est_vec + motion;

    nlx = (double)pt_3d_updated.x();
    nly = (double)pt_3d_updated.y();
    nlz = (double)pt_3d_updated.z();

    vgl_point_3d<double> new_rpc_origin(nlx,nly,nlz);
    scene->set_rpc_origin(new_rpc_origin);
  }

  // correct the output for (local) rational camera using the estimated offset pair (max_u,max_v)
  // the following block of code takes care of different input camera types
  //  e.g., vpgl_local_rational_camera and vpgl_rational_camera
  bool is_local_cam = true;
  bool is_rational_cam = true;
  vpgl_local_rational_camera<double> *cam_inp_local;
  if (!(cam_inp_local = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr()))) {
    is_local_cam = false;
  }
  vpgl_rational_camera<double> *cam_inp_rational;
  if (!(cam_inp_rational = dynamic_cast<vpgl_rational_camera<double>*>(camera.ptr()))) {
    is_rational_cam = false;
  }

  if (is_local_cam) {
    vpgl_local_rational_camera<double> cam_out_local(*cam_inp_local);
    double offset_u,offset_v;
    cam_out_local.image_offset(offset_u,offset_v);
    offset_u += (best_offset_u*(1.0-motion_mult));
    offset_v += (best_offset_v*(1.0-motion_mult));
    cam_out_local.set_image_offset(offset_u,offset_v);
    camera_out = new vpgl_local_rational_camera<double>(cam_out_local);
  }
  else if (is_rational_cam) {
    vpgl_rational_camera<double> cam_out_rational(*cam_inp_rational);
    double offset_u,offset_v;
    cam_out_rational.image_offset(offset_u,offset_v);
    offset_u += (best_offset_u*(1.0-motion_mult));
    offset_v += (best_offset_v*(1.0-motion_mult));
    cam_out_rational.set_image_offset(offset_u,offset_v);
    camera_out = new vpgl_rational_camera<double>(cam_out_rational);
  }
  else {
    vcl_cerr << "error: process expects camera to be a vpgl_rational_camera or vpgl_local_rational_camera.\n";
    return false;
  }

  return true;
}

#endif
