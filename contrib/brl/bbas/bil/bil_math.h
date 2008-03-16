// This is brl/bbas/bil/bil_math.h
#ifndef bil_math_h_
#define bil_math_h_
//:
// \file
// \brief Various mathematical manipulations of 2D images
// \author Matt Leotta

#include <vcl_cassert.h>

//: compute the boolean "and" of two boolean images
inline void bil_math_boolean_and(const vil_image_view<bool>& mask1,
                                 const vil_image_view<bool>& mask2,
                                       vil_image_view<bool>& result)
{
  unsigned ni = mask1.ni(),nj = mask1.nj(),np = mask1.nplanes();
  if (!result) result = vil_image_view<bool>(ni,nj,np);
  assert(mask2.ni()==ni && mask2.nj()==nj && mask2.nplanes()==np);
  assert(result.ni()==ni && result.nj()==nj && result.nplanes()==np);

  vcl_ptrdiff_t istep1=mask1.istep(),jstep1=mask1.jstep(),pstep1=mask1.planestep();
  vcl_ptrdiff_t istep2=mask2.istep(),jstep2=mask2.jstep(),pstep2=mask2.planestep();
  vcl_ptrdiff_t istepR=result.istep(),jstepR=result.jstep(),pstepR=result.planestep();

  const bool* plane1 = mask1.top_left_ptr();
  const bool* plane2 = mask2.top_left_ptr();
  bool*       planeR = result.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane1 += pstep1,plane2 += pstep2,planeR += pstepR)
  {
    const bool* row1 = plane1;
    const bool* row2 = plane2;
    bool*       rowR = planeR;
    for (unsigned j=0;j<nj;++j,row1 += jstep1,row2 += jstep2,rowR += jstepR)
    {
      const bool* pixel1   = row1;
      const bool* pixel2   = row2;
      bool*       pixelR   = rowR;
      for (unsigned i=0;i<ni;++i,pixel1+=istep1,pixel2+=istep2,pixelR+=istepR)
      {
        *pixelR = *pixel1 && *pixel2;
      }
    }
  }
}


//: compute the boolean "or" of two boolean images
inline void bil_math_boolean_or(const vil_image_view<bool>& mask1,
                                const vil_image_view<bool>& mask2,
                                      vil_image_view<bool>& result)
{
  unsigned ni = mask1.ni(),nj = mask1.nj(),np = mask1.nplanes();
  if (!result) result = vil_image_view<bool>(ni,nj,np);
  assert(mask2.ni()==ni && mask2.nj()==nj && mask2.nplanes()==np);
  assert(result.ni()==ni && result.nj()==nj && result.nplanes()==np);

  vcl_ptrdiff_t istep1=mask1.istep(),jstep1=mask1.jstep(),pstep1=mask1.planestep();
  vcl_ptrdiff_t istep2=mask2.istep(),jstep2=mask2.jstep(),pstep2=mask2.planestep();
  vcl_ptrdiff_t istepR=result.istep(),jstepR=result.jstep(),pstepR=result.planestep();

  const bool* plane1 = mask1.top_left_ptr();
  const bool* plane2 = mask2.top_left_ptr();
  bool*       planeR = result.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane1 += pstep1,plane2 += pstep2,planeR += pstepR)
  {
    const bool* row1 = plane1;
    const bool* row2 = plane2;
    bool*       rowR = planeR;
    for (unsigned j=0;j<nj;++j,row1 += jstep1,row2 += jstep2,rowR += jstepR)
    {
      const bool* pixel1   = row1;
      const bool* pixel2   = row2;
      bool*       pixelR   = rowR;
      for (unsigned i=0;i<ni;++i,pixel1+=istep1,pixel2+=istep2,pixelR+=istepR)
      {
        *pixelR = *pixel1 || *pixel2;
      }
    }
  }
}


//: compute the complement of a boolean image
inline void bil_math_boolean_not(const vil_image_view<bool>& mask,
                                       vil_image_view<bool>& inv_mask)
{
  unsigned ni = mask.ni(),nj = mask.nj(),np = mask.nplanes();
  if (!inv_mask) inv_mask = vil_image_view<bool>(ni,nj,np);
  assert(inv_mask.ni()==ni && inv_mask.nj()==nj && inv_mask.nplanes()==np);

  vcl_ptrdiff_t istep1=mask.istep(),jstep1=mask.jstep(),pstep1=mask.planestep();
  vcl_ptrdiff_t istep2=inv_mask.istep(),jstep2=inv_mask.jstep(),pstep2=inv_mask.planestep();

  const bool* plane = mask.top_left_ptr();
  bool*       plane_inv = inv_mask.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep1,plane_inv += pstep2)
  {
    const bool* row = plane;
    bool*       row_inv = plane_inv;
    for (unsigned j=0;j<nj;++j,row += jstep1,row_inv += jstep2)
    {
      const bool* pixel = row;
      bool*       pixel_inv = row_inv;
      for (unsigned i=0;i<ni;++i,pixel+=istep1,pixel_inv+=istep2)
      {
        *pixel_inv = !(*pixel);
      }
    }
  }
}


//: Set the mask pixel to false for each image pixel that is not in the range [min,max]
// \note this differs from vil_threshold in that values in the mask are not modified
//       if for pixels in the range
template<class T>
inline void bil_math_threshold_mask(const vil_image_view<T>& image, T min, T max,
                                          vil_image_view<bool>& mask)
{
  unsigned ni = image.ni(),nj = image.nj();
  assert(image.nplanes()==1);
  assert(mask.ni()==ni && mask.nj()==nj);
  assert(mask.nplanes()==1);

  vcl_ptrdiff_t istepM=mask.istep(),jstepM=mask.jstep();
  vcl_ptrdiff_t istepD=image.istep(),jstepD=image.jstep();
  const T* rowD = image.top_left_ptr();
  bool*    rowM = mask.top_left_ptr();

  for (unsigned j=0;j<nj;++j,rowM += jstepM,rowD += jstepD)
  {
    const T* pixelD = rowD;
    bool*    pixelM = rowM;
    for (unsigned i=0;i<ni;++i,pixelM+=istepM,pixelD+=istepD)
    {
      if (*pixelD < min || *pixelD > max)
        *pixelM = false;
    }
  }
}


#endif // bil_math_h_
