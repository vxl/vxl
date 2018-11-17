#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include "bvxm_util.h"
#include "bvxm_world_params.h"

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vtol/vtol_edge_2d.h>
#include <sdet/sdet_detector.h>
#include <vnl/algo/vnl_gaussian_kernel_1d.h>
#include <vnl/algo/vnl_chi_squared.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_math.h>
#include <bil/algo/bil_edt.h>

#include "grid/bvxm_voxel_slab.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool bvxm_util::read_cameras(const std::string& filename, std::vector<vnl_double_3x3> &Ks, std::vector<vnl_double_3x3> &Rs, std::vector<vnl_double_3x1> &Ts)
{
  std::ifstream file_inp(filename.c_str());
  if (!file_inp.good()) {
    std::cerr << "error opening file "<< filename <<'\n';
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


bool bvxm_util::write_cameras(const std::string& filename, std::vector<vnl_double_3x3> &Ks, std::vector<vnl_double_3x3> &Rs, std::vector<vnl_double_3x1> &Ts)
{
  std::ofstream file_out(filename.c_str());
  if (!file_out.good()) {
    std::cerr << "error opening file "<< filename <<'\n';
    return false;
  }
  unsigned ncameras = Ks.size();

  file_out << ncameras << std::endl << std::endl;
  for (unsigned i=0; i < ncameras; i++) {
    file_out << Ks[i] << '\n'
             << Rs[i] << '\n'
             << Ts[i] << '\n'
             << std::endl;
  }
  file_out.close();

  return true;
}


void bvxm_util::bilinear_weights(const vgl_h_matrix_2d<double>& invH, unsigned nx_out, unsigned ny_out, vnl_matrix<unsigned> &xvals, vnl_matrix<unsigned> &yvals, vnl_matrix<float> &weights)
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
      pix_out_homg.set_column(col_idx,vnl_vector_fixed<float,3>((float)x,(float)y,1.0f).as_ref());
    }
  }
  vnl_matrix<float> pix_in_homg = H*pix_out_homg;
  xvals.set_size(4,npix_out);
  yvals.set_size(4,npix_out);
  weights.set_size(4,npix_out);

  for (unsigned n=0; n<npix_out; ++n) {
    // normalize homogeneous coordinate
    float pix_in_x = pix_in_homg[0][n] / pix_in_homg[2][n];
    float pix_in_y = pix_in_homg[1][n] / pix_in_homg[2][n];
    // calculate weights and pixel values
    auto x0 = (unsigned)std::floor(pix_in_x);
    auto x1 = (unsigned)std::ceil(pix_in_x);
    float x0_weight = (float)x1 - pix_in_x;
    float x1_weight = 1.0f - (float)x0_weight;
    auto y0 = (unsigned)std::floor(pix_in_y);
    auto y1 = (unsigned)std::ceil(pix_in_y);
    float y0_weight = (float)y1 - pix_in_y;
    float y1_weight = 1.0f - (float)y0_weight;
    xvals.set_column(n,vnl_vector_fixed<unsigned,4>(x0,x0,x1,x1).as_ref());
    yvals.set_column(n,vnl_vector_fixed<unsigned,4>(y0,y1,y0,y1).as_ref());
    weights.set_column(n,vnl_vector_fixed<float,4>(x0_weight*y0_weight,
                                                   x0_weight*y1_weight,
                                                   x1_weight*y0_weight,
                                                   x1_weight*y1_weight).as_ref());
  }
}


vil_image_view_base_sptr bvxm_util::downsample_image_by_two(const vil_image_view_base_sptr& image)
{
  auto*img_view_float = new vil_image_view<float>(image->ni(),image->nj(),image->nplanes());

  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    auto *img_view_char = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr());
    vil_convert_cast<unsigned char,float>(*img_view_char,*img_view_float);
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    img_view_float = dynamic_cast<vil_image_view<float>*>(image.ptr());
  }

  vil_image_view<float> output((int)std::floor((float)img_view_float->ni()/2),
                               (int)std::floor((float)img_view_float->nj()/2),
                               img_view_float->nplanes());

#if 0
  vil_image_view<unsigned char>* img_view_out
    = bvxm_multiscale_util::half_resolution(*img_view_float,0.359375f);

  float min_b,max_b;
  vil_math_value_range(*img_view_float,min_b,max_b);
#endif // 0

  vil_resample_bilin<float,float>(*img_view_float,output,output.ni(),output.nj());
  auto* img_view_out = new vil_image_view<unsigned char>(output.ni(),output.nj(),output.nplanes());
  img_view_out->fill(0);

  for (unsigned p=0; p<output.nplanes(); p++)
    for (unsigned i=0;i<output.ni();i++)
      for (unsigned j=0;j<output.nj();j++)
      {
        if (output(i,j,p)>255.0)
          (*img_view_out)(i,j,p)=255;
        else if (output(i,j,p)<0.0)
          (*img_view_out)(i,j,p)=0;
        else
          (*img_view_out)(i,j,p)=static_cast<vxl_byte>(output(i,j)+0.5);

        //std::cout<<(*img_view_out)(i,j);
      }
  ////vil_convert_stretch_range_limited<float>(output,*img_view_out,min_b,max_b);
  vil_image_view_base_sptr return_img=img_view_out;
  return return_img;
}


