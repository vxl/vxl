// This is brl/bbas/bil/algo/bil_equalize.h
#ifndef bil_equalize_h_
#define bil_equalize_h_
//:
// \file
// \brief Equalize the color of one image with respect to another
// \author Matt Leotta

#include <vil/vil_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Equalize the pixels values relative to a model image
// \relatesalso vil_image_view
template<class mT, class dT>
inline void bil_equalize_linear_vals(const vil_image_view<mT>& model,
                                     const vil_image_view<dT>& data,
                                     double& scale, double& offset)
{
  unsigned ni = model.ni(), nj = model.nj(), np = model.nplanes();
  assert(data.ni()==ni && data.nj()==nj && data.nplanes()==np);

  std::ptrdiff_t istepM=model.istep(),
                jstepM=model.jstep(),
                pstepM=model.planestep();
  std::ptrdiff_t istepD=data.istep(),
                jstepD=data.jstep(),
                pstepD=data.planestep();
  const mT* planeM = model.top_left_ptr();
  const dT* planeD = data.top_left_ptr();

  // accumulate statistics
  double mom_m=0.0, mom_d=0.0, mom_d2=0.0, mom_md=0.0;
  unsigned count = 0;
  for (unsigned p=0;p<np;++p,planeM += pstepM,planeD += pstepD)
  {
    const mT* rowM   = planeM;
    const dT* rowD   = planeD;
    for (unsigned j=0;j<nj;++j,rowM += jstepM,rowD += jstepD)
    {
      const mT* pixelM   = rowM;
      const dT* pixelD   = rowD;
      for (unsigned i=0;i<ni;++i,pixelM+=istepM,pixelD+=istepD)
      {
        double m = *pixelM;
        double d = *pixelD;
        mom_m += m;
        mom_d += d;
        mom_d2 += d*d;
        mom_md += m*d;
        ++count;
      }
    }
  }
  double mean_m = mom_m/count;
  double mean_d = mom_d/count;
  scale = (mom_md - count*mean_m*mean_d)/(mom_d2 - count*mean_d*mean_d);
  offset = mean_m - scale*mean_d;
}


//: Equalize the pixels values relative to a model image
// \relatesalso vil_image_view
template<class mT, class dT>
inline void bil_equalize_linear(const vil_image_view<mT>& model,
                                      vil_image_view<dT>& data)
{
  double scale=1.0, offset=0.0;
  bil_equalize_linear_vals(model,data,scale,offset);
  for (unsigned p=0; p<data.nplanes(); ++p)
  {
    vil_image_view<dT> plane = vil_plane(data,p);
    vil_math_scale_and_offset_values(plane,scale,offset);
  }
}


//: Equalize the pixels values relative to a model image
// \relatesalso vil_image_view
template<class mT, class dT>
inline void bil_equalize_linear_planes(const vil_image_view<mT>& model,
                                             vil_image_view<dT>& data)
{
  double scale=1.0, offset=0.0;
  for (unsigned p=0; p<data.nplanes(); ++p)
  {
    vil_image_view<dT> plane = vil_plane(data,p);
    bil_equalize_linear_vals(model,plane,scale,offset);
    vil_math_scale_and_offset_values(plane,scale,offset);
  }
}


//: Equalize the pixels values relative to a model image with a mask
// \relatesalso vil_image_view
template<class mT, class dT>
inline void bil_equalize_linear_vals(const vil_image_view<mT>& model,
                                     const vil_image_view<dT>& data,
                                     const vil_image_view<bool>& mask,
                                     double& scale, double& offset)
{
  unsigned ni = model.ni(),nj = model.nj(),np = model.nplanes();
  assert(data.ni()==ni && data.nj()==nj && data.nplanes()==np);
  assert(mask.ni()==ni && mask.nj()==nj && mask.nplanes()==1 );

  std::ptrdiff_t istepM=model.istep(),jstepM=model.jstep(),pstepM = model.planestep();
  std::ptrdiff_t istepD=data.istep(),jstepD=data.jstep(),pstepD = data.planestep();
  std::ptrdiff_t istepB=mask.istep(),jstepB=mask.jstep();
  const mT* planeM = model.top_left_ptr();
  const dT* planeD = data.top_left_ptr();

  // accumulate statistics
  double mom_m=0.0, mom_d=0.0, mom_d2=0.0, mom_md=0.0;
  unsigned count = 0;
  for (unsigned p=0;p<np;++p,planeM += pstepM,planeD += pstepD)
  {
    const mT* rowM   = planeM;
    const dT* rowD   = planeD;
    const bool* rowB = mask.top_left_ptr();
    for (unsigned j=0;j<nj;++j,rowM += jstepM,rowD += jstepD,rowB += jstepB)
    {
      const mT* pixelM   = rowM;
      const dT* pixelD   = rowD;
      const bool* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelM+=istepM,pixelD+=istepD,pixelB+=istepB)
        if (*pixelB)
        {
          double m = *pixelM;
          double d = *pixelD;
          mom_m += m;
          mom_d += d;
          mom_d2 += d*d;
          mom_md += m*d;
          ++count;
        }
    }
  }

  double mean_m = mom_m/count;
  double mean_d = mom_d/count;
  scale = (mom_md - count*mean_m*mean_d)/(mom_d2 - count*mean_d*mean_d);
  offset = mean_m - scale*mean_d;
}


//: Equalize the pixels values relative to a model image with a mask
// \relatesalso vil_image_view
template<class mT, class dT>
inline void bil_equalize_linear(const vil_image_view<mT>& model,
                                      vil_image_view<dT>& data,
                                const vil_image_view<bool>& mask)
{
  double scale=1.0, offset=0.0;
  bil_equalize_linear_vals(model,data,mask,scale,offset);
  for (unsigned p=0; p<data.nplanes(); ++p)
  {
    vil_image_view<dT> plane = vil_plane(data,p);
    vil_math_scale_and_offset_values(plane,scale,offset);
  }
}


//: Equalize the pixels values relative to a model image with a mask
// \relatesalso vil_image_view
template<class mT, class dT>
inline void bil_equalize_linear_planes(const vil_image_view<mT>& model,
                                             vil_image_view<dT>& data,
                                       const vil_image_view<bool>& mask)
{
  double scale=1.0, offset=0.0;
  for (unsigned p=0; p<data.nplanes(); ++p)
  {
    vil_image_view<dT> plane = vil_plane(data,p);
    bil_equalize_linear_vals(model,plane,mask,scale,offset);
    vil_math_scale_and_offset_values(plane,scale,offset);
  }
}


#endif // bil_equalize_h_
