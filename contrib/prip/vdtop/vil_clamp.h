// This is prip/vdtop/vil_clamp.h
#ifndef vil_clamp_h_
#define vil_clamp_h_

//:
// \file
// \brief Function computing the exponantial gradient magnitude only.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vil/vil_image_view.h>

template<class srcT>
void vil_clamp_below(vil_image_view<srcT>& src, srcT t, srcT value)
{
   unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();

   vcl_ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
   srcT* planeA = src.top_left_ptr();

   for (unsigned p=0;p<np;++p,planeA += pstepA)
   {
     srcT* rowA   = planeA;
     for (unsigned j=0;j<nj;++j,rowA += jstepA)
     {
       srcT* pixelA = rowA;
       for (unsigned i=0;i<ni;++i,pixelA+=istepA)
         if (*pixelA<=t) *pixelA=value ;
     }
  }
}

#endif
