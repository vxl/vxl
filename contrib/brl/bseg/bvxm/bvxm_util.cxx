
#include "bvxm_util.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vpgl/vpgl_camera.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>

#include "bvxm_voxel_slab.h"
#include "bvxm_world_params.h"


void bvxm_util::compute_plane_image_H(vpgl_camera_double_sptr const& cam, bvxm_world_params_sptr world_params, unsigned z_idx, vgl_h_matrix_2d<double> &H_plane_to_image, vgl_h_matrix_2d<double> &H_image_to_plane)
{
    float vox_length = world_params->voxel_length();
    vgl_vector_3d<unsigned int> grid_size = world_params->num_voxels();
    vgl_point_3d<float> grid_corner_bottom = world_params->corner();
    // corner in parameters refers to the bottom. we want the top since slice 0 is the top-most slice.
    vgl_point_3d<float> grid_corner = grid_corner_bottom + vgl_vector_3d<float>(0.0f,0.0f,vox_length*(grid_size.z() - 0.5f));


    vcl_vector<vgl_homg_point_2d<double> > voxel_corners_img;
    vcl_vector<vgl_homg_point_2d<double> > voxel_corners_vox;

    vgl_vector_3d<float> x_step(vox_length,0,0);
    vgl_vector_3d<float> y_step(0,vox_length,0);
    vgl_vector_3d<float> z_step(0,0,-vox_length);

    // create vectors containing four corners of grid, and their projections into the image
    double u=0, v=0;
    vgl_point_3d<float> corner_world;

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(0,0));
    corner_world = grid_corner + z_step*z_idx;
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(grid_size.x()-1,0));
    corner_world = grid_corner + z_step*z_idx + x_step*(grid_size.x()-1);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(grid_size.x()-1,grid_size.y()-1));
    corner_world = grid_corner + z_step*z_idx + x_step*(grid_size.x()-1) + y_step*(grid_size.y()-1);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));

    voxel_corners_vox.push_back(vgl_homg_point_2d<double>(0,(grid_size.y()-1)));
    corner_world = grid_corner + z_step*z_idx + y_step*(grid_size.y()-1);
    cam->project(corner_world.x(),corner_world.y(),corner_world.z(),u,v);
    voxel_corners_img.push_back(vgl_homg_point_2d<double>(u,v));


    vgl_h_matrix_2d_compute_linear comp_4pt;
    if (!comp_4pt.compute(voxel_corners_img,voxel_corners_vox, H_image_to_plane)) {
      vcl_cerr << "ERROR computing homography from image to voxel slice. " << vcl_endl;
    }
    if (!comp_4pt.compute(voxel_corners_vox,voxel_corners_img, H_plane_to_image)) {
      vcl_cerr << "ERROR computing homography from voxel slice to image. " << vcl_endl;
    }
    return;
}


bool bvxm_util::read_cameras(const vcl_string filename, std::vector<vnl_double_3x3> &Ks, std::vector<vnl_double_3x3> &Rs, std::vector<vnl_double_3x1> &Ts)
{
  vcl_ifstream file_inp(filename.c_str());
  if (!file_inp.good()) {
    vcl_cerr << "error opening file "<< filename <<vcl_endl;
    return false;
  }
  unsigned ncameras;

  file_inp >> ncameras;
  for (unsigned i=0; i < ncameras; i++) {
    vnl_double_3x3 K,R;
    vnl_double_3x1 T;
    file_inp >> K;
    file_inp >> R;
    file_inp >> T;
    Ks.push_back(K);
    Rs.push_back(R);
    Ts.push_back(T);
  }
  file_inp.close();

  return true;
}


bool bvxm_util::write_cameras(const vcl_string filename, std::vector<vnl_double_3x3> &Ks, std::vector<vnl_double_3x3> &Rs, std::vector<vnl_double_3x1> &Ts)
{
  vcl_ofstream file_out(filename.c_str());
  if (!file_out.good()) {
    vcl_cerr << "error opening file "<< filename <<vcl_endl;
    return false;
  }
  unsigned ncameras = Ks.size();

  file_out << ncameras << vcl_endl << vcl_endl;
  for (unsigned i=0; i < ncameras; i++) {

    file_out << Ks[i] << vcl_endl;
    file_out << Rs[i] << vcl_endl;
    file_out << Ts[i] << vcl_endl;
    file_out << vcl_endl;
  }
  file_out.close();

  return true;
}


void bvxm_util::bilinear_weights(vgl_h_matrix_2d<double> invH, unsigned nx_out, unsigned ny_out, vnl_matrix<unsigned> &xvals, vnl_matrix<unsigned> &yvals, vnl_matrix<float> &weights)
{
  // perform bilinear interpolation.
  vnl_matrix_fixed<float,3,3> H;
  vnl_matrix_fixed<double,3,3> Hd = invH.get_matrix();
  // convert H to a float matrix
  vnl_matrix_fixed<float,3,3>::iterator Hit = H.begin();
  vnl_matrix_fixed<double,3,3>::iterator Hdit = Hd.begin();
  for (; Hit != H.end(); ++Hit, ++Hdit)
      *Hit = (float)(*Hdit);

  unsigned npix_out = nx_out * ny_out;

  vnl_matrix<float> pix_out_homg(3,npix_out);
    unsigned col_idx = 0;
    for (unsigned y=0; y<ny_out; ++y) {
      for (unsigned x=0; x<nx_out; ++x, ++col_idx) {
        pix_out_homg.set_column(col_idx,vnl_vector_fixed<float,3>((float)x,(float)y,1.0f));
      }
    }
    vnl_matrix<float> pix_in_homg = H*pix_out_homg;
    xvals = vnl_matrix<unsigned>(4,npix_out);
    yvals = vnl_matrix<unsigned>(4,npix_out);
    weights = vnl_matrix<float>(4,npix_out);

    for (unsigned n=0; n<npix_out; ++n) {
      // normalize homogeneous coordinate
      float pix_in_x = pix_in_homg[0][n] / pix_in_homg[2][n];
      float pix_in_y = pix_in_homg[1][n] / pix_in_homg[2][n];
      // calculate weights and pixel values
      unsigned x0 = (unsigned)vcl_floor(pix_in_x);
      unsigned x1 = (unsigned)vcl_ceil(pix_in_x);
      float x0_weight = (float)(x1 - pix_in_x);
      float x1_weight = (float)(1.0f - x0_weight);
      unsigned y0 = (unsigned)vcl_floor(pix_in_y);
      unsigned y1 = (unsigned)vcl_ceil(pix_in_y);
      float y0_weight = (float)(y1 - pix_in_y);
      float y1_weight = (float)(1.0f - y0_weight);
      xvals.set_column(n,vnl_vector_fixed<unsigned,4>(x0,x0,x1,x1));
      yvals.set_column(n,vnl_vector_fixed<unsigned,4>(y0,y1,y0,y1));
      weights.set_column(n,vnl_vector_fixed<float,4>(x0_weight*y0_weight,
                                                     x0_weight*y1_weight,
                                                     x1_weight*y0_weight,
                                                     x1_weight*y1_weight));
    }
}

