// This is brl/bseg/sdet/sdet_texture_classifier_params.cxx
#include <sstream>
#include <iostream>
#include "sdet_texture_classifier_params.h"
//:
// \file
// See sdet_texture_classifier_params.h
//
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//------------------------------------------------------------------------
// Constructors
//
sdet_texture_classifier_params::sdet_texture_classifier_params():
  n_scales_(3), scale_interval_(45.0f), angle_interval_(30.0f)
  {
    lambda0_=4.0f;  lambda1_=1.6f; //was 3.0/1.2
    laplace_radius_ = 1.5f;
    gauss_radius_ = 1.5f;
    cutoff_per_ = 0.01f;
    signed_response_ = true;
    mag_ = false;
    fast_ = true;
    k_ = 100;
    n_samples_ = 200000;
    block_size_ = 64;
    weight_offset_ = 0.01f;
  }

sdet_texture_classifier_params::
sdet_texture_classifier_params(const sdet_texture_classifier_params& dmp)
  : gevd_param_mixin()
{
  InitParams(dmp.n_scales_, dmp.scale_interval_, dmp.angle_interval_,
             dmp.lambda0_, dmp.lambda1_,dmp.laplace_radius_,
             dmp.gauss_radius_,dmp.cutoff_per_, dmp.signed_response_,
             dmp.mag_, dmp.fast_, dmp.k_, dmp.n_samples_, dmp.block_size_,
             dmp.weight_offset_);
}

sdet_texture_classifier_params::
sdet_texture_classifier_params(unsigned n_scales,
                               float scale_interval,
                               float angle_interval,
                               float lambda0,
                               float lambda1,
                               float laplace_radius,
                               float gauss_radius,
                               float cutoff_per,
                               bool signed_response,
                               bool mag,
                               bool fast,
                               unsigned k,
                               unsigned n_samples,
                               unsigned block_size,
                               float weight_offset
                               )
{
  InitParams(n_scales, angle_interval,scale_interval,
             lambda0, lambda1, laplace_radius, gauss_radius,
             cutoff_per, signed_response, mag, fast, k, n_samples,
             block_size, weight_offset);
}

