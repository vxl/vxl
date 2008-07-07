
#include "bvxm_multiscale_util.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>
#include <vil/vil_resample_bilin.h>
#include "bvxm_voxel_slab.h"
#include "bvxm_world_params.h"


void bvxm_multiscale_util::compute_plane_image_H(vpgl_camera_double_sptr const& cam, bvxm_world_params_sptr world_params, unsigned z_idx, vgl_h_matrix_2d<double> &H_plane_to_image, vgl_h_matrix_2d<double> &H_image_to_plane,unsigned scale)
{
    float vox_length = world_params->voxel_length(scale);
    vgl_vector_3d<unsigned int> grid_size = world_params->num_voxels(scale);
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


//bool bvxm_multiscale_util::read_cameras(const vcl_string filename, std::vector<vnl_double_3x3> &Ks, std::vector<vnl_double_3x3> &Rs, std::vector<vnl_double_3x1> &Ts)
//{
//  vcl_ifstream file_inp(filename.c_str());
//  if (!file_inp.good()) {
//    vcl_cerr << "error opening file "<< filename <<vcl_endl;
//    return false;
//  }
//  unsigned ncameras;
//
//  file_inp >> ncameras;
//  for (unsigned i=0; i < ncameras; i++) {
//    vnl_double_3x3 K,R;
//    vnl_double_3x1 T;
//    file_inp >> K;
//    file_inp >> R;
//    file_inp >> T;
//    Ks.push_back(K);
//    Rs.push_back(R);
//    Ts.push_back(T);
//  }
//  file_inp.close();
//
//  return true;
//}
//
//
//bool bvxm_multiscale_util::write_cameras(const vcl_string filename, std::vector<vnl_double_3x3> &Ks, std::vector<vnl_double_3x3> &Rs, std::vector<vnl_double_3x1> &Ts)
//{
//  vcl_ofstream file_out(filename.c_str());
//  if (!file_out.good()) {
//    vcl_cerr << "error opening file "<< filename <<vcl_endl;
//    return false;
//  }
//  unsigned ncameras = Ks.size();
//
//  file_out << ncameras << vcl_endl << vcl_endl;
//  for (unsigned i=0; i < ncameras; i++) {
//
//    file_out << Ks[i] << vcl_endl;
//    file_out << Rs[i] << vcl_endl;
//    file_out << Ts[i] << vcl_endl;
//    file_out << vcl_endl;
//  }
//  file_out.close();
//
//  return true;
//}


void bvxm_multiscale_util::bilinear_weights(vgl_h_matrix_2d<double> invH, unsigned nx_out, unsigned ny_out, vnl_matrix<unsigned> &xvals, vnl_matrix<unsigned> &yvals, vnl_matrix<float> &weights)
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

void bvxm_multiscale_util::smooth_gaussian(vil_image_view_base_sptr image, float stdx, float stdy)
{
    if ( (stdx < 0) || (stdy < 0) ) {
        vcl_cerr << "error: smooth_gaussian called with negative std. deviation!\n"
            << "stdx = " << stdx << "  stdy = " << stdy << vcl_endl;
        return;
    }

    if ( (stdx == 0) && (stdy == 0) )
        return;

    // create 1d kernels
    unsigned kernel_radius_x = (unsigned)(stdx*3);
    unsigned kernel_size_x = 2*kernel_radius_x + 1;
    vnl_vector<float> kernel_1dx(kernel_size_x);

    // fill in kernel
    for (unsigned i=0; i<kernel_size_x; ++i) {
        kernel_1dx[i] = (float)(vnl_math::sqrt1_2 * vnl_math::two_over_sqrtpi * (0.5/stdx) * vcl_exp(-((((float)i-kernel_radius_x)*((float)i-kernel_radius_x))/(2*stdx*stdx))));
    }
    // normalize kernel in case taps dont sum to exactly one
    kernel_1dx = kernel_1dx / kernel_1dx.sum();

    unsigned kernel_radius_y = (unsigned)(stdy*3);
    unsigned kernel_size_y = 2*kernel_radius_y + 1;
    vnl_vector<float> kernel_1dy(kernel_size_y);
    // fill in kernel
    for (unsigned i=0; i<kernel_size_y; ++i) {
        kernel_1dy[i] = (float)(vnl_math::sqrt1_2 * vnl_math::two_over_sqrtpi * (0.5/stdy) * vcl_exp(-((((float)i-kernel_radius_y)*((float)i-kernel_radius_y))/(2*stdy*stdy))));
    }
    // normalize kernel in case taps dont sum to exactly one
    kernel_1dy = kernel_1dy / kernel_1dy.sum();

    //vcl_cout << "kernel co-eff sum: " << kernel_1dy.sum() << vcl_endl;

    if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    {
        vil_image_view<unsigned char> *img_view = dynamic_cast<vil_image_view<unsigned char>*>(image.ptr());

        vil_image_view<unsigned char> image_work(image->ni(),image->nj());
        image_work.deep_copy(*img_view);

        if (stdx > 0) {
            // create temporary slab
            // convolve rows
            for (unsigned y=0; y<image->nj(); ++y) {
                for (unsigned x=0; x<=image->ni() - kernel_size_x; ++x) {
                    double sum = (*img_view)(x,y) * kernel_1dx[0];
                    for (unsigned k=1; k<kernel_size_x; ++k) {
                        sum += (*img_view)(x+k,y) * kernel_1dx[k];
                    }
                    image_work(x+kernel_radius_x,y) = vcl_ceil(sum);
                }
                // left edge
                for (unsigned x=0; x<kernel_radius_x; ++x) {
                    double sum(0.0);
                    for (unsigned k=kernel_radius_x - x; k<kernel_size_x; ++k) {
                        sum += (*img_view)(x+k-kernel_radius_x,y) * kernel_1dx[k];
                    }
                    image_work(x,y) = vcl_ceil(sum);
                }
                // right edge
                for (unsigned x=image->ni() - kernel_radius_x; x<image->ni(); ++x) {
                    double sum(0.0);
                    for (unsigned k=0; k<(image->ni() - x + kernel_radius_x); ++k) {
                        sum += (*img_view)(x+k-kernel_radius_x,y) * kernel_1dx[k];
                    }
                    image_work(x,y) = vcl_ceil(sum);
                }
            }
        } else {
            // stdx was zero, just use original image.
            image_work.deep_copy(*img_view);
        }

        if (stdy > 0.0) {
            // convolve columns
            for (unsigned x=0; x<image->ni(); ++x) {
                for (unsigned y=0; y<=image->nj() - kernel_size_y; ++y) {
                    double sum = image_work(x,y) * kernel_1dy[0];
                    for (unsigned k=1; k<kernel_size_y; ++k) {
                        sum += image_work(x,y+k) * kernel_1dy[k];
                    }
                    (*img_view)(x,y+kernel_radius_y) = vcl_ceil(sum);
                }
                // top edge
                for (unsigned y=0; y<kernel_radius_y; ++y) {
                    double sum(0.0);
                    for (unsigned k=kernel_radius_y - y; k<kernel_size_y; ++k) {
                        sum += image_work(x,y+k-kernel_radius_y) * kernel_1dy[k];
                    }
                    (*img_view)(x,y) = vcl_ceil(sum);
                }
                // bottom edge
                for (unsigned y=image->nj()-kernel_radius_y; y<image->nj(); ++y) {
                    double sum(0.0);
                    for (unsigned k=0; k<(image->nj() - y + kernel_radius_y); ++k) {
                        sum += image_work(x,y+k-kernel_radius_y) * kernel_1dy[k];
                    }
                    (*img_view)(x,y) = vcl_ceil(sum);
                }
            }
        }else {
            (*img_view).deep_copy(image_work);
        }

    }

     else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
        vil_image_view<float> *img_view = dynamic_cast<vil_image_view<float>*>(image.ptr());

        vil_image_view<float> image_work(image->ni(),image->nj());
        image_work.deep_copy(*img_view);

        if (stdx > 0) {
            // create temporary slab
            // convolve rows
            for (unsigned y=0; y<image->nj(); ++y) {
                for (unsigned x=0; x<=image->ni() - kernel_size_x; ++x) {
                    float sum = (*img_view)(x,y) * kernel_1dx[0];
                    for (unsigned k=1; k<kernel_size_x; ++k) {
                        sum += (*img_view)(x+k,y) * kernel_1dx[k];
                    }
                    image_work(x+kernel_radius_x,y) = sum;
                }
                // left edge
                for (unsigned x=0; x<kernel_radius_x; ++x) {
                    float sum(0.0);
                    for (unsigned k=kernel_radius_x - x; k<kernel_size_x; ++k) {
                        sum += (*img_view)(x+k-kernel_radius_x,y) * kernel_1dx[k];
                    }
                    image_work(x,y) = sum;
                }
                // right edge
                for (unsigned x=image->ni() - kernel_radius_x; x<image->ni(); ++x) {
                    float sum(0.0);
                    for (unsigned k=0; k<(image->ni() - x + kernel_radius_x); ++k) {
                        sum += (*img_view)(x+k-kernel_radius_x,y) * kernel_1dx[k];
                    }
                    image_work(x,y) = sum;
                }
            }
        } else {
            // stdx was zero, just use original image.
            image_work.deep_copy(*img_view);
        }

        if (stdy > 0.0) {
            // convolve columns
            for (unsigned x=0; x<image->ni(); ++x) {
                for (unsigned y=0; y<=image->nj() - kernel_size_y; ++y) {
                    float sum = image_work(x,y) * kernel_1dy[0];
                    for (unsigned k=1; k<kernel_size_y; ++k) {
                        sum += image_work(x,y+k) * kernel_1dy[k];
                    }
                    (*img_view)(x,y+kernel_radius_y) = sum;
                }
                // top edge
                for (unsigned y=0; y<kernel_radius_y; ++y) {
                    float sum(0.0);
                    for (unsigned k=kernel_radius_y - y; k<kernel_size_y; ++k) {
                        sum += image_work(x,y+k-kernel_radius_y) * kernel_1dy[k];
                    }
                    (*img_view)(x,y) = sum;
                }
                // bottom edge
                for (unsigned y=image->nj()-kernel_radius_y; y<image->nj(); ++y) {
                    float sum(0.0);
                    for (unsigned k=0; k<(image->nj() - y + kernel_radius_y); ++k) {
                        sum += image_work(x,y+k-kernel_radius_y) * kernel_1dy[k];
                    }
                    (*img_view)(x,y) = sum;
                }
            }
        }else {
            (*img_view).deep_copy(image_work);
        }

    }
     else
      vcl_cerr << "error: failed to cast image_view_base to image_view\n";
    return;
}

