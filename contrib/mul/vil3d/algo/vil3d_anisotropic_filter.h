// This is mul/vil3d/algo/vil3d_anisotropic_filter.h

#ifndef vil3d_anisotropic_filter_h_
#define vil3d_anisotropic_filter_h_

#include <vil3d/vil3d_image_view.h>
#include <vgl/vgl_vector_3d.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil3d/algo/vil3d_convolve_1d.h>
#include <vnl/vnl_math.h>
#include <vil3d/vil3d_switch_axes.h>
#include <vil3d/vil3d_convert.h>


//:
// \file
// \brief Functions to apply anistropic filters to 3D images.
//
// \author Kevin de Souza and Ian Scott


//: Generates and applies an anisotropic Gaussian filter to 
// an anisotropic 3D image.
// \param src The source image.
// \retval dest The destination (filtered) image.
// \param vox The voxel size in each of the 3 dimensions. This is used to 
// define 1D Gaussian filters suitable for each dimension.
// \param sd The width of the Gaussian (in world coordinates) 
// to be used for each dimension.
// \note The filtering is done in floating-point arithmetic, 
// and the destination image is rounded to the same pixel type as the source.
// \note You may wish to apply isotropic smoothing of width s to 
// an anisotropic image of voxel size vox=(dx, dy, dz). 
// In this case, simply supply sd=(s,s,s).
template <class T>
inline 
void anisotropic_gaussian_filter(const vil3d_image_view<T>& src,
                                 vil3d_image_view<T>& dest,
                                 const vgl_vector_3d<double>& vox,
                                 const vgl_vector_3d<double>& sd)
{
  // Get the image dimensions
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  unsigned nk = src.nk();  
  
  
  // Create some workspace images
  vil3d_image_view<float> work1(ni, nj, nk);
  vil3d_image_view<float> work2(ni, nj, nk);
  vil3d_image_view<float> work3(ni, nj, nk);


  // Size of filter (number of "taps")- filter should be ~ 7*sd wide
  vgl_vector_3d<unsigned> nt(vnl_math_rnd(7.0*sd.x()/vox.x()), 
                             vnl_math_rnd(7.0*sd.y()/vox.y()), 
                             vnl_math_rnd(7.0*sd.z()/vox.z()) );
  
  
  // Centre (absolute) tap of filter (or just past centre if even length)
  vgl_vector_3d<unsigned> c(nt/2);
  
  // Lowest (relative) tap of filter
  vgl_vector_3d<int> lo(-c.x(), -c.y(), -c.z() );      
  
  // Highest (relative) tap of filter (depends whether filter length is odd)
  vgl_vector_3d<int> hi((nt.x()%2)==0 ? c.x()-1 : c.x(),
                        (nt.y()%2)==0 ? c.y()-1 : c.y(),
                        (nt.z()%2)==0 ? c.z()-1 : c.z());
  
  // Create a suitable 1D Gaussian filter in each dimension
  vcl_vector<double> filter_x(nt.x());
  vcl_vector<double> filter_y(nt.y());
  vcl_vector<double> filter_z(nt.z());
  vil_gauss_filter_gen_ntap(sd.x()/vox.x(), 0, filter_x);
  vil_gauss_filter_gen_ntap(sd.y()/vox.y(), 0, filter_y);
  vil_gauss_filter_gen_ntap(sd.z()/vox.z(), 0, filter_z);
  
  
  // Specify the convolution boundary option
  vil_convolve_boundary_option cbo = vil_convolve_trim;

  
  // Perform the convolution of each dimension in succession
  vil3d_convolve_1d(                      src,  work1, 
    &filter_x[c.x()], lo.x(), hi.x(), float(), cbo, cbo);
  
  vil3d_image_view<float> work2_jki = vil3d_switch_axes_jki(work2);  
  vil3d_convolve_1d(vil3d_switch_axes_jki(work1), work2_jki,
    &filter_y[c.y()], lo.y(), hi.y(), float(), cbo, cbo);
  
  vil3d_image_view<float> work3_kij = vil3d_switch_axes_kij(work3);  
  vil3d_convolve_1d(vil3d_switch_axes_kij(work2), work3_kij, 
    &filter_z[c.z()], lo.z(), hi.z(), float(), cbo, cbo);
  
  
  // Convert the results to destination type
  vil3d_convert_round(work3, dest);

  
  ////////////////////////////////////////////////////////////////////////
  //
  // Testing
  //
  vcl_cout << "Source image: \n";
  vil3d_print_all(vcl_cout, src);

  vcl_cout << "\n\nWork1 image: \n";
  vil3d_print_all(vcl_cout, work1);
  double sum1 = 0;
  vil3d_math_sum(sum1, work1, 0);
  vcl_cout << "\nSum1= " << sum1 << vcl_endl;

  vcl_cout << "\n\nWork2 image: \n";
  vil3d_print_all(vcl_cout, work2);
  double sum2 = 0;
  vil3d_math_sum(sum2, work2, 0);
  vcl_cout << "\nSum2= " << sum2 << vcl_endl;
   
  vcl_cout << "\n\nWork3 image: \n";
  vil3d_print_all(vcl_cout, work3);
  double sum3 = 0;
  vil3d_math_sum(sum3, work3, 0);
  vcl_cout << "\nSum3= " << sum3 << vcl_endl;

  vcl_cout << "\n\nDest image: \n";
  vil3d_print_all(vcl_cout, dest);
  double sumd = 0;
  vil3d_math_sum(sumd, dest, 0);
  vcl_cout << "\nSumd= " << sumd << vcl_endl;
  //
  //////////////////////////////////////////////////////////////////////////  
  
}




#endif // vil3d_anisotropic_filter_h_