vpgl_camera_double_sptr bvxm_util::downsample_camera(const vpgl_camera_double_sptr& camera,unsigned int scale)
{
  if ( auto* rat_camera = dynamic_cast<vpgl_local_rational_camera<double>*> (camera.as_pointer()))
  {
    auto* new_rat_camera=new vpgl_local_rational_camera<double>(*rat_camera);
    double u_s,v_s;
    float factor=  1.0f / float(1 << scale);
    rat_camera->image_scale(u_s,v_s);
    new_rat_camera->set_image_scale(u_s*factor,v_s*factor);

    double u_off,v_off;
    rat_camera->image_offset(u_off,v_off);
    new_rat_camera->set_image_offset(u_off*factor,v_off*factor);

    return new_rat_camera;
  }
  else
  {
    return nullptr;
  }
}

vpgl_camera_double_sptr bvxm_util::downsample_persp_camera(const vpgl_camera_double_sptr& camera,unsigned int scale)
{
  if ( auto* persp_camera =    dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer()))
  {
    auto* new_persp_camera=new vpgl_perspective_camera<double>(*persp_camera);
    vnl_matrix_fixed<double,3,4> camera_matrix = persp_camera->get_matrix();
    vnl_matrix_fixed<double,3,3> scale_matrix;
    scale_matrix.fill(0.0);
    float factor=  1.0f / float(1 << scale);

    scale_matrix.put(0,0,1*factor);
    scale_matrix.put(1,1,1*factor);
    scale_matrix.put(2,2,1.0);

    vnl_matrix_fixed<double,3,4> new_camera_matrix = scale_matrix*camera_matrix;

    new_persp_camera->set_matrix(new_camera_matrix);
    return new_persp_camera;
  }
  else
  {
    return nullptr;
  }
}

void bvxm_util::logical_and(bvxm_voxel_slab<bool> const& s1, bvxm_voxel_slab<bool> const& s2, bvxm_voxel_slab<bool> &result)
{
  // check sizes
  if ( (result.nx() != s1.nx()) || (result.nx() != s2.nx()) ||
       (result.ny() != s1.ny()) || (result.ny() != s2.ny()) ||
       (result.nz() != s1.nz()) || (result.nz() != s2.nz()) ) {
    std::cerr << "error: sizes of slabs to AND do not match.\n";
    return;
  }

  bvxm_voxel_slab<bool>::const_iterator s1_it = s1.begin(), s2_it = s2.begin();
  bvxm_voxel_slab<bool>::iterator result_it = result.begin();
  for (; result_it != result.end(); ++s1_it, ++s2_it, ++result_it) {
    *result_it = *s1_it && *s2_it;
  }
  return;
}

int bvxm_util::convert_uncertainty_from_meters_to_pixels(float uncertainty, const vpgl_lvcs_sptr& lvcs, const vpgl_camera_double_sptr& camera)
{
  // estimate the offset search size in the image space
  vgl_box_3d<double> box_uncertainty(-uncertainty,-uncertainty,-uncertainty,uncertainty,uncertainty,uncertainty);
  std::vector<vgl_point_3d<double> > box_uncertainty_corners = bvxm_util::corners_of_box_3d<double>(box_uncertainty);
  auto* roi_uncertainty = new vgl_box_2d<double>();

  for (auto curr_corner : box_uncertainty_corners) {
    vgl_point_3d<double> curr_pt;
    if (camera->type_name()=="vpgl_local_rational_camera") {
      curr_pt.set(curr_corner.x(),curr_corner.y(),curr_corner.z());
    }
    else if (camera->type_name()=="vpgl_rational_camera") {
      double lon, lat, gz;
      lvcs->local_to_global(curr_corner.x(), curr_corner.y(), curr_corner.z(),
                            vpgl_lvcs::wgs84, lon, lat, gz, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      curr_pt.set(lon, lat, gz);
    }
    else // dummy initialisation, to avoid compiler warning
      curr_pt.set(-1e99, -1e99, -1.0);

    double curr_u,curr_v;
    camera->project(curr_pt.x(),curr_pt.y(),curr_pt.z(),curr_u,curr_v);
    vgl_point_2d<double> p2d_uncertainty(curr_u,curr_v);
    roi_uncertainty->add(p2d_uncertainty);
  }

  return vnl_math::ceil(0.5*std::max(roi_uncertainty->width(),roi_uncertainty->height()));
}
