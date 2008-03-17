// This is brl/bbas/bil/algo/bil_edge_indicator.h
#ifndef bil_edge_indicator_h_
#define bil_edge_indicator_h_
//:
// \file
// \brief Functions to compute Malladi image force
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 12/03/2005
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vnl/vnl_math.h>

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_transform.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>

#include <vcl_iostream.h>

//: Functor class to compute malladi image force function
class bil_malladi_scale_functor
{
 public:
  float operator()(float x) const {return x<0 ? 1.0f : 1.0f/(1.0f+x); }
  double operator()(double x) const {return x<0 ? 1.0 : 1.0/(1.0+x); }
};

//: Functor class to compute square of a number
class bil_square_functor
{
 public:
  float operator()(float x) const {return x*x; }
  double operator()(double x) const {return x*x; }
};


//: Functor class to compute cubic of a number
class bil_cube_functor
{
 public:
  float operator()(float x) const {return x*x*x; }
  double operator()(double x) const {return x*x*x; }
};

//: Functor class to compute power of a number
class bil_power_functor
{
  int p_;
 public:
  bil_power_functor(int p):p_(p){}
  float operator()(float x) const {return vcl_pow(x, (float)p_); }
  double operator()(double x) const {return vcl_pow(x, p_); }
};


//: Functor class to apply a Gaussian function on a number
class bil_gaussian_functor
{
  double mu_;
  double sigma_;
 public:
  bil_gaussian_functor(double mu, double sigma): mu_(mu), sigma_(sigma){}
  double operator()(double x) const
  {
    return vcl_exp(-(x-mu_)*(x-mu_)/(2*sigma_*sigma_) ) /
      (sigma_ * vnl_math::two_over_sqrtpi * vnl_math::sqrt1_2 / 2);
  }
  float operator()(float x) const
  {
    return float(vcl_exp(-(x-mu_)*(x-mu_)/(2*sigma_*sigma_) ) /
      (sigma_ * vnl_math::two_over_sqrtpi * vnl_math::sqrt1_2 / 2));
  }
};


// ------------------------------------------------------------------
//: Compute image force using gradient of image
// \a g = 1 / (1 + (|dG*I|/M)^p
// \a dG : Gaussian smoothing
// \a M : down_scale value
// \a p: exponent
// (for now) keep \a gauss_sigma < 0.7
// Reference: Malladi, R., J. A. Sethian, and B. C. Vemuri, "Shape Modeling with front propagation: A Level Set Approach".
template <class inT, class outT>
inline void bil_malladi_image_force(const vil_image_view<inT >& src_im,
                                    double M,
                                    int exponent,
                                    double gauss_sigma,
                                    vil_image_view<outT >& gmap)
{
  vil_image_view<outT > smoothed;

  // smooth source image using gaussian filter
  vil_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil_gauss_filter_5tap<inT, outT>(src_im, smoothed, gauss_params);

  //compute image gradient values
  vil_image_view<outT > grad_i, grad_j, grad_mag;
  vil_sobel_3x3(smoothed, grad_i, grad_j);

  // gradient magnitude
  vil_math_image_vector_mag(grad_i, grad_j, grad_mag);

  //compute image gradient magnitude and values for
  // g = 1/1+(|dG*I|/M)^p
  //                 1
  // g = ---------------------------
  //       1 +   (|dG * I| /M)^p
  //

  if (M == 0) M = 1;
  vil_math_scale_values(grad_mag, (1.0/M));

  // compute (|dG*I|/M)^p
  // special treatment for cases exponent=2 and exponent=3
  switch (exponent)
  {
  case (2):
    vil_transform(grad_mag, bil_square_functor());
    break;
  case (3):
    vil_transform(grad_mag, bil_cube_functor());
    break;
  default:
    vil_transform(grad_mag, bil_power_functor(exponent));
    break;
  };

  vil_transform(grad_mag, bil_malladi_scale_functor());

  // g
  gmap = grad_mag;
#if 0
  //compute gradients of g
  vil_sobel_3x3(gmap, gx, gy);
#endif
  return;
}


