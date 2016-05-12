#ifndef vil_threshold_hxx_
#define vil_threshold_hxx_
//:
// \file
// \brief Apply thresholds to image data
// \author Tim Cootes

#include "vil_threshold.h"

//: Apply threshold such that dest(i,j,p)=true if src(i,j,p)>=t
template<class srcT>
void vil_threshold_above(const vil_image_view<srcT>& src,
                         vil_image_view<bool>& dest,  srcT t)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,np);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),pstepB = dest.planestep();
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
void vil_threshold_below(const vil_image_view<srcT>& src,
                         vil_image_view<bool>& dest,  srcT t)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,np);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),pstepB = dest.planestep();
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
void vil_threshold_inside(const vil_image_view<srcT>& src,
                          vil_image_view<bool>& dest,  srcT t0, srcT t1)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,np);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),pstepB = dest.planestep();
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
void vil_threshold_outside(const vil_image_view<srcT>& src,
                           vil_image_view<bool>& dest,  srcT t0, srcT t1)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,np);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),pstepB = dest.planestep();
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

#undef VIL_THRESHOLD_INSTANTIATE
#define VIL_THRESHOLD_INSTANTIATE(srcT) \
template void vil_threshold_above(const vil_image_view<srcT >& src, \
                                  vil_image_view<bool >& dest,  srcT t); \
template void vil_threshold_below(const vil_image_view<srcT >& src, \
                                  vil_image_view<bool >& dest,  srcT t); \
template void vil_threshold_inside(const vil_image_view<srcT >& src, \
                                   vil_image_view<bool >& dest,  srcT t0, srcT t1); \
template void vil_threshold_outside(const vil_image_view<srcT >& src, \
                                    vil_image_view<bool >& dest,  srcT t0, srcT t1)

#endif // vil_threshold_hxx_
