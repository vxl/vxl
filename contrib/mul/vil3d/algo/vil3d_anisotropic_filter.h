// This is mul/vil3d/algo/vil3d_anisotropic_filter.h
#ifndef vil3d_anisotropic_filter_h_
#define vil3d_anisotropic_filter_h_
//:
// \file
// \brief Functions to apply anisotropic filters to 3D images.
//
// \author Kevin de Souza and Ian Scott

#include <vil3d/vil3d_image_view.h>
#include <vgl/vgl_vector_3d.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil3d/algo/vil3d_convolve_1d.h>
#include <vnl/vnl_math.h>
#include <vil3d/vil3d_switch_axes.h>
#include <vil3d/vil3d_convert.h>
#include <vil3d/vil3d_print.h>
#include <vcl_iostream.h>


//========================================================================
//: Function to apply 3 different 1D filters to a 3D image.
// \param src     The source image.
// \retval dest   The destination image (of type <float>).
// \param filter  The filter to apply in each direction.
// \param c       The (absolute) index of the centre tap of each filter.
// \param lo      The (relative) index of the lowest tap of each filter.
// \param hi      The (relative) index of the highest tap of each filter.
// \param cbo     The convolution boundary option to use with each filter.
// \param work1   Workspace image (assumed to be correct size).
// \param work2   Workspace image (assumed to be correct size).
// \note The returned image <float> can be converted to the source type
// by a subsequent call to vil3d_convert_round() or vil3d_convert-cast().
//========================================================================
template <class T>
inline
void vil3d_anisotropic_filter(const vil3d_image_view<T>& src,
                              vil3d_image_view<float>& dest,
                              const vgl_vector_3d<vcl_vector<double> >& filter,
                              const vgl_vector_3d<unsigned>& c,
                              const vgl_vector_3d<int>& lo,
                              const vgl_vector_3d<int>& hi,
                              const vgl_vector_3d<vil_convolve_boundary_option>& cbo,
                              vil3d_image_view<float>& work1,
                              vil3d_image_view<float>& work2)
{
  vil3d_convolve_1d(src,  work1, &(filter.x()[c.x()]),
                    lo.x(), hi.x(), float(), cbo.x(), cbo.x());

  vil3d_image_view<float> work2_jki = vil3d_switch_axes_jki(work2);

  vil3d_convolve_1d(vil3d_switch_axes_jki(work1), work2_jki,
    &(filter.y()[c.y()]), lo.y(), hi.y(), float(), cbo.y(), cbo.y());

  vil3d_image_view<float> dest_kij = vil3d_switch_axes_kij(dest);

  vil3d_convolve_1d(vil3d_switch_axes_kij(work2), dest_kij,
    &(filter.z()[c.z()]), lo.z(), hi.z(), float(), cbo.z(), cbo.z());


  ////////////////////////////////////////////////////////////////////////
  //
  // Testing
  //
#ifdef PERFORM_TESTING
  //
  vcl_cout << "Source image:\n";
  vil3d_print_all(vcl_cout, src);

  vcl_cout << "\n\nWork1 image:\n";
  vil3d_print_all(vcl_cout, work1);
  double sum1 = 0;
  vil3d_math_sum(sum1, work1, 0);
  vcl_cout << "\nSum1= " << sum1 << vcl_endl;

  vcl_cout << "\n\nWork2 image:\n";
  vil3d_print_all(vcl_cout, work2);
  double sum2 = 0;
  vil3d_math_sum(sum2, work2, 0);
  vcl_cout << "\nSum2= " << sum2 << vcl_endl;

  vcl_cout << "\n\nDest image:\n";
  vil3d_print_all(vcl_cout, dest);
  double sumd = 0;
  vil3d_math_sum(sumd, dest, 0);
  vcl_cout << "\nSumd= " << sumd << vcl_endl;
  //
#endif // PERFORM_TESTING
  //
  ////////////////////////////////////////////////////////////////////////
}


