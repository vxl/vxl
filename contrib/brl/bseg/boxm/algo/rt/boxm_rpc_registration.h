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


#include <iostream>
#include <fstream>
#include <limits>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/vpgl_lvcs.h>

#include <brip/brip_vil_float_ops.h>

#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vil/vil_image_view.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

int convert_uncertainty_from_meters_to_pixels(float uncertainty,
                                              vpgl_lvcs lvcs,
                                              vpgl_camera_double_sptr camera)
{
  // estimate the offset search size in the image space
  vgl_box_3d<double> box_uncertainty(-uncertainty,-uncertainty,-uncertainty,uncertainty,uncertainty,uncertainty);
  std::vector<vgl_point_3d<double> > box_uncertainty_corners = boxm_utils::corners_of_box_3d(box_uncertainty);
  vgl_box_2d<double> roi_uncertainty;

  for (auto curr_corner : box_uncertainty_corners) {
    vgl_point_3d<double> curr_pt;
    if (camera->type_name()=="vpgl_local_rational_camera") {
      curr_pt.set(curr_corner.x(),curr_corner.y(),curr_corner.z());
    }
    else if (camera->type_name()=="vpgl_rational_camera") {
      double lon, lat, gz;
      lvcs.local_to_global(curr_corner.x(), curr_corner.y(), curr_corner.z(),
                           vpgl_lvcs::wgs84, lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      curr_pt.set(lon, lat, gz);
    }
    else // dummy initialisation, to avoid compiler warning
      curr_pt.set(-1e99, -1e99, -1.0);

    double curr_u,curr_v;
    camera->project(curr_pt.x(),curr_pt.y(),curr_pt.z(),curr_u,curr_v);
    vgl_point_2d<double> p2d_uncertainty(curr_u,curr_v);
    roi_uncertainty.add(p2d_uncertainty);
  }

  return vnl_math::ceil(0.5*std::max(roi_uncertainty.width(),roi_uncertainty.height()));
}

template<class T_loc, class T_data>
bool boxm_rpc_registration(boxm_scene_base_sptr scene_base,//<boct_tree<T_loc, T_data > &scene,
                           vil_image_view<vxl_byte> edge_image,
                           vil_image_view<vxl_byte> expected_edge_image,
                           vpgl_camera_double_sptr camera,
                           vpgl_camera_double_sptr camera_out,
                           bool rpc_shift_3d_flag,
                           float uncertainty,
                           float n_normal, // FIXME - unused
                           unsigned num_observations)
{
  double max_prob = std::numeric_limits<double>::min();
  unsigned int best_offset_u = 0, best_offset_v = 0;

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
  std::cout << "Estimating image offsets:" << std::endl;
  for (int u=offset_lower_limit_u; u<=offset_upper_limit_u; u++) {
    std::cout << '.';
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
        best_offset_u = u;
        best_offset_v = v;
      }
    }
  }
  std::cout << std::endl;

  std::cout << "Estimated changes in offsets (u,v)=(" << best_offset_u << ',' << best_offset_v << ')' << std::endl;
  // local variables
  std::ifstream file_inp;
  std::ofstream file_out;
  file_out.clear();
  file_out.open("offsets.txt",std::ofstream::app);
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
    std::cerr << "error: process expects camera to be a vpgl_rational_camera or vpgl_local_rational_camera.\n";
    return false;
  }

  return true;
}


