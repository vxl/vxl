// This is brl/bmvl/brct/brct_plane_sweeper_params.cxx
#include <brct/brct_plane_sweeper_params.h>
//:
// \file
// See brct_plane_sweeper_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//------------------------------------------------------------------------
// Constructors
//

brct_plane_sweeper_params::
brct_plane_sweeper_params(const brct_plane_sweeper_params& sp)
  : gevd_param_mixin()
{
  InitParams(sp.zmin_,
             sp.zmax_,
             sp.nz_,
             sp.point_radius_,
             sp.corr_radius_,
             sp.corr_min_,
             sp.corr_max_,
             sp.corr_thresh_,
             sp.corr_sigma_,
             sp.intensity_thresh_,
             (sdet_harris_detector_params) sp.hdp_
            );
}

brct_plane_sweeper_params::
brct_plane_sweeper_params(const float zmin,
                          const float zmax,
                          const int  nz,
                          const float point_radius,
                          const int corr_radius,
                          const float corr_min,
                          const float corr_max,
                          const float corr_thresh,
                          const float corr_sigma,
                          const float  intensity_thresh,
                          const sdet_harris_detector_params& hdp
                          )
{
  InitParams( zmin, zmax, nz, point_radius, corr_radius, corr_min, corr_max,
              corr_thresh, corr_sigma, intensity_thresh, hdp);
}

void brct_plane_sweeper_params::InitParams(float zmin,
                                           float zmax,
                                           int  nz,
                                           float point_radius,
                                           int corr_radius,
                                           float corr_min,
                                           float corr_max,
                                           float corr_thresh,
                                           float corr_sigma,
                                           float  intensity_thresh,
                                           const sdet_harris_detector_params& hdp
                                          )
{
  zmin_ = zmin;
  zmax_ = zmax;
  nz_ = nz;
  point_radius_ = point_radius;
  corr_radius_ = corr_radius;
  corr_min_ = corr_min;
  corr_max_ = corr_max;
  corr_thresh_ = corr_thresh;
  corr_sigma_ = corr_sigma;
  intensity_thresh_ = intensity_thresh;
  hdp_ = hdp;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool brct_plane_sweeper_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator<<(vcl_ostream& os, const brct_plane_sweeper_params& sp)
{
  return
  os << "brct_plane_sweeper_params:\n[---\n"
     << "zmin " << sp.zmin_ << vcl_endl
     << "zmax " << sp.zmax_ << vcl_endl
     << "N zplanes " << sp.nz_ << vcl_endl
     << "Point Match Radius " << sp.point_radius_ << vcl_endl
     << "Corr Window Radius " << sp.corr_radius_ << vcl_endl
     << "Corr Display Range Min " << sp.corr_min_ << vcl_endl
     << "Corr Display Range Max " << sp.corr_max_ << vcl_endl
     << "Coor Thresh " << sp.corr_thresh_ << vcl_endl
     << "Corr Smooth Sigma " << sp.corr_sigma_ << vcl_endl
     << "Intensity Thresh " << sp.intensity_thresh_ << vcl_endl
     << sp.hdp_
     << "---]" << vcl_endl;
}
