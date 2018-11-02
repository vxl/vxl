#ifndef vil3d_locally_z_normalise_h_
#define vil3d_locally_z_normalise_h_
//:
//  \file
//  \brief Locally apply z-normalisation to images
//  \author Tim Cootes

#include <vil/vil_convert.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/algo/vil3d_exp_filter.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Locally apply z-normalisation to images
// The mean and standard deviation over a window around each voxel.
// The voxel_value is modified v_new = (v_old - mean)/max(min_var, stdev)
// For integer types min_var is 1. For floating point min_var is the larger of
// 0.001 of the image wide stdev, or sqrt_eps.
template <class T>
inline void vil3d_locally_z_normalise(const vil3d_image_view<T>& input,
                                      double half_width_i,
                                      double half_width_j,
                                      double half_width_k,
                                      vil3d_image_view<T>& output)
{
  assert(input.nplanes()==1);

  unsigned ni=input.ni();
  unsigned nj=input.nj();
  unsigned nk=input.nk();
  output.set_size(ni,nj,nk);

  if (ni*nj*nk == 0)
    return;

  //: Workspace
  vil3d_image_view<float> smth_im, sqr_im, smth_sqr_im;

  // k^half_width = 0.5
  // So k=exp(log(0.5)/half_width)
  double ki = std::exp(std::log(0.5)/half_width_i);
  double kj = std::exp(std::log(0.5)/half_width_j);
  double kk = std::exp(std::log(0.5)/half_width_k);

  vil3d_exp_filter(input,smth_im,ki,kj,kk);
  vil3d_math_image_product(input,input,sqr_im);
  vil3d_exp_filter(input,smth_sqr_im,ki,kj,kk);

  double min_var=1.0;
  if (!std::numeric_limits<T>::is_integer)
  {
    double sum=0, sum_sq=0;
    vil3d_math_sum(sum, input, 0);
    vil3d_math_sum(sum_sq, sqr_im, 0);
    double mean = sum/input.size();
    min_var = std::max( sum_sq/(input.size()) - mean*mean,
      static_cast<double>(std::sqrt(std::numeric_limits<T>::epsilon())) );
  }

  vil_convert_round_pixel<double, T> round;

  for (unsigned k=0;k<nk;++k)
   for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
    {
      double mean = smth_im(i,j,k);
      double var = smth_sqr_im(i,j,k)-mean*mean;
      double sd = std::sqrt(std::max(min_var,var));
      double v = (input(i,j,k)-mean)/sd;

      round(v, output(i,j,k));
    }
}

#endif // vil3d_locally_z_normalise_h_
