#ifndef brip_max_scale_response_txx_
#define brip_max_scale_response_txx_

#include "brip_max_scale_response.h"
#include <vcl_cmath.h>
#include <vcl_iomanip.h>
#include <vnl/vnl_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_resample_bilin.h>
#include <vnl/vnl_numeric_traits.h>
#include <brip/brip_vil_float_ops.h>

template <class T>
brip_max_scale_response<T>::
brip_max_scale_response( vcl_vector<vil_image_view<T> > const& pyramid)
{
  pyramid_ = pyramid;
  pyramid_scales_.push_back(1.0f);
  unsigned ni = pyramid_[0].ni(), nj = pyramid_[0].nj();
  float nif = static_cast<float>(ni), njf = static_cast<float>(nj);
  float diag0 = vcl_sqrt(nif*nif + njf*njf);
  for (unsigned level = 1; level<pyramid.size(); ++level)
  {
    ni = pyramid_[level].ni(); nj = pyramid_[level].nj();
    nif = static_cast<float>(ni); njf = static_cast<float>(nj);
    float diag = vcl_sqrt(nif*nif + njf*njf);
    float scale = diag0/diag;
    pyramid_scales_.push_back(scale);
  }
  this->compute_trace_pyramid();
}

template <class T>
brip_max_scale_response<T>::
brip_max_scale_response( vil_image_view<T> const& base_image,
                         double scale_ratio,
                         double max_scale )
{
  // compute half octave scale intervals
  float scale = 1.0;
  if(scale_ratio>max_scale)
    scale_ratio = max_scale;
  while(scale<=static_cast<float>(max_scale))
    {
      pyramid_scales_.push_back(scale);
      scale*=static_cast<float>(scale_ratio);
    }
  pyramid_.push_back(base_image);
  unsigned nlevels = pyramid_scales_.size();
  for(unsigned level = 1; level<nlevels; ++level)
    {
      float s = pyramid_scales_[level];
      float s1 = pyramid_scales_[level-1];
      vil_image_view<T> img1 = pyramid_[level-1];
      unsigned nil1 = img1.ni(), nilj = img1.nj();
      unsigned nil =  static_cast<unsigned>(nil1*s1/s),
        njl = static_cast<unsigned>(nilj*s1/s);
      vil_image_view<T> dec_img;
      vil_resample_bilin(img1, dec_img, nil, njl);
      pyramid_.push_back(dec_img);
    }
  this->compute_trace_pyramid();
}

template <class T>
void brip_max_scale_response<T>::compute_trace_pyramid()
{
  unsigned nlevels = pyramid_.size();
  for (unsigned level = 0; level<nlevels; ++level)
  {
    unsigned ni = pyramid_[level].ni(), nj = pyramid_[level].nj();
    vil_image_view<float> lview;
    if (ni<=5||nj<=5) {
      lview.set_size(ni, nj);
      lview.fill(0.0f);
      trace_.push_back(lview);
      continue;
    }
    if (pyramid_[level].nplanes()>1)
      vil_convert_planes_to_grey(pyramid_[level], lview);
    else {
      vil_image_view_base_sptr v = new vil_image_view<T>(pyramid_[level]);
      lview = vil_convert_cast(float(), v);
    }
    vil_image_view<float> smooth = brip_vil_float_ops::gaussian(lview, 0.75, lview(0,0));
#ifdef DEBUG
    vcl_cout << "Input at level " << level << '\n';
    for (unsigned j = 0; j<smooth.nj(); ++j){
      for (unsigned i = 0; i<smooth.nj(); ++i)
        vcl_cout << vcl_setprecision(2) << vcl_fixed << smooth(i,j) << ' ';
      vcl_cout <<'\n';
    }
#endif
    const unsigned radius = 2;
    vil_image_view<float> tr =
      brip_vil_float_ops::trace_grad_matrix_NxN(smooth, radius);
    trace_.push_back(tr);
#ifdef DEBUG
    vcl_cout << "Level " << level << '\n';
    for (unsigned j = 0; j<tr.nj(); ++j){
      for (unsigned i = 0; i<tr.nj(); ++i)
        vcl_cout << 10*tr(i,j) << ' ';
      vcl_cout <<'\n';
    }
#endif
  }
  trace_valid_ = true;
}

