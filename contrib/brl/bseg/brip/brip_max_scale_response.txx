#ifndef brip_max_scale_response_txx_
#define brip_max_scale_response_txx_

#include "brip_max_scale_response.h"
#include <vcl_cmath.h>
#include <vcl_iomanip.h>
#include <vil/vil_convert.h>
#include <vnl/vnl_numeric_traits.h>
#include <brip/brip_vil_float_ops.h>

template <class T>
brip_max_scale_response<T>::
brip_max_scale_response( vcl_vector<vil_image_view<T> > const& pyramid)
{
  pyramid_ = pyramid;
  pyramid_scales_.push_back(1.0f);
  unsigned ni = pyramid_[0].ni(), nj = pyramid_[0].nj();
  unsigned nplanes = pyramid_[0].nplanes();
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
                         float max_scale )
: trace_valid_(false)
{
  // not yet implemented - FIXME - JLM
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

template <class T>
vcl_vector<vil_image_view<float> >
brip_max_scale_response<T>::scale_pyramid()
{
  vcl_vector<vil_image_view<float> > temp, junk;
  if (!trace_valid_)
    return temp;
  vil_image_view<float> sbase = this->scale_base();
  unsigned ni = sbase.ni(), nj = sbase.nj();
#if 0
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
    float sc1 = pyramid_scales_[level-1];
    unsigned step = static_cast<unsigned>(sc/sc0);
    unsigned nil = trace_[level].ni(), njl = trace_[level].nj();
    vil_image_view<float> sl(nil, njl);
    for (unsigned j = 0; j<njl; ++j)
      for (unsigned i = 0; i<nil; ++i) {
        //find the maximum scale within the pixel at level
        float scg = vnl_numeric_traits<float>::maxval;
        for (unsigned m = 0; m<step; ++m){
          unsigned bj = j*step+m;
          for (unsigned k = 0; k<step; ++k){
            unsigned bi = i*step+k;
            if (sbase(bi, bj)<scg)
              scg = sbase(bi, bj);
          }
        }
        sl(i,j) = scg;
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
  vil_image_view<float> base_tr;
  base_tr.deep_copy(trace_[0]);
  vil_image_view<float> base_sc(ni, nj);
  float sc0 = pyramid_scales_[0];
  base_sc.fill(sc0);
  for (unsigned level = 1; level<nlevels; ++level)
  {
    float sc = pyramid_scales_[level];
    unsigned step = static_cast<unsigned>(sc/sc0);
    vil_image_view<float>& trl = trace_[level];
    ni = trl.ni(); nj = trl.nj();
    for (unsigned j = 0; j<nj; ++j)
      for (unsigned i = 0; i<ni; ++i)
        for (unsigned m = 0; m<step; ++m){
          unsigned bj = j*step+m;
          for (unsigned k = 0; k<step; ++k){
            unsigned bi = i*step+k;
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
