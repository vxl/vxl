#ifndef vil2_math_h_
#define vil2_math_h_
//: \file
//  \brief Various mathematical manipulations of 2D images
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>

//: Sum of elements in plane p of image
// \relates vil2_image_view
template<class imT, class sumT>
inline void vil2_math_sum(sumT& sum, const vil2_image_view<imT>& im, unsigned p)
{
  const imT* row = im.top_left_ptr()+p*im.planestep();
  int istep = im.istep(),jstep=im.jstep();
  const imT* row_end = row + im.nj()*jstep;
  int row_len = im.ni()*im.istep();
  sum = 0;
  for (;row!=row_end;row+=jstep)
  {
    const imT* v_end = row + row_len;
    for (const imT* v = row;v!=v_end;v+=istep) sum+=*v;
  }
}

//: Mean of elements in plane p of image
// \relates vil2_image_view
template<class imT, class sumT>
inline void vil2_math_mean(sumT& mean, const vil2_image_view<imT>& im, unsigned p)
{
  if (im.size()==0) { mean=0; return; }
  vil2_math_sum(mean,im,p);
  mean/=(im.ni()*im.nj());
}

//: Sum of squares of elements in plane p of image
// \relates vil2_image_view
template<class imT, class sumT>
inline void vil2_math_sum_squares(sumT& sum, sumT& sum_sq, const vil2_image_view<imT>& im, unsigned p)
{
  const imT* row = im.top_left_ptr()+p*im.planestep();
  int istep = im.istep(),jstep=im.jstep();
  const imT* row_end = row + im.nj()*jstep;
  int row_len = im.ni()*im.istep();
  sum = 0; sum_sq = 0;
  for (;row!=row_end;row+=jstep)
  {
    const imT* v_end = row + row_len;
    for (const imT* v = row;v!=v_end;v+=istep) { sum+=*v; sum_sq+=(*v)*(*v); }
  }
}

//: Mean and variance of elements in plane p of image
// \relates vil2_image_view
template<class imT, class sumT>
inline void vil2_math_mean_and_variance(sumT& mean, sumT& var, const vil2_image_view<imT>& im, unsigned p)
{
  if (im.size()==0) { mean=0; var=0; return; }
  sumT sum,sum_sq;
  vil2_math_sum_squares(sum,sum_sq,im,p);
  mean = sum/(im.ni()*im.nj());
  var = sum_sq/(im.ni()*im.nj()) - mean*mean;
}

//: Multiply values in image view by scale
// \relates vil2_image_view
template<class T>
void vil2_math_scale_values(vil2_image_view<T>& image, double scale)
{
  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  int istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
  T* plane = image.top_left_ptr();
  for (int p=0;p<np;++p,plane += pstep)
  {
    T* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      T* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep) *pixel = T(scale*(*pixel));
    }
  }
}

//: Multiply values in image view by scale and add offset
// \relates vil2_image_view
template<class imT, class offsetT>
void vil2_math_scale_and_offset_values(vil2_image_view<imT>& image, double scale, offsetT offset)
{
  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  int istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
  imT* plane = image.top_left_ptr();
  for (int p=0;p<np;++p,plane += pstep)
  {
    imT* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      imT* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep) *pixel = imT(scale*(*pixel)+offset);
    }
  }
}

//: Compute sum of two images (im_sum = imA+imB)
// \relates vil2_image_view
template<class aT, class bT, class sumT>
void vil2_math_image_sum(const vil2_image_view<aT>& imA,
                         const vil2_image_view<bT>& imB,
                         vil2_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_sum.resize(ni,nj,np);

  int istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  int istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  int istepS=im_sum.istep(),jstepS=im_sum.jstep(),pstepS = im_sum.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  sumT* planeS     = im_sum.top_left_ptr();
  for (int p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    sumT* rowS = planeS;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowS += jstepS)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      sumT* pixelS = rowS;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelS+=istepS)
        *pixelS = sumT(*pixelA)+sumT(*pixelB);
    }
  }
}

//: Compute difference of two images (im_sum = imA-imB)
// \relates vil2_image_view
template<class aT, class bT, class sumT>
void vil2_math_image_difference(const vil2_image_view<aT>& imA,
                                const vil2_image_view<bT>& imB,
                                vil2_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_sum.resize(ni,nj,np);

  int istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  int istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  int istepS=im_sum.istep(),jstepS=im_sum.jstep(),pstepS = im_sum.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  sumT* planeS     = im_sum.top_left_ptr();
  for (int p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    sumT* rowS = planeS;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowS += jstepS)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      sumT* pixelS = rowS;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelS+=istepS)
        *pixelS = sumT(*pixelA)-sumT(*pixelB);
    }
  }
}

//: Compute absolute difference of two images (im_sum = |imA-imB|)
// \relates vil2_image_view
template<class aT, class bT, class sumT>
void vil2_math_image_abs_difference(const vil2_image_view<aT>& imA,
                                    const vil2_image_view<bT>& imB,
                                    vil2_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_sum.resize(ni,nj,np);

  int istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  int istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  int istepS=im_sum.istep(),jstepS=im_sum.jstep(),pstepS = im_sum.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  sumT* planeS     = im_sum.top_left_ptr();
  for (int p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    sumT* rowS = planeS;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowS += jstepS)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      sumT* pixelS = rowS;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelS+=istepS)
        *pixelS = vcl_fabs(sumT(*pixelA)-sumT(*pixelB));
    }
  }
}

