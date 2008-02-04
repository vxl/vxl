// This is brl/bseg/bbgm/bbgm_apply.h
#ifndef bbgm_apply_h_
#define bbgm_apply_h_
//:
// \file
// \brief Apply functors to distribution images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/3/05
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vcl_cassert.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_vector_fixed.h>

#include "bbgm_image_of.h"
#include "bbgm_planes_to_sample.h"

//: Apply the functor at every pixel
//  \returns an image of results, each vector component in a separate plane
//  \param fail_val sets the value of pixels where the functor fails
template <class dist_, class functor_, class T, bool single_ = false>
struct bbgm_apply_no_data
{
  static inline void apply(const bbgm_image_of<dist_>& dimg,
                           const functor_& functor,
                           vil_image_view<T>& result,
                           const T* fail_val = NULL )
  {
    typedef typename functor_::return_T return_T;

    const unsigned ni = dimg.ni();
    const unsigned nj = dimg.nj();
    const unsigned np = functor_::return_dim;

    if (ni==0 && nj==0)
      return;

    result.set_size(ni,nj,np);

    const vcl_ptrdiff_t planestep = result.planestep();
    const vcl_ptrdiff_t istep = result.istep();
    const vcl_ptrdiff_t jstep = result.jstep();

    return_T temp_val;
    typename bbgm_image_of<dist_>::const_iterator itr = dimg.begin();
    T* row = result.top_left_ptr();
    for (unsigned int j=0; j<nj; ++j, row+=jstep){
      T* col = row;
      for (unsigned int i=0; i<ni; ++i, col+=istep, ++itr){
        T* data = col;
        if ( functor(*itr,temp_val) ) {
          typename return_T::iterator v_itr = temp_val.begin();
          for (unsigned int p=0; p<np; ++p, data += planestep, ++v_itr){
            *data = *v_itr;
          }
        }
        else if (fail_val){
          const T* v_itr = fail_val;
          for (unsigned int p=0; p<np; ++p, data += planestep, ++v_itr){
            *data = *v_itr;
          }
        }
      }
    }
  }
};


template <class dist_, class functor_, class T>
struct bbgm_apply_no_data<dist_,functor_,T,true>
{
  static inline void apply(const bbgm_image_of<dist_>& dimg,
                           const functor_& functor,
                           vil_image_view<T>& result,
                           const T* fail_val = NULL )
  {
    typedef typename functor_::return_T return_T;

    const unsigned ni = dimg.ni();
    const unsigned nj = dimg.nj();

    if (ni==0 && nj==0)
      return;

    result.set_size(ni,nj,1);
    const vcl_ptrdiff_t istep = result.istep();
    const vcl_ptrdiff_t jstep = result.jstep();

    return_T temp_val;
    typename bbgm_image_of<dist_>::const_iterator itr = dimg.begin();
    T* row = result.top_left_ptr();
    for (unsigned int j=0; j<nj; ++j, row+=jstep){
      T* col = row;
      for (unsigned int i=0; i<ni; ++i, col+=istep, ++itr){
        if (functor(*itr, temp_val))
          *col = static_cast<T>(temp_val);
        else if (fail_val)
          *col = fail_val[0];
      }
    }
  }
};

//: Apply without data
template <class dist_, class functor_, class T>
void bbgm_apply(const bbgm_image_of<dist_>& dimg,
                const functor_& functor,
                vil_image_view<T>& result,
                const T* fail_val = NULL )
{
  bbgm_apply_no_data<dist_,functor_,T,functor_::return_dim == 1>::apply(dimg,functor,result,fail_val);
}