//========================================================================
//: Generates 3 Gaussian filters, 1 for each dimension.
// \param sd      The width of the Gaussian (in voxel widths) for each dimension.
// \retval filter The 3 filters.
// \retval c      The (absolute) index of the centre tap of each filter.
// \retval lo     The (relative) index of the lowest tap of each filter.
// \retval hi     The (relative) index of the highest tap of each filter.
//========================================================================
inline
void vil3d_generate_gaussian_filters(const vgl_vector_3d<double>& sd,
                                     vgl_vector_3d<vcl_vector<double> >& filter,
                                     vgl_vector_3d<unsigned>& c,
                                     vgl_vector_3d<int>& lo,
                                     vgl_vector_3d<int>& hi)
{
  // Size of filters (number of "taps") - filter should be ~ 7*sd wide
  vgl_vector_3d<unsigned> nt;
  nt.x_ = vnl_math_rnd(7.0*sd.x());
  nt.y_ = vnl_math_rnd(7.0*sd.y());
  nt.z_ = vnl_math_rnd(7.0*sd.z());

  // Temporary fix - force filters to have odd number of taps.
  // Not sure why, but even-numbered filters cause strange errors.
  if (nt.x_%2==0) nt.x_++;
  if (nt.y_%2==0) nt.y_++;
  if (nt.z_%2==0) nt.z_++;

  // Is nt even?
  if (nt.x()%2==0 || nt.y()%2==0 || nt.z()%2==0)
  {
    vcl_cout << "------------------------------------------------------\n"
             << "Warning: filter size is even: this may cause problems.\n"
             << nt.x() << '\t' << nt.y()<< '\t'  << nt.z() << '\n'
             << "------------------------------------------------------\n"
             << vcl_endl;
  }


  // Centre (absolute) tap of filter (or just past centre if even length)
  c = nt/2;

  // Lowest (relative) tap of filter
  lo.x_ = -c.x();
  lo.y_ = -c.y();
  lo.z_ = -c.z();

  // Highest (relative) tap of filter (depends whether filter length is odd)
  hi.x_ = c.x() +1 -(nt.x()%2);
  hi.y_ = c.y() +1 -(nt.y()%2);
  hi.z_ = c.z() +1 -(nt.z()%2);

  // Create a suitable 1D Gaussian filter for each dimension
  filter.x_.resize(nt.x());
  filter.y_.resize(nt.y());
  filter.z_.resize(nt.z());
  vil_gauss_filter_gen_ntap(sd.x(), 0, filter.x_);
  vil_gauss_filter_gen_ntap(sd.y(), 0, filter.y_);
  vil_gauss_filter_gen_ntap(sd.z(), 0, filter.z_);
}


//========================================================================
//: A convenience function to generate and apply an anisotropic Gaussian filter to a 3D image.
// \param src     The source image.
// \retval dest   The destination (filtered) image.
// \param sd      The width of the Gaussian (in voxel widths) for each dimension.
// \param work1   Workspace image (assumed to be correct size).
// \param work2   Workspace image (assumed to be correct size).
// \param work3   Workspace image (assumed to be correct size).
// \note          The filtering is done in floating-point arithmetic,
//                and the destination image is rounded to the same pixel
//                type as the source.
// \sa            vil3d_generate_gaussian_filters()
// \sa            vil3d_anisotropic_filter()
//========================================================================
template <class T>
inline
void vil3d_anisotropic_gaussian_filter(const vil3d_image_view<T>& src,
                                       vil3d_image_view<T>& dest,
                                       const vgl_vector_3d<double>& sd,
                                       vil3d_image_view<float>& work1,
                                       vil3d_image_view<float>& work2,
                                       vil3d_image_view<float>& work3)
{
  // Generate Gaussian filters suitable for each dimension
  vgl_vector_3d<vcl_vector<double> > filter;
  vgl_vector_3d<unsigned> c;
  vgl_vector_3d<int> lo;
  vgl_vector_3d<int> hi;
  vil3d_generate_gaussian_filters(sd, filter, c, lo, hi);

  // Specify the convolution boundary option for each dimension
  vgl_vector_3d<vil_convolve_boundary_option> cbo(vil_convolve_trim,
                                                  vil_convolve_trim,
                                                  vil_convolve_trim);

  // Apply the filters
  vil3d_anisotropic_filter(src, work3, filter, c, lo, hi, cbo, work1, work2);


  // Convert the results to destination type
  vil3d_convert_round(work3, dest);


  ////////////////////////////////////////////////////////////////////////
  //
  // Testing
  //
#ifdef PERFORM_TESTING
  //
  vcl_cout << "Source image:\n";
  vil3d_print_all(vcl_cout, src);

  vcl_cout << "\n\nWork3 image:\n";
  vil3d_print_all(vcl_cout, work3);
  double sum3 = 0;
  vil3d_math_sum(sum3, work3, 0);
  vcl_cout << "\nSum3= " << sum3 << vcl_endl;

  vcl_cout << "\n\nDest image:\n";
  vil3d_print_all(vcl_cout, dest);
  double sumd = 0;
  vil3d_math_sum(sumd, dest, 0);
  vcl_cout << "\nSumd= " << sumd << vcl_endl;
  //
#endif // PERFORM_TESTING
  //
  //////////////////////////////////////////////////////////////////////////
}


#endif // vil3d_anisotropic_filter_h_
