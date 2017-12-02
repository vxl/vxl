#ifndef vil3d_threshold_hxx_
#define vil3d_threshold_hxx_
//:
// \file
// \brief Apply thresholds to image data
// \author Tim Cootes

#include "vil3d_threshold.h"

//: Apply threshold such that dest(i,j,k,p)=true if src(i,j,k,p)>=t
template<class srcT>
void vil3d_threshold_above(const vil3d_image_view<srcT>& src,
                           vil3d_image_view<bool>& dest,  srcT t)
{
  unsigned ni = src.ni(),nj = src.nj(),nk = src.nk(),np = src.nplanes();
  dest.set_size(ni,nj,nk,np);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),kstepA=src.kstep();
  std::ptrdiff_t pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),kstepB=dest.kstep();
  std::ptrdiff_t pstepB = dest.planestep();
  const srcT* planeA = src.origin_ptr();
  bool* planeB = dest.origin_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const srcT* sliceA = planeA;
    bool* sliceB = planeB;
    for (unsigned k=0;k<nk;++k,sliceA += kstepA,sliceB += kstepB)
    {
      const srcT* rowA   = sliceA;
      bool* rowB   = sliceB;
      for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
      {
        const srcT* pixelA = rowA;
        bool* pixelB = rowB;
        for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
          *pixelB = *pixelA>=t;
      }
    }
  }
}

//: Apply threshold such that dest(i,j,k,p)=true if src(i,j,k,p)<=t
template<class srcT>
void vil3d_threshold_below(const vil3d_image_view<srcT>& src,
                           vil3d_image_view<bool>& dest,  srcT t)
{
  unsigned ni = src.ni(),nj = src.nj(),nk = src.nk(),np = src.nplanes();
  dest.set_size(ni,nj,nk,np);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),kstepA=src.kstep();
  std::ptrdiff_t pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),kstepB=dest.kstep();
  std::ptrdiff_t pstepB = dest.planestep();
  const srcT* planeA = src.origin_ptr();
  bool* planeB = dest.origin_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const srcT* sliceA = planeA;
    bool* sliceB = planeB;
    for (unsigned k=0;k<nk;++k,sliceA += kstepA,sliceB += kstepB)
    {
      const srcT* rowA   = sliceA;
      bool* rowB   = sliceB;
      for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
      {
        const srcT* pixelA = rowA;
        bool* pixelB = rowB;
        for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
          *pixelB = *pixelA<=t;
      }
    }
  }
}

//: Apply threshold such that dest(i,j,k,p)=true if t0<=src(i,j,k,p)<=t1
template<class srcT>
void vil3d_threshold_inside(const vil3d_image_view<srcT>& src,
                            vil3d_image_view<bool>& dest,  srcT t0, srcT t1)
{
  unsigned ni = src.ni(),nj = src.nj(),nk = src.nk(),np = src.nplanes();
  dest.set_size(ni,nj,nk,np);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),kstepA=src.kstep();
  std::ptrdiff_t pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),kstepB=dest.kstep();
  std::ptrdiff_t pstepB = dest.planestep();
  const srcT* planeA = src.origin_ptr();
  bool* planeB = dest.origin_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const srcT* sliceA = planeA;
    bool* sliceB = planeB;
    for (unsigned k=0;k<nk;++k,sliceA += kstepA,sliceB += kstepB)
    {
      const srcT* rowA   = sliceA;
      bool* rowB   = sliceB;
      for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
      {
        const srcT* pixelA = rowA;
        bool* pixelB = rowB;
        for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
          *pixelB = (t0<=*pixelA) && (*pixelA<=t1);
      }
    }
  }
}

//: Apply threshold such that dest(i,j,k,p)=true if src(i,j,k,p)<=t0 or src(i,j,k,p)>=t1
template<class srcT>
void vil3d_threshold_outside(const vil3d_image_view<srcT>& src,
                             vil3d_image_view<bool>& dest,  srcT t0, srcT t1)
{
  unsigned ni = src.ni(),nj = src.nj(),nk = src.nk(),np = src.nplanes();
  dest.set_size(ni,nj,nk,np);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),kstepA=src.kstep();
  std::ptrdiff_t pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep(),kstepB=dest.kstep();
  std::ptrdiff_t pstepB = dest.planestep();
  const srcT* planeA = src.origin_ptr();
  bool* planeB = dest.origin_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const srcT* sliceA = planeA;
    bool* sliceB = planeB;
    for (unsigned k=0;k<nk;++k,sliceA += kstepA,sliceB += kstepB)
    {
      const srcT* rowA   = sliceA;
      bool* rowB   = sliceB;
      for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
      {
        const srcT* pixelA = rowA;
        bool* pixelB = rowB;
        for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
          *pixelB = (*pixelA<=t0) || (*pixelA>=t1);
      }
    }
  }
}

#undef VIL3D_THRESHOLD_INSTANTIATE
#define VIL3D_THRESHOLD_INSTANTIATE(srcT) \
template void vil3d_threshold_above(const vil3d_image_view<srcT >& src, \
                          vil3d_image_view<bool >& dest,  srcT t); \
template void vil3d_threshold_below(const vil3d_image_view<srcT >& src, \
                          vil3d_image_view<bool >& dest,  srcT t); \
template void vil3d_threshold_inside(const vil3d_image_view<srcT >& src, \
                          vil3d_image_view<bool >& dest,  srcT t0, srcT t1); \
template void vil3d_threshold_outside(const vil3d_image_view<srcT >& src, \
                          vil3d_image_view<bool >& dest,  srcT t0, srcT t1)

#endif // vil3d_threshold_hxx_
