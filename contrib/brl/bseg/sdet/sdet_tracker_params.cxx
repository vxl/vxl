//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/bseg/sdet/sdet_tracker_params.cxx
#include <sdet/sdet_tracker_params.h>
//:
// \file
// See sdet_tracker_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_tracker_params::
sdet_tracker_params(const sdet_tracker_params& tp)
{
  InitParams(tp.n_samples_,
             tp.search_radius_,
             tp.match_thresh_
             );
}

sdet_tracker_params::
sdet_tracker_params(const int n_samples, 
                    const float search_radius,
                    const float match_thresh)
{
  InitParams(n_samples, 
             search_radius,
             match_thresh);
}

void sdet_tracker_params::InitParams(int n_samples, 
                                     float search_radius,
                                     float match_thresh)
{
  n_samples_ = n_samples; 
  search_radius_ = search_radius;
  match_thresh_ = match_thresh;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool sdet_tracker_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_tracker_params& tp)
{
  os << "sdet_tracker_params:" << vcl_endl << "[---" << vcl_endl;
  os << "n_samples " << tp.n_samples_ << vcl_endl;
  os << "search_radius " << tp.search_radius_ << vcl_endl;
  os << "match_thresh " << tp.match_thresh_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
