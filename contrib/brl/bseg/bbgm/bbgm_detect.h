// This is brl/bseg/bbgm/bbgm_detect.h
#ifndef bbgm_detect_h_
#define bbgm_detect_h_
//:
// \file
// \brief Detection wrappers in distribution images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date October 3, 2005
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/vil_image_view.h>
#include "bbgm_image_of.h"
#include "bbgm_planes_to_sample.h"
#include <bsta/bsta_detector_mixture.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: For each pixel, detect at all \a se neighbors in bbgm_image
//  \returns true if detection succeeds at any neighbor
template <class dist_, class detector_, class dT>
void detect(bbgm_image_of<dist_>& dimg,
            const vil_image_view<dT>& data,
            vil_image_view<bool>& result,
            const detector_& detector,
            const vil_structuring_element& se)
{
  typedef typename dist_::field_type F;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = vpdt_field_traits<F>::dimension;
  assert(data.ni() == ni);
  assert(data.nj() == nj);
  assert(data.nplanes() == d_np);

  result.set_size(ni,nj,1);
  const std::ptrdiff_t r_istep = result.istep();
  const std::ptrdiff_t r_jstep = result.jstep();
  const std::ptrdiff_t d_istep = data.istep();
  const std::ptrdiff_t d_jstep = data.jstep();
  const std::ptrdiff_t d_pstep = data.planestep();

  const unsigned size_se = se.p_i().size();

  bool temp_val;
  F sample;
  bool* r_row = result.top_left_ptr();
  const dT* d_row = data.top_left_ptr();
  for (unsigned int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep){
    bool* r_col = r_row;
    const dT* d_col = d_row;
    for (unsigned int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep){
      bool& detected = *r_col;
      const dT* d_plane = d_col;
      for (unsigned int k=0; k<d_np; ++k, d_plane+=d_pstep)
        sample[k] = *d_plane;
      detected = false;
      for (unsigned int k=0; k<size_se; ++k){
        int ri = static_cast<int>(i)+se.p_i()[k];
        int rj = static_cast<int>(j)+se.p_j()[k];
        if (ri < 0 || ri >= static_cast<int>(ni) ||
            rj < 0 || rj >= static_cast<int>(nj) )
          continue;
        if (detector(dimg(ri,rj), sample, temp_val) && temp_val){
          detected = true;
          break;
        }
      }
    }
  }
}

//: For each masked pixel, detect at all \a se neighbors in bbgm_image
// \returns true if detection succeeds at any neighbor
template <class dist_, class detector_, class dT>
void detect_masked(bbgm_image_of<dist_>& dimg,
                   const vil_image_view<dT>& data,
                   vil_image_view<bool>& result,
                   const detector_& detector,
                   const vil_structuring_element& se,
                   const vil_image_view<bool>& mask)
{
  typedef typename dist_::field_type F;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = vpdt_field_traits<F>::dimension;
  assert(data.ni() == ni);
  assert(data.nj() == nj);
  assert(data.nplanes() == d_np);
  assert(mask.ni() == ni);
  assert(mask.nj() == nj);

  result.set_size(ni,nj,1);
  const std::ptrdiff_t r_istep = result.istep();
  const std::ptrdiff_t r_jstep = result.jstep();
  const std::ptrdiff_t d_istep = data.istep();
  const std::ptrdiff_t d_jstep = data.jstep();
  const std::ptrdiff_t d_pstep = data.planestep();
  const std::ptrdiff_t m_istep = mask.istep();
  const std::ptrdiff_t m_jstep = mask.jstep();

  const unsigned size_se = se.p_i().size();

  bool temp_val;
  F sample;
  bool* r_row = result.top_left_ptr();
  const dT* d_row = data.top_left_ptr();
  const bool* m_row = mask.top_left_ptr();
  for (unsigned int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep, m_row+=m_jstep){
    bool* r_col = r_row;
    const dT* d_col = d_row;
    const bool* m_col = m_row;
    for (unsigned int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep, m_col+=m_istep){
      if (!*m_col)
        continue;
      bool& detected = *r_col;
      const dT* d_plane = d_col;
      for (unsigned int k=0; k<d_np; ++k, d_plane+=d_pstep)
        sample[k] = *d_plane;
      detected = false;
      for (unsigned int k=0; k<size_se; ++k){
        int ri = static_cast<int>(i)+se.p_i()[k];
        int rj = static_cast<int>(j)+se.p_j()[k];
        if (ri < 0 || ri >= ni || rj < 0 || rj >= nj)
          continue;
        if (detector(dimg(ri,rj), sample, temp_val) && temp_val){
          detected = true;
          break;
        }
      }
    }
  }
}