vil_image_view_base_sptr bvxm_multiscale_util::downsample_image_by_two(vil_image_view_base_sptr image)
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

    //vil_image_view<unsigned char>* img_view_out;
    /*= bvxm_multiscale_util::half_resolution(*img_view_float,0.359375f);

    float min_b,max_b;
    vil_math_value_range(*img_view_float,min_b,max_b);*/

    vil_resample_bilin<float,float>(*img_view_float,output,output.ni(),output.nj());
    vil_image_view<unsigned char>* img_view_out = new vil_image_view<unsigned char>(output.ni(),output.nj());
    img_view_out->fill(0);
   
       for(unsigned i=0;i<output.ni();i++)
        for(unsigned j=0;j<output.nj();j++)
        {
            if( output(i,j)>255.0)
                (*img_view_out)(i,j)=255;
            else if(output(i,j)<0.0)
                (*img_view_out)(i,j)=0;
            else
                (*img_view_out)(i,j)=static_cast<vxl_byte>(output(i,j)+0.5);

            //vcl_cout<<(*img_view_out)(i,j);
        }
    ////vil_convert_stretch_range_limited<float>(output,*img_view_out,min_b,max_b);
    vil_image_view_base_sptr return_img=img_view_out;
    return return_img;
}

void bvxm_multiscale_util::fill_1d_array(vil_image_view<float> const& input,
                          int y, float* output)
{
  unsigned w = input.ni();
  for (unsigned x = 0; x<w; x++)
    output[x] = input(x,y);
}

     //: Downsamples the 1-d array by 2 using the Burt-Adelson reduction algorithm.