//: imA = fa*imA + fb*imB  (Useful for moving averages!)
// Can do running sum using vil2_add_image_fraction(running_mean,1-f,new_im,f)
// to update current mean by a fraction f of new_im
// \relates vil2_image_view
template<class aT, class bT, class scaleT>
void vil2_math_add_image_fraction(vil2_image_view<aT>& imA, scaleT fa,
                                  const vil2_image_view<bT>& imB, scaleT fb)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);

  int istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  int istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  for (int p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    aT* rowA   = planeA;
    const bT* rowB   = planeB;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
    {
      aT* pixelA = rowA;
      const bT* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
        *pixelA = aT(fa*(*pixelA)+fb*(*pixelB));
    }
  }
}

//: Compute integral image im_sum(i,j) = sum (x<=i,y<=j) imA(x,y)
//  Useful thing for quickly computing mean over large regions,
//  as demonstrated in Viola and Jones (CVPR01).
// The sum of elements in the ni x nj square with corner (i,j)
// is given by im_sum(i,j)+im_sum(i+ni-1,j+nj-1)-im_sum(i+ni-1,j)-im_sum(i,j+nj-1)
// \relates vil2_image_view
template<class aT, class sumT>
void vil2_math_integral_image(const vil2_image_view<aT>& imA,
                              vil2_image_view<sumT>& im_sum)
{
  assert(imA.nplanes()==1);
  unsigned ni = imA.ni(),nj = imA.nj();
  im_sum.resize(ni,nj,1);

  int istepA=imA.istep(),jstepA=imA.jstep();
  int istepS=im_sum.istep(),jstepS=im_sum.jstep();
  const aT* rowA = imA.top_left_ptr();
  sumT* rowS     = im_sum.top_left_ptr();

  // Compute running sum of first row
  sumT sum=0;
  const aT* pixelA = rowA;
  sumT* pixelS = rowS;
  for (int i=0;i<ni;++i,pixelA+=istepA,pixelS+=istepS)
    { sum+= *pixelA; *pixelS=sum; }

  // For subsequent rows, include sum from row above as well
  int prev_j = -jstepS;
  rowA += jstepA; rowS += jstepS;
  for (unsigned j=1;j<nj;++j,rowA += jstepA,rowS += jstepS)
  {
    pixelA = rowA;
    pixelS = rowS;
    sum = 0;
    for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelS+=istepS)
    { sum+= *pixelA; *pixelS=sum + pixelS[prev_j];}
  }
}

//: Compute integral image im_sum_sq(i,j) = sum (x<=i,y<=j) imA(x,y)^2
// Also computes sum im_sum(i,j) = sum (x<=i,y<=j) imA(x,y)
//
//  Useful thing for quickly computing mean and variance over large regions,
//  as demonstrated in Viola and Jones (CVPR01).
//
// The sum of elements in the ni x nj square with corner (i,j)
// is given by im_sum(i,j)+im_sum(i+ni-1,j+nj-1)-im_sum(i+ni-1,j)-im_sum(i,j+nj-1)
//
// Similar result holds for sum of squares, allowing rapid calculation of variance etc.
// \relates vil2_image_view
template<class aT, class sumT>
void vil2_math_integral_sqr_image(const vil2_image_view<aT>& imA,
                                  vil2_image_view<sumT>& im_sum,
                                  vil2_image_view<sumT>& im_sum_sq)
{
  assert(imA.nplanes()==1);
  unsigned ni = imA.ni(),nj = imA.nj();
  im_sum.resize(ni,nj,1);
  im_sum_sq.resize(ni,nj,1);

  int istepA=imA.istep(),jstepA=imA.jstep();
  int istepS=im_sum.istep(),jstepS=im_sum.jstep();
  int istepS2=im_sum_sq.istep(),jstepS2=im_sum_sq.jstep();
  const aT* rowA = imA.top_left_ptr();
  sumT* rowS     = im_sum.top_left_ptr();
  sumT* rowS2    = im_sum_sq.top_left_ptr();

  // Compute running sum of first row
  sumT sum=0,sum2=0;
  const aT* pixelA = rowA;
  sumT* pixelS     = rowS;
  sumT* pixelS2    = rowS2;
  for (int i=0;i<ni;++i,pixelA+=istepA,pixelS+=istepS,pixelS2+=istepS2)
  {
    sum+= *pixelA; *pixelS=sum;
    sum2+=sumT(*pixelA)*sumT(*pixelA); *pixelS2=sum2;
  }

  // For subsequent rows, include sum from row above as well
  int prev_j = -jstepS;
  int prev_j2 = -jstepS2;
  rowA += jstepA; rowS += jstepS; rowS2 += jstepS2;
  for (unsigned j=1;j<nj;++j,rowA += jstepA,rowS += jstepS,rowS2 += jstepS2)
  {
    pixelA  = rowA;
    pixelS  = rowS;
    pixelS2 = rowS2;
    sum = 0; sum2 = 0;
    for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelS+=istepS,pixelS2+=istepS2)
    {
      sum+= *pixelA;
      *pixelS=sum + pixelS[prev_j];
      sum2+=sumT(*pixelA)*sumT(*pixelA);
      *pixelS2 = sum2 + pixelS2[prev_j2];
    }
  }
}

#endif // vil2_math_h_
