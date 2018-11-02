// This is core/vil/vil_fill.h
#ifndef vil_fill_h_
#define vil_fill_h_
//:
// \file
// \brief Various functions for manipulating image views
// \author Tim Cootes - Manchester

#include <algorithm>
#include <cassert>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Fill view with given value
//  O(size).
// \relatesalso vil_image_view
template<class T>
void vil_fill(vil_image_view<T>& view, T value)
{
  if (view.is_contiguous())
    std::fill(view.begin(), view.end(), value);

  unsigned ni = view.ni();
  std::ptrdiff_t istep=view.istep();
  unsigned nj = view.nj();
  std::ptrdiff_t jstep=view.jstep();
  unsigned np = view.nplanes();
  std::ptrdiff_t pstep = view.planestep();

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
// \sa vil_fill_line(vil_image_view<T>)
template<class T>
void vil_fill_line(T* data, unsigned n, std::ptrdiff_t step, T value)
{
  T* end_data = data + n*step;
  while (data!=end_data) { *data=value; data+=step; }
}


//: Fill line from (ai,aj) to (bi,bj) using Bresenham's algorithm.
// Only modifies first plane.
// \relatesalso vil_image_view
template<class T>
void vil_fill_line(vil_image_view<T> &im,
                   int ai, int aj, int bi, int bj,
                   T value)
{
  if (ai == bi && aj==bj)
  {
    if (im.in_range(ai, aj)) im(ai,aj) = value;
    return;
  }

  int d, x, y, xinc, yinc, incr1, incr2;

  int dx = bi-ai;
  int dy = bj-aj;
  if (dy<0)
  {
    dy=-dy;
    yinc=-1;
  }
  else
    yinc=1;

  if (dx<0)
  {
    dx=-dx;
    xinc=-1;
  }
  else
    xinc=1;

  if (im.in_range(ai, aj)) im(ai, aj)=value;
  x=ai;
  y=aj;
  if (dy<=dx)
  {
    d=(dy<<1)-dx;
    incr1=dy<<1;
    incr2=-((dx-dy)<<1);
    while (x!=bi)
    {
      x+=xinc;
      if (d<0)
        d+=incr1;
      else
      {
        y+=yinc;
        d+=incr2;
      }
      if (im.in_range(x, y)) im(x, y)=value;
    }
  }
  else
  {
    d=(dx<<1)-dy;
    incr1=dx<<1;
    incr2=-((dy-dx)<<1);
    while (y!=bj)
    {
      y+=yinc;
      if (d<0)
        d+=incr1;
      else
      {
        x+=xinc;
        d+=incr2;
      }
      if (im.in_range(x, y)) im(x, y)=value;
    }
  }
}

//: Fill row j in view with given value
//  O(ni).
// \relatesalso vil_image_view
template<class T>
void vil_fill_row(vil_image_view<T>& view, unsigned j, T value)
{
  unsigned ni = view.ni();      std::ptrdiff_t istep=view.istep();
  assert(j<view.nj());          std::ptrdiff_t jstep=view.jstep();
  unsigned np = view.nplanes(); std::ptrdiff_t pstep=view.planestep();

  T* row = view.top_left_ptr() + j*jstep;
  for (unsigned p=0;p<np;++p,row += pstep)
    vil_fill_line(row,ni,istep,value);
}

//: Fill column i in view with given value
//  O(nj).
// \relatesalso vil_image_view
template<class T>
void vil_fill_col(vil_image_view<T>& view, unsigned i, T value)
{
  assert(i<view.ni());          std::ptrdiff_t istep=view.istep();
  unsigned nj = view.nj();      std::ptrdiff_t jstep=view.jstep();
  unsigned np = view.nplanes(); std::ptrdiff_t pstep=view.planestep();

  T* col_top = view.top_left_ptr() + i*istep;
  for (unsigned p=0;p<np;++p,col_top += pstep)
    vil_fill_line(col_top,nj,jstep,value);
}

//: Writes given value into each pixel of image under the elements of the mask set to b
//  If mask.nplanes()==1 then the same mask is applied to every image plane, otherwise
//  there must be the same number of mask planes as image planes.
//  \relatesalso vil_image_view
template<class srcT>
inline
void vil_fill_mask(vil_image_view<srcT>& image,
                   const vil_image_view<bool>& mask,
                   srcT value, bool b=true)
{
  unsigned ni = image.ni(), nj = image.nj(), np = image.nplanes();
  assert(ni==mask.ni() && nj==mask.nj());
  assert(mask.nplanes()==1 ||  mask.nplanes() ==np);

  std::ptrdiff_t istepA=image.istep(),jstepA=image.jstep(),pstepA = image.planestep();
  std::ptrdiff_t istepB=mask.istep(),jstepB=mask.jstep(),pstepB = mask.planestep();

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
//  \relatesalso vil_image_view
template<class T>
inline
void vil_fill_disk(vil_image_view<T>& image, double ci, double cj, double r, T value)
{
  unsigned ilo = std::max(0,int(ci-r));
  unsigned ihi = std::max(0,std::min(int(image.ni()-1),int(ci+r+1.0)));
  unsigned jlo = std::max(0,int(cj-r));
  unsigned jhi = std::max(0,std::min(int(image.nj()-1),int(cj+r+1.0)));

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