void sdet_texture_classifier_params::
InitParams(unsigned n_scales, float scale_interval, float angle_interval,
           float lambda0, float lambda1,
           float laplace_radius, float gauss_radius,
           float cutoff_per, bool signed_response,
           bool mag, bool fast, unsigned k, unsigned n_samples,
           unsigned block_size,
           float weight_offset)
{
  n_scales_ = n_scales;
  scale_interval_ = scale_interval;
  angle_interval_ = angle_interval;
  lambda0_ = lambda0;
  lambda1_ = lambda1;
  laplace_radius_ = laplace_radius;
  gauss_radius_ = gauss_radius;
  cutoff_per_ = cutoff_per;
  signed_response_ = signed_response;
  mag_ = mag;
  fast_ = fast;
  k_ = k;
  n_samples_ = n_samples;
  block_size_ = block_size;
  weight_offset_ =   weight_offset;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
bool sdet_texture_classifier_params::SanityCheck()
{
  //  Note that msg << ends seems to restart the string and erase the
  //  previous string. We should only use it as the last call, use
  //  std::endl otherwise.
  std::stringstream msg;
  bool valid = true;

  if (n_scales_ < 3)
    {
      msg << "ERROR: at least 3 filters are needed!\n";
      valid = false;
    }
  if(mag_&&signed_response_){
      msg << "ERROR: can't have both abs mag and signed responses!\n";
      valid = false;
    }


  msg << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator << (std::ostream& os, const sdet_texture_classifier_params& dmp)
{
  os << "sdet_texture_classifier_params:\n[---\n"
     << "n scales " << dmp.n_scales_ << " scale interval "
     << dmp.scale_interval_ << '\n'
     << "angle interval " << dmp.angle_interval_ << '\n'
     << "lambda0 " << dmp.lambda0_ << " lambda1 " << dmp.lambda1_ << '\n';
  os << "laplace lambda " << dmp.laplace_radius_ << '\n';
  os << "gauss lambda " << dmp.gauss_radius_ << '\n';
  os << "gauss cutoff thresh " << dmp.cutoff_per_ << '\n';
  os << "signed response " << dmp.signed_response_ << '\n';
  os << "magnitude " << dmp.mag_ << '\n';
  os << "fast " << dmp.fast_ << '\n';
  os << "k " << dmp.k_ << '\n';
  os << "n_samples " << dmp.n_samples_ << '\n';
  os << "block size " << dmp.block_size_ << '\n';
  os << "weight offset " << dmp.weight_offset_ << '\n';
  os << "---]\n";
  return os;
}
std::string sdet_texture_classifier_params::filter_dir_name()
{
  std::stringstream str;
  if (signed_response_)
    str << "filter_bank_" << n_scales_ << "_" << lambda0_ << "_" << lambda1_ << "_" << scale_interval_ << "_" << angle_interval_ << "_"
        << laplace_radius_ << "_" << gauss_radius_ << "_" << cutoff_per_ << "_signed";
  else
    str << "filter_bank_" << n_scales_ << "_" << lambda0_ << "_" << lambda1_ << "_" << scale_interval_ << "_" << angle_interval_ << "_"
        << laplace_radius_ << "_" << gauss_radius_ << "_" << cutoff_per_ << "_not_signed";
  return str.str();
}

//: Binary save vgl_point_2d to stream.
void vsl_b_write(vsl_b_ostream &os,
                 const sdet_texture_classifier_params & tcp)
{
  auto& tcpnc =
    const_cast<sdet_texture_classifier_params&>(tcp);
  vsl_b_write(os,tcpnc.n_scales_);
  vsl_b_write(os,tcpnc.scale_interval_);
  vsl_b_write(os,tcpnc.angle_interval_);
  vsl_b_write(os,tcpnc.lambda0_);
  vsl_b_write(os,tcpnc.lambda1_);
  vsl_b_write(os,tcpnc.laplace_radius_);
  vsl_b_write(os,tcpnc.gauss_radius_);
  vsl_b_write(os,tcpnc.cutoff_per_);
  vsl_b_write(os,tcpnc.signed_response_);
  vsl_b_write(os,tcpnc.mag_);
  vsl_b_write(os,tcpnc.fast_);
  vsl_b_write(os,tcpnc.k_);
  vsl_b_write(os,tcpnc.n_samples_);
  vsl_b_write(os,tcpnc.block_size_);
  vsl_b_write(os,tcpnc.weight_offset_);
}

//: Binary load vgl_point_2d from stream.
void vsl_b_read(vsl_b_istream &is, sdet_texture_classifier_params & tcp)
{
  vsl_b_read(is,tcp.n_scales_);
  vsl_b_read(is,tcp.scale_interval_);
  vsl_b_read(is,tcp.angle_interval_);
  vsl_b_read(is,tcp.lambda0_);
  vsl_b_read(is,tcp.lambda1_);
  vsl_b_read(is,tcp.laplace_radius_);
  vsl_b_read(is,tcp.gauss_radius_);
  vsl_b_read(is,tcp.cutoff_per_);
  vsl_b_read(is,tcp.signed_response_);
  vsl_b_read(is,tcp.mag_);
  vsl_b_read(is,tcp.fast_);
  vsl_b_read(is,tcp.k_);
  vsl_b_read(is,tcp.n_samples_);
  vsl_b_read(is,tcp.block_size_);
  vsl_b_read(is,tcp.weight_offset_);
}
//: Print human readable summary of object to a stream
void vsl_print_summary(std::ostream& os,
                       const sdet_texture_classifier_params & tcp)
{
  os << tcp;
}
