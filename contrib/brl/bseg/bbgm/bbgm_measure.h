// This is brl/bseg/bbgm/bbgm_measure.h
#ifndef bbgm_measure_h_
#define bbgm_measure_h_
//:
// \file
// \brief Measurement wrappers in distribution images
// \author J.L. Mundy
// \date February 10, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_cassert.h>
#include <vil/vil_image_view.h>
#include "bbgm_image_of.h"
#include "bbgm_planes_to_sample.h"

//: measure a property (typically probability) requiring a tolerance, delta.
// For probability, delta defines a square n-dimensional box over which
// the probablity density is integrated. In this case measure_functor_
// is the bsta_probability_functor.
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             const vil_image_view<typename dist_::math_type>& image,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& measure,
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

    const vcl_ptrdiff_t pstep = image.planestep();

    vector_ sample;
    vector_ del(delta);
    typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
    for ( unsigned j=0; j<nj; ++j)
      for ( unsigned i=0; i<ni; ++i, ++itr){
        vector_ sample;
        const T* iptr = &image(i,j);
        bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
        T temp_val;
        vector_ r_min = sample-del, r_max = sample+del;
        measure(*itr, r_min, r_max, temp_val);
        result(i,j) = temp_val;
      }
}

#endif // bbgm_measure_h_