void bvxm_multiscale_util::half_resolution_1d(const float* input, unsigned width,
                                            float k0, float k1, float k2,
                                            float* output)
{
  float w[5];
  int n = 0;
  for (; n<5; n++)
    w[n]=input[n];
  output[0]=k0*w[0]+ 2.0f*(k1*w[1] + k2*w[2]);//reflect at boundary
  for (unsigned x = 1; x<width; x++)
  {
    output[x]=k0*w[2]+ k1*(w[1]+w[3]) + k2*(w[0]+w[4]);
    //shift the window, w, over by two pixels
    w[0] = w[2];       w[1] = w[3];     w[2] = w[4];
    //handle the boundary conditions
    if (x+2<width)
      w[3] = input[n++], w[4] = input[n++];
    else
      w[3] = w[1], w[4] = w[0];
  }
}

//: Downsamples the image by 2 using the Burt-Adelson reduction algorithm.
// Convolution with a 5-point kernel [(0.5-ka)/2, 0.25, ka, 0.25, (0.5-ka)/2]
// ka = 0.6  maximum decorrelation, wavelet for image compression.
// ka = 0.5  linear interpolation,
// ka = 0.4  Gaussian filter
// ka = 0.359375 min aliasing, wider than Gaussian
// The image sizes are related by: output_dimension = (input_dimension +1)/2.
vil_image_view<float>
bvxm_multiscale_util::half_resolution(vil_image_view<float> const& input,
                                    float filter_coef)
{
  float k0 = filter_coef, k1 = 0.25f*filter_coef, k2 = 0.5f*(0.5f-filter_coef);
  unsigned w = input.ni(), h = input.nj();
  int half_w =(w+1)/2, half_h = (h+1)/2;
  vil_image_view<float> output;
  output.set_size(half_w, half_h);
  //Generate input/output arrays
  int n = 0;
  float* in0 = new float[w];  float* in1 = new float[w];
  float* in2 = new float[w];  float* in3 = new float[w];
  float* in4 = new float[w];

  float* out0 = new float[half_w];  float* out1 = new float[half_w];
  float* out2 = new float[half_w];  float* out3 = new float[half_w];
  float* out4 = new float[half_w];
  //Initialize arrays
  fill_1d_array(input, n++, in0);   fill_1d_array(input, n++, in1);
  fill_1d_array(input, n++, in2);   fill_1d_array(input, n++, in3);
  fill_1d_array(input, n++, in4);

  //downsample initial arrays
  half_resolution_1d(in0, half_w, k0, k1, k2, out0);
  half_resolution_1d(in1, half_w, k0, k1, k2, out1);
  half_resolution_1d(in2, half_w, k0, k1, k2, out2);
  half_resolution_1d(in3, half_w, k0, k1, k2, out3);
  half_resolution_1d(in4, half_w, k0, k1, k2, out4);
  int x=0, y;
  //do the first output line
  for (;x<half_w;x++)
    output(x,0)= k0*out0[x]+ 2.0f*(k1*out1[x]+k2*out2[x]);
  //normal lines
  for (y=1; y<half_h; y++)
  {
    for (x=0; x<half_w; x++)
      output(x,y) = k0*out2[x]+ k1*(out1[x]+out3[x]) + k2*(out0[x]+out4[x]);
    //shift the neighborhood down two lines
    float* temp0 = out0;
    float* temp1 = out1;
    out0 = out2;  out1 = out3;  out2 = out4;
    out3 = temp0; out4 = temp1;//reflect values
    //test border condition
    if (y<half_h-2)
    {
      //normal processing, so don't reflect
      fill_1d_array(input, n++, in3);
      fill_1d_array(input, n++, in4);
     half_resolution_1d(in3, half_w, k0, k1, k2, out3);
     half_resolution_1d(in4, half_w, k0, k1, k2, out4);
    }
  }
  delete [] in0;  delete [] in1; delete [] in2;
  delete [] in3;  delete [] in4;
  delete [] out0;  delete [] out1; delete [] out2;
  delete [] out3;  delete [] out4;

  return output;
}

