#ifndef vil2_image_view_maths_h_
#define vil2_image_view_maths_h_
//: \file
//  \brief Various mathematical manipulations of 2D images
//  \author Tim Cootes

#include <vil2/vil2_image_view.h>

//: Sum of elements in plane p of image
// \relates vil2_image_view
template<class imT, class sumT>
inline void vil2_sum(sumT& sum, const vil2_image_view<imT>& im, unsigned p)
{
  const imT* row = im.top_left()+p*im.planestep();
  int xstep = im.xstep(),ystep=im.ystep;
  const imT* row_end = row + im.ny()*im.ystep();
  int row_len = im.nx()*im.xstep();
  sum = 0;
  for (;row!=row_end;row+=ystep)
  {
    const imT* v_end = row + row_len;
	for (const imT* v = row;v!=v_end;v+=xstep) sum+=*v;
  }
}

//: Mean of elements in plane p of image
// \relates vil2_image_view
template<class imT, class sumT>
inline void vil2_mean(sumT& mean, const vil2_image_view<imT>& im, unsigned p)
{
  if (im.size()==0) { mean=0; return; }
  vil2_sum(mean,im,p);
  mean/=(im.nx()*im.ny());
}

//: Sum of squares of elements in plane p of image
// \relates vil2_image_view
template<class imT, class sumT>
inline void vil2_sum_squares(sumT& sum, sumT& sum_sq, const vil2_image_view<imT>& im, unsigned p)
{
  const imT* row = im.top_left()+p*im.planestep();
  int xstep = im.xstep(),ystep=im.ystep;
  const imT* row_end = row + im.ny()*im.ystep();
  int row_len = im.nx()*im.xstep();
  sum = 0; sum_sq = 0;
  for (;row!=row_end;row+=ystep)
  {
    const imT* v_end = row + row_len;
	for (const imT* v = row;v!=v_end;v+=xstep) { sum+=*v; sum_sq+=(*v)*(*v); }
  }
}

//: Mean and variance of elements in plane p of image
// \relates vil2_image_view
template<class imT, class sumT>
inline void vil2_mean_and_variance(sumT& mean, sumT& var, const vil2_image_view<imT>& im, unsigned p)
{
  if (im.size()==0) { mean=0; var=0; return; }
  sumT sum,sum_sq;
  vil2_sum_squares(sum,sum_sq,im,p);
  mean = sum/(im.nx()*im.ny());
  var = sum_sq/(im.nx()*im.ny()) - mean*mean;
}

#endif