template<class T_loc, class T_data>
bool boxm_rpc_registration(boxm_scene_base_sptr scene_base,//<boct_tree<T_loc, T_data > &scene,
                           vil_image_view<float> edge_image,
                           vil_image_view<float > expected_edge_image,
                           vpgl_camera_double_sptr camera,
                           vpgl_camera_double_sptr camera_out,
                           bool rpc_shift_3d_flag,
                           float uncertainty,
                           unsigned num_observations)
{
  double max_cost = std::numeric_limits<double>::max();
  unsigned int best_offset_u = 0, best_offset_v = 0;
  double subpixel_best_offset_u = 0.0, subpixel_best_offset_v = 0.0;

  typedef boct_tree<T_loc,T_data> tree_type;
  boxm_scene<tree_type> *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());

  int offset_search_size = convert_uncertainty_from_meters_to_pixels(uncertainty, scene->lvcs(), camera);
  int ni = edge_image.ni();
  int nj = edge_image.nj();

  std::cout<<ni<<','<<nj<<','<<expected_edge_image.ni()<<','<<expected_edge_image.nj();
  // this is the two level offset search algorithm
  int offset_lower_limit_u = -offset_search_size;
  int offset_lower_limit_v = -offset_search_size;
  int offset_upper_limit_u =  offset_search_size;
  int offset_upper_limit_v =  offset_search_size;
  std::cout << "Estimating image offsets:" << std::endl;
  for (int u=offset_lower_limit_u; u<=offset_upper_limit_u; u++) {
    std::cout << '.';
    for (int v=offset_lower_limit_v; v<=offset_upper_limit_v; v++) {
      // for each offset pair (u,v)
      double cost = 0.0;
      double norm = 0.0;
      // find the total probability of the edge image given the expected edge image
      for (int m=offset_search_size+1; m<ni-offset_search_size-1; m++) {
        for (int n=offset_search_size+1; n<nj-offset_search_size-1; n++) {
          if (edge_image(m,n,0)>-1 && edge_image(m,n,1)>-1 && edge_image(m,n,2)>-1) {
            if (expected_edge_image(m-u,n-v,0)>0 && expected_edge_image(m-u,n-v,1)>0 && expected_edge_image(m-u,n-v,2)>0) {
            //std::cout<<'.';
            float sintheta1=std::sin(edge_image(m,n,2));
            float costheta1=std::cos(edge_image(m,n,2));

            float sintheta2=std::sin(expected_edge_image(m-u,n-v,2));
            float costheta2=std::cos(expected_edge_image(m-u,n-v,2));
#if 0
            float dx=expected_edge_image(m-u,n-v,0)+u-edge_image(m,n,0);
            float dy=expected_edge_image(m-u,n-v,1)+v-edge_image(m,n,1);

            float dist=std::sqrt(dx*dx+dy*dy);
            if (dist<min_dist)
              min_dist=dist;

            float dist1=std::sqrt((dx*sintheta1*sintheta1+dy*sintheta1*costheta1)*(dx*sintheta1*sintheta1+dy*sintheta1*costheta1)
                                +(dy*costheta1*costheta1+dx*sintheta1*costheta1)*(dy*costheta1*costheta1+dx*sintheta1*costheta1));
            float dist2=std::sqrt((dx*sintheta2*sintheta2+dy*sintheta2*costheta2)*(dx*sintheta2*sintheta2+dy*sintheta2*costheta2)
                                +(dy*costheta2*costheta2+dx*sintheta2*costheta2)*(dy*costheta2*costheta2+dx*sintheta2*costheta2));
            if ((dist1+dist2)/2<min_dist)
              min_dist=(dist1+dist2)/2;
#endif // 0
            float dist=(1-std::fabs(sintheta1*sintheta2+costheta2*costheta1));//+std::sqrt(dx*dx+dy*dy);
            cost += dist;//(dist1+dist2)/2;
          }
          else
          {
            cost+=0.5;
          }
          norm += 1;
        }
      }
    }

    if (norm>0.0)
        cost/=norm;

      //std::cout<<cost<<"u "<<u<<" v"<<v<<'\n';
      // if maximum is found
      if (cost < max_cost) {
        max_cost = cost;
        best_offset_u = u;
        best_offset_v = v;
      }
    }
  }
  std::cout << "Estimated changes in offsets (u,v)=(" << best_offset_u << ',' << best_offset_v << ')' << std::endl;
  double subpixel_offset_limit=0.5;
  max_cost = std::numeric_limits<double>::max();
  for (double u=-subpixel_offset_limit; u<=subpixel_offset_limit; ) {
    std::cout << '.';
    for (double v=-subpixel_offset_limit; v<=subpixel_offset_limit; ) {
      // for each offset pair (u,v)
      double cost = 0.0;
      // find the total probability of the edge image given the expected edge image
      for (int m=offset_search_size; m<ni-offset_search_size; m++) {
        for (int n=offset_search_size; n<nj-offset_search_size; n++) {
          if (edge_image(m,n,0)!=-1 || edge_image(m,n,1)!=-1 || edge_image(m,n,2)!=-1) {
            if (expected_edge_image(m-best_offset_u,n-best_offset_v,0)>0 && expected_edge_image(m-best_offset_u,n-best_offset_v,1)>0 && expected_edge_image(m-best_offset_u,n-best_offset_v,2)>0) {
              double dx=expected_edge_image(m-best_offset_u,n-best_offset_v,0)+best_offset_u+u-edge_image(m,n,0);
              double dy=expected_edge_image(m-best_offset_u,n-best_offset_v,1)+best_offset_v+v-edge_image(m,n,1);
              double dist=std::sqrt(dx*dx+dy*dy);
              cost += dist;
            }
          }
        }
      }
      //std::cout<<cost<<' ';
      if (cost < max_cost) {
          max_cost = cost;
          subpixel_best_offset_u = (double)u;
          subpixel_best_offset_v = (double)v;
      }

      v=v+0.05;
    }
    u+=0.05;
  }
  std::cout << "Estimated changes in offsets (u,v)=(" << subpixel_best_offset_u << ',' << subpixel_best_offset_v << ')' << std::endl;

  // local variables
  std::ifstream file_inp;
  std::ofstream file_out;
  file_out.clear();
  file_out.open("offsets.txt",std::ofstream::app);
  file_out << best_offset_u +subpixel_best_offset_u<< ' ' << best_offset_v+subpixel_best_offset_v << '\n';
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
    std::cerr << "error: process expects camera to be a vpgl_rational_camera or vpgl_local_rational_camera.\n";
    return false;
  }

  return true;
}

#endif