vpgl_camera_double_sptr bvxm_multiscale_util::downsample_camera(vpgl_camera_double_sptr camera,unsigned int scale)
{

    if( vpgl_local_rational_camera<double>* rat_camera =    dynamic_cast<vpgl_local_rational_camera<double>*> (camera.as_pointer()))
    {
        vpgl_local_rational_camera<double>* new_rat_camera=new vpgl_local_rational_camera<double>(*rat_camera);
        double u_s,v_s;
        rat_camera->image_scale(u_s,v_s);
        new_rat_camera->set_image_scale(u_s/vcl_pow(2.0,scale),v_s/vcl_pow(2.0,scale));

        double u_off,v_off;
        rat_camera->image_offset(u_off,v_off);
        new_rat_camera->set_image_offset(u_off/vcl_pow(2.0,scale),v_off/vcl_pow(2.0,scale));

        return new_rat_camera;

    }
    else 
    {
        return NULL;

    }
}

vpgl_camera_double_sptr bvxm_multiscale_util::downsample_persp_camera(vpgl_camera_double_sptr camera,unsigned int scale)
{

    if( vpgl_perspective_camera<double>* persp_camera =    dynamic_cast<vpgl_perspective_camera<double>*> (camera.as_pointer()))
    {
        vpgl_perspective_camera<double>* new_persp_camera=new vpgl_perspective_camera<double>(*persp_camera);
        vnl_matrix_fixed<double,3,4> camera_matrix = persp_camera->get_matrix();
        vnl_matrix_fixed<double,3,3> scale_matrix;
        scale_matrix.fill(0.0);
        scale_matrix.put(0,0,1/vcl_pow(2.0,scale));
        scale_matrix.put(1,1,1/vcl_pow(2.0,scale));
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