//: Apply the functor at every pixel
//  \returns an image of results, each vector component in a separate plane
//  \param fail_val sets the value of pixels where the functor fails
template <class dist_, class functor_, class dT, class rT, bool single_ = false>
struct bbgm_apply_data
{
  static inline void apply(const bbgm_image_of<dist_>& dimg,
                           const functor_& functor,
                           const vil_image_view<dT>& data,
                           vil_image_view<rT>& result,
                           const rT* fail_val = NULL )
  {
    typedef typename functor_::return_T return_T;
    typedef vnl_vector_fixed<dT,dist_::dimension> vector_;

    const unsigned ni = dimg.ni();
    const unsigned nj = dimg.nj();
    const unsigned d_np = dist_::dimension;
    const unsigned r_np = functor_::return_dim;
    assert(data.ni() == ni);
    assert(data.nj() == nj);
    assert(data.nplanes() == d_np);

    result.set_size(ni,nj,r_np);
    const vcl_ptrdiff_t r_istep = result.istep();
    const vcl_ptrdiff_t r_jstep = result.jstep();
    const vcl_ptrdiff_t r_pstep = result.planestep();
    const vcl_ptrdiff_t d_istep = data.istep();
    const vcl_ptrdiff_t d_jstep = data.jstep();
    const vcl_ptrdiff_t d_pstep = data.planestep();

    return_T temp_val;
    vector_ sample;
    typename bbgm_image_of<dist_>::const_iterator itr = dimg.begin();
    rT* r_row = result.top_left_ptr();
    const dT* d_row = data.top_left_ptr();
    for (unsigned int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep){
      rT* r_col = r_row;
      const dT* d_col = d_row;
      for (unsigned int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep, ++itr){
        rT* r_plane = r_col;
        const dT* d_plane = d_col;
        for (unsigned int k=0; k<d_np; ++k, d_plane+=d_pstep)
          sample[k] = *d_plane;
        if (functor(*itr, sample, temp_val)){
          for (unsigned int k=0; k<r_np; ++k, r_plane+=r_pstep)
            *r_plane = temp_val[k];
        }
        else if ( fail_val ){
          for (unsigned int k=0; k<r_np; ++k, r_plane+=r_pstep)
            *r_plane = fail_val[k];
        }
      }
    }
  }
};


template <class dist_, class functor_, class dT, class rT>
struct bbgm_apply_data<dist_,functor_,dT,rT,true>
{
  static inline void apply(const bbgm_image_of<dist_>& dimg,
                           const functor_& functor,
                           const vil_image_view<dT>& data,
                           vil_image_view<rT>& result,
                           const rT* fail_val = NULL )
  {
    typedef typename functor_::return_T return_T;
    typedef typename dist_::vector_type vector_;

    const unsigned ni = dimg.ni();
    const unsigned nj = dimg.nj();
    const unsigned d_np = dist_::dimension;
    assert(data.ni() == ni);
    assert(data.nj() == nj);
    assert(data.nplanes() == d_np);

    result.set_size(ni,nj,1);
    const vcl_ptrdiff_t r_istep = result.istep();
    const vcl_ptrdiff_t r_jstep = result.jstep();
    const vcl_ptrdiff_t d_istep = data.istep();
    const vcl_ptrdiff_t d_jstep = data.jstep();
    const vcl_ptrdiff_t d_pstep = data.planestep();

    return_T temp_val;
    vector_ sample;
    typename bbgm_image_of<dist_>::const_iterator itr = dimg.begin();
    rT* r_row = result.top_left_ptr();
    const dT* d_row = data.top_left_ptr();
    for (unsigned int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep){
      rT* r_col = r_row;
      const dT* d_col = d_row;
      for (unsigned int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep, ++itr){
        const dT* d_plane = d_col;
        bbgm_planes_to_sample<dT,vector_,dist_::dimension>::apply(d_plane,sample,d_pstep);
        if (functor(*itr, sample, temp_val))
          *r_col = static_cast<rT>(temp_val);
        else if ( fail_val )
          *r_col = fail_val[0];
      }
    }
  }
};


//: Apply with data
template <class dist_, class functor_, class dT, class rT>
void bbgm_apply(const bbgm_image_of<dist_>& dimg,
                 const functor_& functor,
                 const vil_image_view<dT>& data,
                 vil_image_view<rT>& result,
                 const rT* fail_val = NULL )
{
  bbgm_apply_data<dist_,functor_,dT,rT,functor_::return_dim == 1>::apply(dimg,functor,data,result,fail_val);
}


#endif // bbgm_apply_h_
