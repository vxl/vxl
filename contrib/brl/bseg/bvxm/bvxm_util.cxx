#include "bvxm_util.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vpgl/vpgl_camera.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vil/vil_resample_bilin.h>
#include <vpgl/vpgl_local_rational_camera.h>

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


vil_image_view_base_sptr bvxm_util::downsample_image_by_two(vil_image_view_base_sptr image)
{
  vil_image_view<float>*img_view_float = new vil_image_view<float>(image->ni(),image->nj());

  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<unsigned char> *img_view_char = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr());
    vil_convert_cast<unsigned char,float>(*img_view_char,*img_view_float);
  }
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    img_view_float = dynamic_cast<vil_image_view<float>*>(image.ptr());
  }

  vil_image_view<float> output((int)vcl_floor((float)img_view_float->ni()/2),(int)vcl_floor((float)img_view_float->nj()/2));

#if 0
  vil_image_view<unsigned char>* img_view_out
    = bvxm_multiscale_util::half_resolution(*img_view_float,0.359375f);

  float min_b,max_b;
  vil_math_value_range(*img_view_float,min_b,max_b);
#endif // 0

  vil_resample_bilin<float,float>(*img_view_float,output,output.ni(),output.nj());
  vil_image_view<unsigned char>* img_view_out = new vil_image_view<unsigned char>(output.ni(),output.nj());
  img_view_out->fill(0);

  for (unsigned i=0;i<output.ni();i++)
    for (unsigned j=0;j<output.nj();j++)
    {
      if (output(i,j)>255.0)
        (*img_view_out)(i,j)=255;
      else if (output(i,j)<0.0)
        (*img_view_out)(i,j)=0;
      else
        (*img_view_out)(i,j)=static_cast<vxl_byte>(output(i,j)+0.5);

        //vcl_cout<<(*img_view_out)(i,j);
    }
  ////vil_convert_stretch_range_limited<float>(output,*img_view_out,min_b,max_b);
  vil_image_view_base_sptr return_img=img_view_out;
  return return_img;
}


vpgl_camera_double_sptr bvxm_util::downsample_camera(vpgl_camera_double_sptr camera,unsigned int scale)
{
  if ( vpgl_local_rational_camera<double>* rat_camera = dynamic_cast<vpgl_local_rational_camera<double>*> (camera.as_pointer()))
  {
    vpgl_local_rational_camera<double>* new_rat_camera=new vpgl_local_rational_camera<double>(*rat_camera);
    double u_s,v_s;
    float factor=  vcl_pow(2.0f,-((float)scale));
    rat_camera->image_scale(u_s,v_s);
    new_rat_camera->set_image_scale(u_s*factor,v_s*factor);

    double u_off,v_off;
    rat_camera->image_offset(u_off,v_off);
    new_rat_camera->set_image_offset(u_off*factor,v_off*factor);

    return new_rat_camera;
  }
  else
  {
    return NULL;
  }
}

vpgl_camera_double_sptr bvxm_util::downsample_persp_camera(vpgl_camera_double_sptr camera,unsigned int scale)
{
  if ( vpgl_perspective_camera<double>* persp_camera =    dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer()))
  {
    vpgl_perspective_camera<double>* new_persp_camera=new vpgl_perspective_camera<double>(*persp_camera);
    vnl_matrix_fixed<double,3,4> camera_matrix = persp_camera->get_matrix();
    vnl_matrix_fixed<double,3,3> scale_matrix;
    scale_matrix.fill(0.0);
    float factor=  vcl_pow(2.0f,-((float)scale));

    scale_matrix.put(0,0,1*factor);
    scale_matrix.put(1,1,1*factor);
    scale_matrix.put(2,2,1.0);

    vnl_matrix_fixed<double,3,4> new_camera_matrix = scale_matrix*camera_matrix;

    new_persp_camera->set_matrix(new_camera_matrix);
    return new_persp_camera;
  }
  else
  {
    return NULL;
  }
}