template <class dist_, class detector_>
void detect(bbgm_image_of<dist_>& dimg,
            const vil_image_view<typename dist_::math_type>& image,
            vil_image_view<bool>& result,
            const detector_& detector,
            int rad)
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

    const std::ptrdiff_t r_istep = result.istep();
    const std::ptrdiff_t r_jstep = result.jstep();
    const std::ptrdiff_t d_istep = image.istep();
    const std::ptrdiff_t d_jstep = image.jstep();
    const std::ptrdiff_t d_pstep = image.planestep();

    bool temp_val;
    vector_ sample;

    typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
    bool* r_row = result.top_left_ptr();
    const T* d_row = image.top_left_ptr();
    for ( int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep){
        bool* r_col = r_row;
        const T* d_col = d_row;
        for ( int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep, ++itr){
            bool flag=false;
            for (int l=-rad;l<=rad;l++)
            {
                for (int k=-rad;k<=rad;k++)
                {
                    if (l+i>=0 && l+i<ni && k+j>=0 && k+j<nj)
                    {
                        const T * d_plane=&image(l+i,k+j);
                        bbgm_planes_to_sample<T,vector_,dist_::dimension>::apply(d_plane,sample,d_pstep);
                        if (detector(*itr, sample, temp_val))
                            if (temp_val)
                                flag=true;
                    }
                }
            }
            *r_col=flag;
        }
    }
}


template <class dist_, class detector_>
void detect_masked(bbgm_image_of<dist_>& dimg,
                   const vil_image_view<typename dist_::math_type>& image,
                   vil_image_view<bool>& result,
                   const detector_& detector,
                   int rad,vil_image_view<bool>& mask)
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

    const std::ptrdiff_t r_istep = result.istep();
    const std::ptrdiff_t r_jstep = result.jstep();
    const std::ptrdiff_t d_istep = image.istep();
    const std::ptrdiff_t d_jstep = image.jstep();
    const std::ptrdiff_t d_pstep = image.planestep();

    const std::ptrdiff_t m_istep = mask.istep();
    const std::ptrdiff_t m_jstep = mask.jstep();


    bool temp_val;
    vector_ sample;

    typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
    bool* r_row = result.top_left_ptr();
    const T* d_row = image.top_left_ptr();
    bool * m_row = mask.top_left_ptr();

    for ( int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep,m_row+=m_jstep){
        bool* r_col = r_row;
        const T* d_col = d_row;
        bool * m_col = m_row;

        for ( int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep,m_col+=m_istep, ++itr)
        {
            if (*m_col)
            {
            bool flag=false;
            for (int l=-rad;l<=rad;l++)
            {
                for (int k=-rad;k<=rad;k++)
                {
                    if (l+i>=0 && l+i<ni && k+j>=0 && k+j<nj)
                    {
                        if (mask(l+i,k+j))
                        {
                        const T * d_plane=&image(l+i,k+j);

                        bbgm_planes_to_sample<T,vector_,dist_::dimension>::apply(d_plane,sample,d_pstep);
                        if (detector(*itr, sample, temp_val))
                            if (temp_val)
                                flag=true;
                        }
                    }
                }
            }
            *r_col=flag;
        }
            else
                *r_col=true;
        }
    }
}


template <class dist_, class detector_, class rT>
void detect_masked(bbgm_image_of<dist_>& dimg,
                   const vil_image_view<typename dist_::math_type>& image,
                   vil_image_view<rT>& result,
                   vil_image_view<rT>& mask,
                   const detector_& detector)
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

    const std::ptrdiff_t r_istep = result.istep();
    const std::ptrdiff_t r_jstep = result.jstep();

    const std::ptrdiff_t m_istep = mask.istep();
    const std::ptrdiff_t m_jstep = mask.jstep();

    const std::ptrdiff_t d_istep = image.istep();
    const std::ptrdiff_t d_jstep = image.jstep();
    const std::ptrdiff_t d_pstep = image.planestep();

    rT temp_val;
    vector_ sample;

    typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
    rT* r_row = result.top_left_ptr();
    rT* m_row = mask.top_left_ptr();
    const T* d_row = image.top_left_ptr();
    for (unsigned int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep,m_row+=m_jstep){
        rT* r_col = r_row;
        rT* m_col = m_row;
        const T* d_col = d_row;
        for (unsigned int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep,m_col+=m_istep, ++itr){
            const T* d_plane = d_col;
            if (*m_col)
            {
            bbgm_planes_to_sample<T,vector_,dist_::dimension>::apply(d_plane,sample,d_pstep);
            if (detector(*itr, sample, temp_val))
                *r_col =temp_val;
            }
    }
  }
}

#endif // bbgm_detect_h_
