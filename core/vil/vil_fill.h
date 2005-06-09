// This is core/vil/vil_fill.h
#ifndef vil_fill_h_
#define vil_fill_h_
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <vcl_cassert.h>
#include <vil/vil_image_view.h>
#include <vcl_algorithm.h>

//: Fill view with given value
//  O(size).
// \relates vil_image_view
template<class T>
void vil_fill(vil_image_view<T>& view, T value)
{

  if (view.is_contiguous())
    vcl_fill(view.begin(), view.end(), value);

  unsigned ni = view.ni(), istep=view.istep();
  unsigned nj = view.nj(), jstep=view.jstep();
  unsigned np = view.nplanes(), pstep = view.planestep();

  T* plane = view.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    T* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      T* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep) *pixel=value;
    }
  }
}

//: Fill data[i*step] (i=0..n-1) with given value
// \relates vil_image_view
template<class T>
void vil_fill_line(T* data, unsigned n, vcl_ptrdiff_t step, T value)
{
  T* end_data = data + n*step;
  while (data!=end_data) { *data=value; data+=step; }
}

//: Fill row j in view with given value
//  O(ni).
// \relates vil_image_view
template<class T>
void vil_fill_row(vil_image_view<T>& view, unsigned j, T value)
{
  unsigned ni = view.ni();      vcl_ptrdiff_t istep=view.istep();
  assert(j<view.nj());          vcl_ptrdiff_t jstep=view.jstep();
  unsigned np = view.nplanes(); vcl_ptrdiff_t pstep=view.planestep();

  T* row = view.top_left_ptr() + j*jstep;
  for (unsigned p=0;p<np;++p,row += pstep)
    vil_fill_line(row,ni,istep,value);
}

//: Fill column i in view with given value
//  O(nj).
// \relates vil_image_view
template<class T>
void vil_fill_col(vil_image_view<T>& view, unsigned i, T value)
{
  assert(i<view.ni());          vcl_ptrdiff_t istep=view.istep();
  unsigned nj = view.nj();      vcl_ptrdiff_t jstep=view.jstep();
  unsigned np = view.nplanes(); vcl_ptrdiff_t pstep=view.planestep();
 
  T* col_top = view.top_left_ptr() + i*istep;
  for (unsigned p=0;p<np;++p,col_top += pstep)
    vil_fill_line(col_top,nj,jstep,value);
}

//: Writes given value into each pixel of image under the elements of the mask set to b
//  If mask.nplanes()==1 then the same mask is applied to every image plane, otherwise
//  there must be the same number of mask planes as image planes.
template<class srcT>
inline
void vil_fill_mask(vil_image_view<srcT>& image,
                   const vil_image_view<bool>& mask,
                   srcT value, bool b=true)
{
  unsigned ni = image.ni(), nj = image.nj(), np = image.nplanes();
  assert(ni==mask.ni() && nj==mask.nj());
  assert(mask.nplanes()==1 ||  mask.nplanes() ==np);

  vcl_ptrdiff_t istepA=image.istep(),jstepA=image.jstep(),pstepA = image.planestep();
  vcl_ptrdiff_t istepB=mask.istep(),jstepB=mask.jstep(),pstepB = mask.planestep();

  // If only one mask plane, apply to all image planes
  // Setting pstepB to 0 ensures that the same mask is used for each pass
  if (mask.nplanes()==1) pstepB=0;

  srcT* planeA = image.top_left_ptr();
  const bool* planeB = mask.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    srcT* rowA   = planeA;
    const bool* rowB   = planeB;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
    {
      srcT* pixelA = rowA;
      const bool* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
        if (*pixelB==b) *pixelA=value;
    }
  }
}

//: Fills pixels in disk with centre (ci,cj), radius r, with given value
//  Fills all planes of image with the value.
template<class T>
inline
void vil_fill_disk(vil_image_view<T>& image, double ci, double cj, double r, T value)
{
  int ilo = vcl_max(0,int(ci-r));
  int ihi = vcl_min(int(image.ni()-1),int(ci+r+1.0));
  int jlo = vcl_max(0,int(cj-r));
  int jhi = vcl_min(int(image.nj()-1),int(cj+r+1.0));

  double r2 = r*r;
  for (unsigned j=jlo;j<=jhi;++j)
  {
    double t2 = r2 - (j-cj)*(j-cj);
    for (unsigned i=ilo;i<=ihi;++i)
    {
      if ((i-ci)*(i-ci)<t2)
      {
        for (unsigned k=0;k<image.nplanes();++k) image(i,j,k)=value;
      }
    }
  }
}

#endif // vil_fill_h_
