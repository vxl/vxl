#include "bvxm_util.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vpgl/vpgl_camera.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>

#include "bvxm_voxel_slab.h"
#include "bvxm_world_params.h"


bool bvxm_util::read_cameras(const vcl_string filename, vcl_vector<vnl_double_3x3> &Ks, vcl_vector<vnl_double_3x3> &Rs, vcl_vector<vnl_double_3x1> &Ts)
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


bool bvxm_util::write_cameras(const vcl_string filename, vcl_vector<vnl_double_3x3> &Ks, vcl_vector<vnl_double_3x3> &Rs, vcl_vector<vnl_double_3x1> &Ts)
{
  vcl_ofstream file_out(filename.c_str());
  if (!file_out.good()) {
    vcl_cerr << "error opening file "<< filename <<vcl_endl;
    return false;
  }
  unsigned ncameras = Ks.size();

  file_out << ncameras << vcl_endl << vcl_endl;
  for (unsigned i=0; i < ncameras; i++) {

    file_out << Ks[i] << vcl_endl
             << Rs[i] << vcl_endl
             << Ts[i] << vcl_endl
             << vcl_endl;
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

