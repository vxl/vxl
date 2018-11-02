// This is brl/bseg/bbgm/bbgm_measure.h
#ifndef bbgm_measure_h_
#define bbgm_measure_h_
//:
// \file
// \brief Measurement wrappers for distribution images and probability images
// \author J.L. Mundy
// \date February 10, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>
#include "bbgm_image_of.h"
#include "bbgm_planes_to_sample.h"

//: measure a property (typically probability) requiring a tolerance, delta.
// For probability, delta defines a square n-dimensional box over which
// the probability density is integrated. For example, the measure_functor_
// is the bsta_probability_functor.
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             const vil_image_view<typename dist_::math_type>& image,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop,
             typename dist_::math_type delta)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.ni() == ni);
  assert(image.nj() == nj);
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const std::ptrdiff_t pstep = image.planestep();

  vector_ del(delta);
  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      vector_ sample;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      T temp_val;
      vector_ r_min = sample-del, r_max = sample+del;
      prop(*itr, r_min, r_max, temp_val);
      result(i,j) = temp_val;
    }
}

//: measure the probability of background given a fixed foreground probability (pf) at each pixel.
// Bayes rule gives
//
//               p(I|b)P(b)              p(I|b)(1-pf)          p(I|b)(1-pf)
//  P(b) =  ---------------------- = -------------------- =  -----------------
//          p(I|b)P(b) + p(I|f)P(f)  p(I|b)(1-pf)+ p(I|f)pf  p(I|b)(1-pf)+ pf
//
// the foreground probability is assumed to be uniform on the interval (0,1)
//
template <class dist_, class measure_functor_>
void measure_bkground(bbgm_image_of<dist_>& dimg,
                      const vil_image_view<typename dist_::math_type>& image,
                      vil_image_view<typename dist_::math_type>& result,
                      const measure_functor_& prop,
                      typename dist_::math_type pf)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.ni() == ni);
  assert(image.nj() == nj);
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const std::ptrdiff_t pstep = image.planestep();

  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      vector_ sample;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      T temp_val;
      prop(*itr, sample, temp_val);
      temp_val = temp_val*(T(1)-pf);
      temp_val = (temp_val)/(temp_val + pf);
      result(i,j) = temp_val;
    }
}

//: measure a property (typically probability) requiring a tolerance, delta.
// For probability, delta defines a square n-dimensional box over which
// the probability density is integrated. For example, the measure_functor_
// is the bsta_probability_functor. In this method the tolerance is
// augmented by an additional value that varies with position in the image
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             const vil_image_view<typename dist_::math_type>& image,
             vil_image_view<typename dist_::math_type>& var,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop,
             typename dist_::math_type delta)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.ni() == ni);
  assert(image.nj() == nj);
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const std::ptrdiff_t pstep = image.planestep();

  vector_ sample;
  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      vector_ sample, var_val;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      const T* vptr = &var(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(vptr, var_val, pstep);
      T temp_val;
      vector_ r_min = sample-delta-var_val, r_max = sample+delta+var_val;
      prop(*itr, r_min, r_max, temp_val);
      result(i,j) = temp_val;
    }
}

//: measure a property (typically probability) requiring a tolerance, delta.
// For probability, delta defines a square n-dimensional box over which
// the probability density is integrated. As an example, the measure_functor_
// is the bsta_probability_functor. In this method, the covariance of the
// distribution is augmented by and additional amount
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             const vil_image_view<typename dist_::math_type>& image,
             const vbl_array_2d<typename dist_::dist_type::covar_type> & covar,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop,
             const typename dist_::math_type delta)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;
  typedef typename dist_::dist_type::covar_type covar_t;
  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.ni() == ni);
  assert(image.nj() == nj);
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const std::ptrdiff_t pstep = image.planestep();

  vector_ sample;
  vector_ del(delta);
  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      vector_ sample;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      T temp_val;
      vector_ r_min = sample-del, r_max = sample+del;
      covar_t add_cov = covar[j][i];
      prop(*itr, r_min, r_max, add_cov, temp_val);
      result(i,j) = temp_val;
    }
}

//: measure a property (typically probability) requiring a tolerance.
// The tolerance is defined by a minimum and maximum values
// For probability, min and max defines a square n-dimensional box over which
// the probability density is integrated. This functor assumes that the same
// box applies to the entire distribution image. An example is probability of
// area lying within an interval, where the distribution image is a
// 2-d array of area probability densities.
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop,
             typename dist_::vector_type min_value,
             typename dist_::vector_type max_value)
{
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();

  result.set_size(ni,nj,1);

  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      T temp_val;
      prop(*itr, min_value, max_value, temp_val);
      result(i,j) = temp_val;
    }
}

#endif // bbgm_measure_h_
