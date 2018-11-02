// This is brl/bseg/bbgm/bbgm_update.h
#ifndef bbgm_update_h_
#define bbgm_update_h_
//:
// \file
// \brief Update wrappers for distribution images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date October 3, 2005
//
// \verbatim
//  Modifications
//   Apr 21, 2009  MJL  Update to work with vpdt
// \endverbatim

#include <vil/vil_image_view.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpdl/vpdt/vpdt_field_traits.h>

#include "bbgm_planes_to_sample.h"
#include "bbgm_image_of.h"

//: Update with no data
template <class dist_, class updater_>
void update(bbgm_image_of<dist_>& dimg,
            const updater_& updater)
{
  for (typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
       itr != dimg.end(); ++itr)
    updater(*itr);
}

//: Update with a new sample image
template <class dist_, class T, class updater_>
void update(bbgm_image_of<dist_>& dimg,
            const vil_image_view<T>& image,
            const updater_& updater)
{
  typedef typename updater_::field_type F;
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  assert(vpdt_field_traits<F>::dimension == image.nplanes());

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();

  const std::ptrdiff_t planestep = image.planestep();
  const std::ptrdiff_t istep = image.istep();
  const std::ptrdiff_t jstep = image.jstep();

  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  const T* row = image.top_left_ptr();
  for (unsigned int j=0; j<nj; ++j, row+=jstep){
    const T* col = row;
    for (unsigned int i=0; i<ni; ++i, col+=istep, ++itr){
      const T* data = col;
      F sample;
      bbgm_planes_to_sample<T,F,vpdt_field_traits<F>::dimension>::apply(data,sample,planestep);
      updater(*itr,sample);
    }
  }
}


//: Update with a new sample image only where mask(i,j)
template <class dist_, class T, class updater_>
void update_masked(bbgm_image_of<dist_>& dimg,
                   const vil_image_view<T>& image,
                   const updater_& updater,
                   const vil_image_view<bool>& mask)
{
  typedef typename updater_::field_type F;
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  assert(dimg.ni() == mask.ni());
  assert(dimg.nj() == mask.nj());
  assert(vpdt_field_traits<F>::dimension == image.nplanes());

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();

  const std::ptrdiff_t planestep = image.planestep();
  const std::ptrdiff_t istep = image.istep();
  const std::ptrdiff_t jstep = image.jstep();
  const std::ptrdiff_t m_istep = mask.istep();
  const std::ptrdiff_t m_jstep = mask.jstep();

  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  const bool* m_row = mask.top_left_ptr();
  const T* row = image.top_left_ptr();
  for (unsigned int j=0; j<nj; ++j, row+=jstep, m_row+=m_jstep){
    const T* col = row;
    const bool* m_col = m_row;
    for (unsigned int i=0; i<ni; ++i, col+=istep, m_col+=m_istep, ++itr){
      if (*m_col) {
        const T* data = col;
        F sample;
        bbgm_planes_to_sample<T,F,vpdt_field_traits<F>::dimension>::apply(data,sample,planestep);
        updater(*itr,sample);
      }
    }
  }
}


#endif // bbgm_update_h_
