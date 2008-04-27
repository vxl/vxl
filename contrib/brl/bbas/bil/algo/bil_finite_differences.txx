#ifndef bil_finite_differences_txx_
#define bil_finite_differences_txx_

#include "bil_finite_differences.h"

#define IMVAL(im,i,j,p,ni,nj) T(im(i<0?0:(i>=ni?ni-1:i),j<0?0:(j>=nj?nj-1:j),p))
#define DX 1
#define DY 1

template <class T>
void bil_finite_differences(const vil_image_view<T>& src_im,
                            vil_image_view<T>& dxp,
                            vil_image_view<T>& dxm,
                            vil_image_view<T>& dxc,
                            vil_image_view<T>& dyp,
                            vil_image_view<T>& dym,
                            vil_image_view<T>& dyc)
{
  int ni = src_im.ni();
  int nj = src_im.nj();
  unsigned n_planes = src_im.nplanes();

  dxp.set_size(ni,nj,n_planes);
  dxm.set_size(ni,nj,n_planes);
  dxc.set_size(ni,nj,n_planes);
  dyp.set_size(ni,nj,n_planes);
  dym.set_size(ni,nj,n_planes);
  dyc.set_size(ni,nj,n_planes);

  T currentPixel;
  for (unsigned p=0;p<n_planes;++p){
      for (int j=0;j<static_cast<int>(nj);++j){
          for (int i=0;i<static_cast<int>(ni);++i){
              currentPixel = src_im(i,j,p);
              dxp(i,j,p) = (IMVAL(src_im,i+1,j,p,ni,nj) - currentPixel)/DX;
              dxm(i,j,p) = (currentPixel - IMVAL(src_im,i-1,j,p,ni,nj))/DX;
              dxc(i,j,p) = (IMVAL(src_im,i+1,j,p,ni,nj) - IMVAL(src_im,i-1,j,p,ni,nj))/T(2*DX);
              dyp(i,j,p) = (IMVAL(src_im,i,j+1,p,ni,nj) - currentPixel)/DY;
              dym(i,j,p) = (currentPixel - IMVAL(src_im,i,j-1,p,ni,nj))/DY;
              dyc(i,j,p) = (IMVAL(src_im,i,j+1,p,ni,nj) - IMVAL(src_im,i,j-1,p,ni,nj))/T(2*DY);
          }
      }
  }
}
#undef IMVAL
#undef DX
#undef DY

#undef BIL_FINITE_DIFFERENCES_INSTANTIATE
#define BIL_FINITE_DIFFERENCES_INSTANTIATE(T) \
template void bil_finite_differences(const vil_image_view<T >& src_im, \
                                     vil_image_view<T >& dxp, \
                                     vil_image_view<T >& dxm, \
                                     vil_image_view<T >& dxc, \
                                     vil_image_view<T >& dyp, \
                                     vil_image_view<T >& dym, \
                                     vil_image_view<T >& dyc)

#endif // bil_finite_differences_txx_
