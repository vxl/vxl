// This is brl/bbas/bil/algo/bil_harr_wavelet_transform.txx
#ifndef bil_harr_wavelet_transform_txx_
#define bil_harr_wavelet_transform_txx_
//:
// \file

#include "bil_harr_wavelet_transform.h"

template< class T>
static inline
void bil_harr_helper(const T* src, T* dest,
                     vcl_ptrdiff_t s_step, vcl_ptrdiff_t d_step,
                     unsigned int length)
{
  for (unsigned int i=0; i<length; ++i){
    *dest = T(*src + *(src+s_step))/2;
    *(dest+length*d_step) = *dest - *src;
    src += 2*s_step;
    dest += d_step;
  }
}

//: Compute the Harr wavelet transform on each plane
template< class T >
void bil_harr_wavelet_transform(const vil_image_view<T>& src,
                                      vil_image_view<T>& dest)
{
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  unsigned np = src.nplanes();
  dest.set_size(ni,nj,np);

  vcl_ptrdiff_t istepS=src.istep(),jstepS=src.jstep(),pstepS=src.planestep();
  vcl_ptrdiff_t istepD=dest.istep(),jstepD=dest.jstep(),pstepD=dest.planestep();
  const T* planeS = src.top_left_ptr();
        T* planeD = dest.top_left_ptr();

  T* temp = new T[ni>nj?ni:nj];
  for (unsigned p=0;p<np;++p,planeS+=pstepS,planeD+=pstepD)
  {
    const T* rowS = planeS;
          T* rowD = planeD;
    for (unsigned j=0;j<nj;++j,rowS+=jstepS,rowD+=jstepD)
    {
      T* temp_ptr = temp;
      const T* temp_src = rowS;
      for (unsigned int i=0;i<ni;++i, ++temp_ptr, temp_src+=istepS)
        *temp_ptr = *temp_src;
      for (unsigned int cni = ni; cni%2 == 0; cni /= 2){
        bil_harr_helper(temp,rowD,1,istepD,cni/2);
        temp_ptr = temp;
        temp_src = rowD;
        for (unsigned int i=0;i<cni/2;++i, ++temp_ptr, temp_src+=istepS)
          *temp_ptr = *temp_src;
      }
    }

    T* colD = planeD;
    for (unsigned i=0;i<ni;++i,colD+=istepD)
    {
      for (unsigned int cnj = nj; cnj%2 == 0; cnj /= 2){
        T* temp_ptr = temp;
        const T* temp_src = colD;
        for (unsigned int j=0;j<cnj;++j, ++temp_ptr, temp_src+=jstepD)
          *temp_ptr = *temp_src;
        bil_harr_helper(temp,colD,1,jstepD,cnj/2);
      }
    }
  }
  delete [] temp;
}


template< class T>
static inline
void bil_harr_inv_helper(const T* src, T* dest,
                         vcl_ptrdiff_t s_step, vcl_ptrdiff_t d_step,
                          unsigned int length)
{
  for (unsigned int i=0; i<length; ++i){
    const T& diff = *(src+length*s_step);
    *dest = *src - diff;
    *(dest+d_step) = *src + diff;
    src += s_step;
    dest += 2*d_step;
  }
}


//: Computes the inverse of the Harr wavelet transform on each plane
template< class T >
void bil_harr_wavelet_inverse(const vil_image_view<T>& src,
                                    vil_image_view<T>& dest)
{
  unsigned ni = src.ni();
  unsigned nj = src.nj();
  unsigned np = src.nplanes();
  dest.set_size(ni,nj,np);
  if (src != dest)
    dest.deep_copy(src);

  //vcl_ptrdiff_t istepS=src.istep(),jstepS=src.jstep(),pstepS=src.planestep();
  vcl_ptrdiff_t istepD=dest.istep(),jstepD=dest.jstep(),pstepD=dest.planestep();
  //const T* planeS = src.top_left_ptr();
        T* planeD = dest.top_left_ptr();

  unsigned int min_ni = ni, min_nj = nj;
  while (min_ni%2==0) min_ni /= 2;
  while (min_nj%2==0) min_nj /= 2;
  T* temp = new T[ni>nj?ni:nj];
  for (unsigned p=0;p<np;++p,planeD+=pstepD)
  {
    //const T* rowS = planeS;
          T* rowD = planeD;
    for (unsigned j=0;j<nj;++j,rowD+=jstepD)
    {
      for (unsigned int cni = min_ni; cni < ni; cni *= 2){
        T* temp_ptr = temp;
        const T* temp_src = rowD;
        for (unsigned int i=0;i<2*cni;++i, ++temp_ptr, temp_src+=istepD)
          *temp_ptr = *temp_src;

        bil_harr_inv_helper(temp,rowD,1,istepD,cni);
      }
    }

    //const T* colS = planeD;
          T* colD = planeD;
    for (unsigned i=0;i<ni;++i,colD+=istepD)
    {
      for (unsigned int cnj = min_nj; cnj < nj; cnj *= 2){
        T* temp_ptr = temp;
        const T* temp_src = colD;
        for (unsigned int j=0;j<2*cnj;++j, ++temp_ptr, temp_src+=jstepD)
          *temp_ptr = *temp_src;
        bil_harr_inv_helper(temp,colD,1,jstepD,cnj);
      }
    }
  }
  delete [] temp;
}


#define BIL_HARR_WAVELET_TRANSFORM_INSTANTIATE(T) \
template void \
bil_harr_wavelet_transform< T >(const vil_image_view< T >& src,\
                                      vil_image_view< T >& dest); \
template void \
bil_harr_wavelet_inverse< T >(const vil_image_view< T >& src,\
                                    vil_image_view< T >& dest)

#endif // bil_harr_wavelet_transform_txx_
