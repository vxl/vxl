// This is brl/bseg/boxm/algo/pro/processes/boxm_line_backproject_process.cxx
#include <iostream>
#include <cstdio>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for computing the backprojection of lines.
//         Lines are represented as an image with 3 plane images (plane 0=a, plane1=b,
//         plane2=c, line is defined as ax+by=c). The backprojection, which is a plane
//         is stored as an other image with 4 planes (ax+by+cz=d) as the output.
//
// \author Gamze Tunali
// \date   Feb 4, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_ray.h>

#include <vgl/vgl_plane_3d.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: globals
namespace boxm_line_backproject_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}

//: set input and output types
// process takes 2 inputs and has 1 output:
// input[0]: The edge image (vil_image_view<float> with three planes for (a,b,c))
// input[1]: camera
// output[0]: The plane image
bool boxm_line_backproject_process_cons(bprb_func_process& pro)
{
  using namespace boxm_line_backproject_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";

  std::vector<std::string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

vpgl_rational_camera<double>
perspective_to_rational(vpgl_perspective_camera<double>&cam_pers)
{
  vnl_matrix_fixed<double,3,4> cam_pers_matrix = cam_pers.get_matrix();
  std::vector<double> neu_u,den_u,neu_v,den_v;
  double x_scale = 1.0, x_off = 0.0, y_scale = 1.0, y_off = 0.0, z_scale = 1.0, z_off = 0.0, u_scale = 1.0, u_off = 0.0, v_scale = 1.0, v_off = 0.0;

  for (int i=0; i<20; i++) {
    neu_u.push_back(0.0);
    neu_v.push_back(0.0);
    den_u.push_back(0.0);
    den_v.push_back(0.0);
  }

  int vector_map[] = {9,15,18,19};

  for (int i=0; i<4; i++) {
    neu_u[vector_map[i]] = cam_pers_matrix(0,i);
    neu_v[vector_map[i]] = cam_pers_matrix(1,i);
    den_u[vector_map[i]] = cam_pers_matrix(2,i);
    den_v[vector_map[i]] = cam_pers_matrix(2,i);
  }

  vpgl_rational_camera<double> cam_rat(neu_u,den_u,neu_v,den_v,
                                       x_scale,x_off,y_scale,y_off,z_scale,z_off,
                                       u_scale,u_off,v_scale,v_off);
  return cam_rat;
}

//: optimizes rpc camera parameters based on edges
bool boxm_line_backproject_process(bprb_func_process& pro)
{
  using namespace boxm_line_backproject_process_globals;

  // check number of inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;

  // image
  vil_image_view_base_sptr edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  // camera
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);

  // make sure that image has 3 planes
  if (edge_image_sptr->nplanes() != 3) {
    std::cerr << "boxm_line_backproject_process: The edge image is expected to have 3 planes!\n";
    return false;
  }

  vil_image_view<float> *plane_image;
  if (edge_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> edge_image(edge_image_sptr);
    unsigned ni=edge_image.ni();
    unsigned nj=edge_image.nj();

    // the output image
    plane_image = new vil_image_view<float>(ni,nj,4);
    float col, row, theta;

    if (camera->type_name() == "vpgl_proj_camera") {
      auto* cam = dynamic_cast<vpgl_proj_camera<double>*>(camera.ptr());

      for (unsigned i=0; i<ni; i++) {
        for (unsigned j=0; j<nj; j++) {
          col =edge_image(i,j,0); // sub-pixel column
          row =edge_image(i,j,1); // sub_pixel row
          theta=edge_image(i,j,2); // orientation in radians

          if (col<0 || row<0) // no edge is present, check sdet_pro/sdet_detect_edge_tangent_process.h
            (*plane_image)(i,j) = 0.0f;
          else {
            // get two points on the line
            vgl_point_2d<double> p1(col,row);

            float x = col + 0.5f*std::cos(theta);
            float y = row + 0.5f*std::sin(theta);
            vgl_point_2d<double> p2(x,y);

            vgl_line_2d<double> line(p1,p2);

            //create a line and backproject it
            vgl_homg_line_2d<double> image_line(line);
            vgl_homg_plane_3d<double> plane = cam->backproject(image_line);
            if (plane.a()==0 && plane.b()==0 && plane.c()==0 && plane.d()==0)
            std::cout << "ZERO a,b,c,d" << std::endl;
            (*plane_image)(i,j,0)=float(plane.a());
            (*plane_image)(i,j,1)=float(plane.b());
            (*plane_image)(i,j,2)=float(plane.c());
            (*plane_image)(i,j,3)=float(plane.d());
          }
        }
      }
    }
    else if (camera->type_name() == "vpgl_local_rational_camera") {
      auto* cam = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr());
      for (unsigned i=0; i<ni; i++) {
        for (unsigned j=0; j<nj; j++) {
          col =edge_image(i,j,0); // sub-pixel column
          row =edge_image(i,j,1); // sub_pixel row
          theta=edge_image(i,j,2); // orientation in radians

          if (col<0 || row<0) { // no edge is present, check sdet_pro/sdet_detect_edge_tangent_process.h
            (*plane_image)(i,j,0) = 0.0f;
            (*plane_image)(i,j,1) = 0.0f;
            (*plane_image)(i,j,2) = 0.0f;
            (*plane_image)(i,j,3) = 0.0f;
          }
          else {
            // get two point on the line
            vgl_point_2d<double> p1(col, row);

            float x = col + 0.5f*std::cos(theta);
            float y = row + 0.5f*std::sin(theta);
            vgl_point_2d<double> p2(x,y);

            //backproject it
            vgl_plane_3d<double> plane;
            if (vpgl_ray::plane_ray(*cam, p1, p2, plane)) {
              (*plane_image)(i,j,0)=float(plane.a());
              (*plane_image)(i,j,1)=float(plane.b());
              (*plane_image)(i,j,2)=float(plane.c());
              (*plane_image)(i,j,3)=float(plane.d());
            }
            else { // the backprojection was unsuccessful
              std::cout << i << ',' << j << "NO PLANE!!!!!" << std::endl;
              (*plane_image)(i,j,0) = 0.0f;
              (*plane_image)(i,j,1) = 0.0f;
              (*plane_image)(i,j,2) = 0.0f;
              (*plane_image)(i,j,3) = 0.0f;
            }
          }
        }
      }
    }
    else if (camera->type_name() == "vpgl_perspective_camera") {
      auto* cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.ptr());
      auto* proj_cam = static_cast<vpgl_proj_camera<double>*>(cam);
      for (unsigned i=0; i<ni; i++) {
        for (unsigned j=0; j<nj; j++) {
          col =edge_image(i,j,0); // sub-pixel column
          row =edge_image(i,j,1); // sub_pixel row
          theta=edge_image(i,j,2); // orientation in radians

          if (col<0 || row<0) { // no edge is present, check sdet_pro/sdet_detect_edge_tangent_process.h
            (*plane_image)(i,j,0) = 0.0f;
            (*plane_image)(i,j,1) = 0.0f;
            (*plane_image)(i,j,2) = 0.0f;
            (*plane_image)(i,j,3) = 0.0f;
          }
          else {
            // get two point on the line
            vgl_point_2d<double> p1(col, row);

            float x = col + 0.5f*std::cos(theta);
            float y = row + 0.5f*std::sin(theta);
            vgl_point_2d<double> p2(x,y);

            vgl_line_2d<double> line(p1,p2);

            //create a line and backproject it
            vgl_homg_line_2d<double> image_line(line);

            vgl_homg_plane_3d<double> plane = proj_cam->backproject(image_line);
            if (plane.a()==0 && plane.b()==0 && plane.c()==0 && plane.d()==0)
            std::cout << "ZERO a,b,c,d" << std::endl;
            (*plane_image)(i,j,0)=float(plane.a());
            (*plane_image)(i,j,1)=float(plane.b());
            (*plane_image)(i,j,2)=float(plane.c());
            (*plane_image)(i,j,3)=float(plane.d());
          }
        }
      }
    }
    else {
      std::cerr << "boxm_line_backproject_process: The camera type [" << camera->type_name() << "]is not defined yet!\n";
      return false;
    }
  }
  else {
    std::cerr << "boxm_line_backproject_process: This pixel format is not supported yet!\n";
    return false;
  }
  // output
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, plane_image);

  return true;
}
