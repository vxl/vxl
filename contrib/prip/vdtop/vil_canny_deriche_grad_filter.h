// This is prip/vdtop/vil_canny_deriche_grad_filter.h
#ifndef vil_canny_deriche_grad_filter_h_
#define vil_canny_deriche_grad_filter_h_

//:
// \file
// \brief Function computing canny deriche gradient.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vil/vil_image_view.h>
#include <vcl_cmath.h>

//: the canny_deriche filter for 1d pass.
template <class srcT, class destT, class accumT>
inline void vil_canny_deriche_base_filter_1d(const srcT* src, vcl_ptrdiff_t sstep,
                                              destT* dest, vcl_ptrdiff_t dstep,
                                              int n, accumT a1, accumT a2, accumT a3, accumT a4, accumT b1, accumT b2, accumT c1)
 {
   const srcT* s = src, *p=src;
   const srcT* src_end = src + (n-1)*sstep;
   accumT y2=0, y1=0, y =0 ;

   // Forward pass
   while (s!=src_end)
   {
      y=a1* *s+a2* *p+b1*y1+b2*y2 ; // y+(x) = a1 I(x)+a2 I(x-1)+b1 y+(x-1)+ b2 y+(x-2)
     *dest = (destT)(y);
     y2=y1 ; y1=y ; p=s ; s+=sstep; dest+=dstep;  // Move to next element
   }

   // Backward pass
   y2=0; y1=0; y =0;
   p=s ; s-=sstep;
   dest[0]=0; dest-=dstep;dest-=dstep;
   src_end = src;
   while (s!=src_end)
   {
     y=a3* *s+a4* *p+b1*y1+b2*y2 ;  // y+(x) = a3 I(x-1)+a4 I(x-2)+b1 y+(x-1)+ b2 y+(x-2)
     *dest = (destT)(c1*((accumT)*dest + y)); // y(x) = a (y+(x) - y-(x))
     y2=y1 ; y1=y ; p=s ; s-=sstep; dest-=dstep;  // Move to next element
   }
 }

//: the canny deriche filter.
template <class srcT, class destT, class accumT>
inline void vil_canny_deriche_grad_filter(const vil_image_view<srcT>& src_im,
                                   vil_image_view<destT>& grad_i,
                                   vil_image_view<destT>& grad_j,
                                   accumT alpha)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  vil_image_view<accumT> tmp ;
  tmp.set_size(ni,nj,src_im.nplanes());
  grad_i.set_size(ni,nj,src_im.nplanes());
  grad_j.set_size(ni,nj,src_im.nplanes());
  vcl_ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();
  vcl_ptrdiff_t t_istep = tmp.istep(), t_jstep = tmp.jstep();
  vcl_ptrdiff_t gi_istep = grad_i.istep(), gi_jstep = grad_i.jstep();
  vcl_ptrdiff_t gj_istep = grad_j.istep(), gj_jstep = grad_j.jstep();

  accumT e = vcl_exp(-alpha) ;
  accumT me2 = (1.0-e)*(1.0-e) ;
  accumT b1 = 2.0*e , b2 = -e*e ;
  accumT k = me2/(1+2*alpha*e-e*e) ;
  accumT a1=0,a2=1,a3=-1,a4=0,c1=-me2,
        a5=k,a6=k*e*(alpha-1),a7=k*e*(alpha+1),a8=-k*e*e,c2=1 ;

  for (unsigned p=0;p<src_im.nplanes();++p)
  {
    const srcT*  src_row  = src_im.top_left_ptr()+p*src_im.planestep() ;
    accumT* tmp_row = tmp.top_left_ptr()+p*tmp.planestep() ;
    // Filter every row
    for (unsigned j=0;j<nj;++j,src_row+=s_jstep,tmp_row+=t_jstep)
      vil_canny_deriche_base_filter_1d(src_row,s_istep, tmp_row,t_istep, ni, a1, a2, a3, a4, b1, b2, c1);

    tmp_row = tmp.top_left_ptr()+p*tmp.planestep() ;
    destT* g_col = grad_i.top_left_ptr()+p*grad_i.planestep() ;
    // Filter every column
    for (unsigned j=0;j<nj;++j,tmp_row+=t_istep,g_col+=gi_istep)
      vil_canny_deriche_base_filter_1d(tmp_row,t_jstep, g_col,gi_jstep, nj, a5, a6, a7, a8, b1, b2, c2);

    src_row  = src_im.top_left_ptr()+p*src_im.planestep() ;
    tmp_row = tmp.top_left_ptr()+p*tmp.planestep() ;
    // Filter every row
    for (unsigned j=0;j<nj;++j,src_row+=s_jstep,tmp_row+=t_jstep)
      vil_canny_deriche_base_filter_1d(src_row,s_istep, tmp_row,t_istep, ni, a5, a6, a7, a8, b1, b2, c2);

    tmp_row = tmp.top_left_ptr()+p*tmp.planestep() ;
    g_col = grad_j.top_left_ptr()+p*grad_j.planestep() ;
    // Filter every column
    for (unsigned j=0;j<nj;++j,tmp_row+=t_istep,g_col+=gj_istep)
      vil_canny_deriche_base_filter_1d(tmp_row,t_jstep, g_col,gj_jstep, nj, a1, a2, a3, a4, b1, b2, c1);
  }
}

//: the canny_deriche filter. Dest receive the magnitude of the gradient.
template <class T1, class T2,class AccumT>
void vil_canny_deriche_grad_filter(const vil_image_view<T1> & src,vil_image_view<T2> & dest, AccumT alpha)
{
  vil_image_view<T2> gradj ;
  vil_canny_deriche_grad_filter(src,dest,gradj, alpha) ;
  typename vil_image_view<T2>::iterator i1=dest.begin(), i2=gradj.begin() ;
  for (;i1!=dest.end(); ++i1, ++i2)
  {
    *i1 = (*i1)*(*i1)+(*i2)*(*i2) ;
  }
  vil_math_sqrt(dest) ;
}

#endif
