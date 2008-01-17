// This is brl/bseg/bbgm/bbgm_detect.h
#ifndef bbgm_detect_h_
#define bbgm_detect_h_

//:
// \file
// \brief Detection wrappers in distribution images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/3/05
//
// \verbatim
//  Modifications
// \endverbatim


#include <vil/vil_image_view.h>
#include "bbgm_image_of.h"
#include "bbgm_planes_to_sample.h"
#include <vnl/vnl_vector_fixed.h>
#include <bsta/bsta_detector_mixture.h>



template <class _dist, class _detector>
void detect(bbgm_image_of<_dist>& dimg,
            const vil_image_view<typename _dist::math_type>& image,
            vil_image_view<bool>& result,
            const _detector& detector,
            int rad)
{
    typedef typename _dist::vector_type _vector;
    typedef typename _dist::math_type T;

    const unsigned ni = dimg.ni();
    const unsigned nj = dimg.nj();
    const unsigned d_np = _dist::dimension;
    assert(image.ni() == ni);
    assert(image.nj() == nj);
    assert(image.nplanes() == d_np);

    result.set_size(ni,nj,1);

    const vcl_ptrdiff_t r_istep = result.istep();
    const vcl_ptrdiff_t r_jstep = result.jstep();
    const vcl_ptrdiff_t d_istep = image.istep();
    const vcl_ptrdiff_t d_jstep = image.jstep();
    const vcl_ptrdiff_t d_pstep = image.planestep();

    bool temp_val;
    _vector sample;

    typename bbgm_image_of<_dist>::iterator itr = dimg.begin();
    bool* r_row = result.top_left_ptr();
    const T* d_row = image.top_left_ptr();
    for ( int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep){
        bool* r_col = r_row;
        const T* d_col = d_row;
        for ( int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep, ++itr){
            bool flag=false;
            for(int l=-rad;l<=rad;l++)
            {
                for(int k=-rad;k<=rad;k++)
                {
                    if(l+i>=0 && l+i<ni && k+j>=0 && k+j<nj)
                    {
                        const T * d_plane=&image(l+i,k+j);
                        bbgm_planes_to_sample<T,_vector,_dist::dimension>::apply(d_plane,sample,d_pstep);
                        if(detector(*itr, sample, temp_val))
                            if(temp_val)
                                flag=true;
                    }
                }
            }
            *r_col=flag;
        }
    }
}


template <class _dist, class _detector>
void detect_masked(bbgm_image_of<_dist>& dimg,
                   const vil_image_view<typename _dist::math_type>& image,
                   vil_image_view<bool>& result,
                   const _detector& detector,
                   int rad,vil_image_view<bool>& mask)
{
    typedef typename _dist::vector_type _vector;
    typedef typename _dist::math_type T;

    const unsigned ni = dimg.ni();
    const unsigned nj = dimg.nj();
    const unsigned d_np = _dist::dimension;
    assert(image.ni() == ni);
    assert(image.nj() == nj);
    assert(image.nplanes() == d_np);

    result.set_size(ni,nj,1);

    const vcl_ptrdiff_t r_istep = result.istep();
    const vcl_ptrdiff_t r_jstep = result.jstep();
    const vcl_ptrdiff_t d_istep = image.istep();
    const vcl_ptrdiff_t d_jstep = image.jstep();
    const vcl_ptrdiff_t d_pstep = image.planestep();

    const vcl_ptrdiff_t m_istep = mask.istep();
    const vcl_ptrdiff_t m_jstep = mask.jstep();


    bool temp_val;
    _vector sample;

    typename bbgm_image_of<_dist>::iterator itr = dimg.begin();
    bool* r_row = result.top_left_ptr();
    const T* d_row = image.top_left_ptr();
    bool * m_row = mask.top_left_ptr();

    for ( int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep,m_row+=m_jstep){
        bool* r_col = r_row;
        const T* d_col = d_row;
        bool * m_col = m_row;

        for ( int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep,m_col+=m_istep, ++itr){
            
            if(*m_col)
            {
            bool flag=false;
            for(int l=-rad;l<=rad;l++)
            {
                for(int k=-rad;k<=rad;k++)
                {
                    if(l+i>=0 && l+i<ni && k+j>=0 && k+j<nj)
                    {
                        if(mask(l+i,k+j))
                        {
                        const T * d_plane=&image(l+i,k+j);

                        bbgm_planes_to_sample<T,_vector,_dist::dimension>::apply(d_plane,sample,d_pstep);
                        if(detector(*itr, sample, temp_val))
                            if(temp_val)
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


template <class _dist, class _detector, class rT>
void detect_masked(bbgm_image_of<_dist>& dimg,
            const vil_image_view<typename _dist::math_type>& image,
            vil_image_view<rT>& result,
            vil_image_view<rT>& mask,
            const _detector& detector)
{
    typedef typename _dist::vector_type _vector;
    typedef typename _dist::math_type T;

    const unsigned ni = dimg.ni();
    const unsigned nj = dimg.nj();
    const unsigned d_np = _dist::dimension;
    assert(image.ni() == ni);
    assert(image.nj() == nj);
    assert(image.nplanes() == d_np);

    result.set_size(ni,nj,1);

    const vcl_ptrdiff_t r_istep = result.istep();
    const vcl_ptrdiff_t r_jstep = result.jstep();

    const vcl_ptrdiff_t m_istep = mask.istep();
    const vcl_ptrdiff_t m_jstep = mask.jstep();

    const vcl_ptrdiff_t d_istep = image.istep();
    const vcl_ptrdiff_t d_jstep = image.jstep();
    const vcl_ptrdiff_t d_pstep = image.planestep();

    rT temp_val;
    _vector sample;

    typename bbgm_image_of<_dist>::iterator itr = dimg.begin();
    rT* r_row = result.top_left_ptr();
    rT* m_row = mask.top_left_ptr();
    const T* d_row = image.top_left_ptr();
    for (unsigned int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep,m_row+=m_jstep){
        rT* r_col = r_row;
        rT* m_col = m_row;
        const T* d_col = d_row;
        for (unsigned int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep,m_col+=m_istep, ++itr){
            const T* d_plane = d_col;
            if(*m_col)
            {
            bbgm_planes_to_sample<T,_vector,_dist::dimension>::apply(d_plane,sample,d_pstep);
            if(detector(*itr, sample, temp_val))
                *r_col =temp_val;
            }
      
    }
  }
}

#endif // bbgm_detect_h_