// ------------------------------------------------------------------
//: Compute image force using gradient of image and feature map
// Similar to malladi_image_force but also take in an feature map
template <class inT, class outT>
inline void bil_malladi_image_force_with_feature_map(
  const vil_image_view<inT >& src_im,
  double M,
  int exponent,
  double gauss_sigma,
  const vil_image_view<bool>& feature_map,
  vil_image_view<outT >& gmap)
{
#if 0
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned np = src_im.nplanes();
#endif

  vil_image_view<outT > smoothed;

  // smooth source image using gaussian filter
  vil_gauss_filter_5tap_params gauss_params(gauss_sigma);
  vil_gauss_filter_5tap<inT, outT>(src_im, smoothed, gauss_params);

  //compute image gradient values
  vil_image_view<outT > grad_i, grad_j, grad_mag;
  vil_sobel_3x3(smoothed, grad_i, grad_j);

  // gradient magnitude
  vil_math_image_vector_mag(grad_i, grad_j, grad_mag);

  // set grad_mag to max wherever there is feature
  outT min_value, max_value;
  vil_math_value_range(grad_mag, min_value, max_value);
  for (unsigned p=0; p<grad_mag.nplanes(); ++p)
  {
    for (unsigned j=0; j<grad_mag.nj(); ++j)
    {
      for (unsigned i=0; i<grad_mag.ni(); ++i)
      {
        if (feature_map(i, j, p) == true)
        {
          grad_mag(i, j, p) = max_value;
        }
      }
    }
  }

  //compute image gradient magnitude and values for
  // g = 1/1+(|dG*I|/M)^p
  //                 1
  // g = ---------------------------
  //       1 +   (|dG * I| /M)^p
  //

  if (M == 0) M = 1;
  vil_math_scale_values(grad_mag, (1.0/M));

  // compute (|dG*I|/M)^p
  // special treatment for cases exponent=2 and exponent=3
  switch (exponent)
  {
  case (2):
    vil_transform(grad_mag, bil_square_functor());
    break;
  case (3):
    vil_transform(grad_mag, bil_cube_functor());
    break;
  default:
    vil_transform(grad_mag, bil_power_functor(exponent));
    break;
  };

  vil_transform(grad_mag, bil_malladi_scale_functor());

  // g
  gmap = grad_mag;

  // make sure the min is ZERO
  vil_math_value_range(gmap, min_value, max_value);
  vil_math_scale_and_offset_values(gmap, outT(1), -min_value);

  ////compute gradients of g
  //vil_sobel_3x3(gmap, gx, gy);
  return;
}


// ------------------------------------------------------------------
//: Compute normalized inverse gradient image
// The output image is 0 at max gradient, 1 at min gradient and the values in between
// are mapped by a function depending on the option
// Currently only "quadratic" mapping is implemented
template <class inT, class outT>
inline void bil_normalized_inverse_gradient(
  const vil_image_view<inT >& src_image,
  double gauss_sigma,
  vil_image_view<outT >& out_image,
  const vcl_string& option = "quadratic")
{
  // smooth image
  vil_image_view<float > float_image;
  vil_convert_cast(src_image, float_image);
  vil_image_view<float > gauss_image;
  vil_gauss_filter_5tap<float, float >(float_image, gauss_image,
                                       vil_gauss_filter_5tap_params(gauss_sigma));

  // compute gradient using sobel kernel
  vil_image_view<float > grad_x;
  vil_image_view<float > grad_y;
  vil_sobel_3x3<float, float >(gauss_image, grad_x, grad_y);

  // gradient magnitude
  vil_image_view<float > grad_mag_cost;
  vil_math_image_vector_mag(grad_x, grad_y, grad_mag_cost);

  // transform gradient magnitude by an inverse linear ramp to get cost
  float grad_max, grad_min;
  vil_math_value_range(grad_mag_cost, grad_min, grad_max);

  if (option == "quadratic")
  {
    // use inverse quadratic instead of linear
    // f(x) = 1 + a (x-b)^2; such that
    // f(xmin) = 1;
    // f(xmax) = 0;
    // f'(xmin) = 0; --> to suppress small gradient
    float b = grad_min;
    float a = -1/((grad_max-grad_min)*(grad_max-grad_min));

    // transform gradient cost
    for (unsigned int i=0; i<grad_mag_cost.ni(); ++i)
    {
      for (unsigned int j=0; j<grad_mag_cost.nj(); ++j)
      {
        grad_mag_cost(i,j) =
          1 + a*vnl_math_sqr(grad_mag_cost(i, j)-b);
      }
    }

    vil_convert_cast(grad_mag_cost, out_image);
    return;
  }
  else
  {
    vcl_cerr << "ERROR: Only 'quadratic' mapping is currently implemented.\n";
    return;
  }
}


// ------------------------------------------------------------------
//: Compute negative normalized gradient image
// The gradient magnitude is normalized with mean at the max_gradient
// then inverse the sign
template <class inT, class outT>
inline void bil_negative_normalized_gradient_magnitude(
  const vil_image_view<inT >& src_image,
  double smoothing_sigma,
  double normalized_sigma,
  vil_image_view<outT >& out_image)
{
  // smooth image
  vil_image_view<float > float_image;
  vil_convert_cast(src_image, float_image);
  vil_image_view<float > gauss_image;
  vil_gauss_filter_5tap<float, float >(float_image, gauss_image,
                                       vil_gauss_filter_5tap_params(smoothing_sigma));

  // compute gradient using sobel kernel
  vil_image_view<float > grad_x;
  vil_image_view<float > grad_y;
  vil_sobel_3x3<float, float >(gauss_image, grad_x, grad_y);

  // gradient magnitude
  vil_image_view<float > grad_mag_cost;
  vil_math_image_vector_mag(grad_x, grad_y, grad_mag_cost);

  // transform gradient magnitude by a Gaussian transform
  float grad_max, grad_min;
  vil_math_value_range(grad_mag_cost, grad_min, grad_max);

  vil_transform(grad_mag_cost,
                bil_gaussian_functor(grad_max, (grad_max-grad_min)*normalized_sigma));

  vil_math_scale_values(grad_mag_cost, -1.0);

  vil_convert_cast(grad_mag_cost, out_image);
  return;
}

#endif // bil_edge_indicator_h_