//find the closest integral pixel location in an image with 
//the specified scale ratio to the image with location i.
static unsigned loci(unsigned i, float scale_ratio)
{
  float pos = scale_ratio*i;
  unsigned ival = static_cast<unsigned>(pos+0.5f);
  return ival;
}

template <class T>
vcl_vector<vil_image_view<float> >
brip_max_scale_response<T>::scale_pyramid()
{
  vcl_vector<vil_image_view<float> > temp, junk;
  if (!trace_valid_)
    return temp;
  vil_image_view<float> sbase = this->scale_base();
  unsigned ni = sbase.ni(), nj = sbase.nj();
#ifdef DEBUG
  vcl_cout << "Printing scale base\n";
  for (unsigned j = 0; j<nj; ++j){
    for (unsigned i = 0; i<ni; ++i)
      vcl_cout << static_cast<unsigned>(sbase(i,j)) << ' ';
    vcl_cout <<'\n';
  }
#endif
  unsigned nlevels = pyramid_scales_.size();
  float sc0 = pyramid_scales_[0];
  temp.resize(nlevels);
  temp[0]=sbase;
  for (unsigned level = 1; level<nlevels; ++level)
  {
    float sc = pyramid_scales_[level];
    float ratio = sc0/sc;
    unsigned nil = trace_[level].ni(), njl = trace_[level].nj();
    vil_image_view<float> sl(nil, njl);
    sl.fill(0.0f);
    for (unsigned bj = 0; bj<nj; ++bj){
      unsigned j = loci(bj, ratio);
      if(j>=njl) j=njl-1;
      for (unsigned bi = 0; bi<ni; ++bi) {
        unsigned i = loci(bi, ratio);
        if(i>=nil) i=nil-1;
        if(sbase(bi,bj)>sl(i,j))
          sl(i,j)=sbase(bi, bj);
      }
    }
    temp[level]=sl;
  }
  return temp;
}


template <class T>
vcl_vector<vil_image_view<vxl_byte> >
brip_max_scale_response<T>::mask_pyramid() 
{
  vcl_vector<vil_image_view<vxl_byte> > temp;
  if (!trace_valid_)
    return temp;
  vcl_vector<vil_image_view<float> > scales = this->scale_pyramid();
  unsigned nlevels = scales.size();
  for (unsigned level = 0; level<nlevels; ++level)
  {
    float cur_scale = pyramid_scales_[level];
    unsigned ni = scales[level].ni(), nj = scales[level].nj();
    vil_image_view<vxl_byte> mask(ni,nj);
    mask.fill(0);

    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        if (scales[level](i,j)==cur_scale)
          mask(i,j) = 255;
    temp.push_back(mask);
  }
  return temp;
}


template <class T>
vil_image_view<float>
brip_max_scale_response<T>::scale_base()
{
  vil_image_view<float> temp;
  if (!trace_valid_)
    this->compute_trace_pyramid();
  if (!trace_valid_)
    return temp;
  unsigned nlevels = trace_.size();
  unsigned ni = trace_[0].ni(), nj = trace_[0].nj();
  unsigned bni = ni, bnj = nj;
  vil_image_view<float> base_tr;
  base_tr.deep_copy(trace_[0]);
  vil_image_view<float> base_sc(ni, nj);
  float sc0 = pyramid_scales_[0];
  base_sc.fill(sc0);
  for (unsigned level = 1; level<nlevels; ++level)
  {
    float sc = pyramid_scales_[level];
    float ratio = sc0/sc;
    vil_image_view<float>& trl = trace_[level];
    ni = trl.ni(); nj = trl.nj();
    for (unsigned bj = 0; bj<bnj; ++bj){
      unsigned j = loci(bj, ratio);
      if(j>=nj) j=nj-1;
      for (unsigned bi = 0; bi<bni; ++bi)
        {
          unsigned i = loci(bi, ratio);
          if(i>=ni) i=ni-1;
          if (trl(i,j)>base_tr(bi,bj)){
            base_tr(bi, bj)=trl(i,j);
            base_sc(bi, bj)=sc;
          }
        }
    }
  }
  return base_sc;
}

// Macro to perform manual instantiations
#define BRIP_MAX_SCALE_RESPONSE(T) \
template class brip_max_scale_response<T >

#endif // brip_max_scale_response_txx_
