
#ifndef bil_finite_second_differences_txx_
#define bil_finite_second_differences_txx_

#include "bil_finite_second_differences.h"

#define IMVAL(im,i,j,p,ni,nj) (im(static_cast<int>(i)<0?0:(i>=ni?ni-1:i),static_cast<int>(j)<0?0:(j>=nj?nj-1:j),p))
#define DX 1
#define DY 1
template <class T>
void bil_finite_second_differences(const vil_image_view<T>& src_im,
                                   const vil_image_view<T>& dxp,
                                   const vil_image_view<T>& dxm,
                                   const vil_image_view<T>& dyp,
                                   const vil_image_view<T>& dym,
                                   vil_image_view<T>& dxx,
                                   vil_image_view<T>& dyy,
                                   vil_image_view<T>& dxy)
{
  int ni = src_im.ni();
  int nj = src_im.nj();
  unsigned n_planes = src_im.nplanes();

  dxx.set_size(ni,nj,n_planes);
  dyy.set_size(ni,nj,n_planes);
  dxy.set_size(ni,nj,n_planes);

  for (unsigned p=0;p<n_planes;++p)
  {
    // ints for now because negative indices are used inside the loop
    for (int j=0;j<static_cast<int>(nj);++j){
      for (int i=0;i<static_cast<int>(ni);++i){
        dxx(i,j,p) = ( dxp(i,j,p) - dxm(i,j,p) )/(DX*DX);
        dyy(i,j,p) = ( dyp(i,j,p) - dym(i,j,p) )/(DY*DY);
        dxy(i,j,p) = ( IMVAL(src_im,i+1,j+1,p,ni,nj) + IMVAL(src_im,i-1,j-1,p,ni,nj)
                      -IMVAL(src_im,i-1,j+1,p,ni,nj) - IMVAL(src_im,i+1,j-1,p,ni,nj))/(4*DX*DY);
      }
    }
  }
}
#undef IMVAL
#undef DX
#undef DY


#undef BIL_FINITE_SECOND_DIFFERENCES_INSTANTIATE
#define BIL_FINITE_SECOND_DIFFERENCES_INSTANTIATE(T) \
template void bil_finite_second_differences(const vil_image_view<T >& src_im,\
                                            const vil_image_view<T >& dxp_,\
                                            const vil_image_view<T >& dxm_,\
                                            const vil_image_view<T >& dyp_,\
                                            const vil_image_view<T >& dym_,\
                                            vil_image_view<T >& dxx_,\
                                            vil_image_view<T >& dyy_,\
                                            vil_image_view<T >& dxy_)

#endif // bil_finite_second_differences_txx_
