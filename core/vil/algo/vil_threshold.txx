#ifndef vil2_threshold_txx_
#define vil2_threshold_txx_
//:
//  \file
//  \brief Apply thresholds to image data
//  \author Tim Cootes

#include "vil2_threshold.h"

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)>=t
template<class srcT>
void vil2_threshold_above(const vil2_image_view<srcT>& src,
                          vil2_image_view<bool>& dest,  srcT t)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,np);

  vcl_ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  vcl_ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),pstepB = dest.planestep();
  const srcT* planeA = src.top_left_ptr();
  bool* planeB = dest.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const srcT* rowA   = planeA;
    bool* rowB   = planeB;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
    {
      const srcT* pixelA = rowA;
      bool* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
        *pixelB = *pixelA>=t;
    }
  }
}

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)<=t
template<class srcT>
void vil2_threshold_below(const vil2_image_view<srcT>& src,
                          vil2_image_view<bool>& dest,  srcT t)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,np);

  vcl_ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  vcl_ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),pstepB = dest.planestep();
  const srcT* planeA = src.top_left_ptr();
  bool* planeB = dest.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const srcT* rowA   = planeA;
    bool* rowB   = planeB;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
    {
      const srcT* pixelA = rowA;
      bool* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
        *pixelB = *pixelA<=t;
    }
  }
}
//: Apply threshold such that dest(i,j,p)=true if t0<=src(i,j,p)<=t1
template<class srcT>
void vil2_threshold_inside(const vil2_image_view<srcT>& src,
                          vil2_image_view<bool>& dest,  srcT t0, srcT t1)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,np);

  vcl_ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  vcl_ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),pstepB = dest.planestep();
  const srcT* planeA = src.top_left_ptr();
  bool* planeB = dest.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const srcT* rowA   = planeA;
    bool* rowB   = planeB;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
    {
      const srcT* pixelA = rowA;
      bool* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
        *pixelB = (t0<=*pixelA) && (*pixelA<=t1);
    }
  }
}

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)<=t0 or src(i,j,p)>=t1
template<class srcT>
void vil2_threshold_outside(const vil2_image_view<srcT>& src,
                          vil2_image_view<bool>& dest,  srcT t0, srcT t1)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,np);

  vcl_ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  vcl_ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),pstepB = dest.planestep();
  const srcT* planeA = src.top_left_ptr();
  bool* planeB = dest.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const srcT* rowA   = planeA;
    bool* rowB   = planeB;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
    {
      const srcT* pixelA = rowA;
      bool* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
        *pixelB = (*pixelA<=t0) || (*pixelA>=t1);
    }
  }
}

#undef VIL2_THRESHOLD_INSTANTIATE
#define VIL2_THRESHOLD_INSTANTIATE(srcT) \
template void vil2_threshold_above(const vil2_image_view<srcT>& src, \
                          vil2_image_view<bool>& dest,  srcT t); \
template void vil2_threshold_below(const vil2_image_view<srcT>& src, \
                          vil2_image_view<bool>& dest,  srcT t); \
template void vil2_threshold_inside(const vil2_image_view<srcT>& src, \
                          vil2_image_view<bool>& dest,  srcT t0, srcT t1); \
template void vil2_threshold_outside(const vil2_image_view<srcT>& src, \
                          vil2_image_view<bool>& dest,  srcT t0, srcT t1)


#endif
