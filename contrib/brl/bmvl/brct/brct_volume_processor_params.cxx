// This is brl/bmvl/brct/brct_volume_processor_params.cxx
#include "brct_volume_processor_params.h"
//:
// \file
// See brct_volume_processor_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

brct_volume_processor_params::
brct_volume_processor_params(brct_volume_processor_params const& vp)
{
  InitParams(vp.xmin_,
             vp.xmax_,
             vp.ymin_,
             vp.ymax_,
             vp.zmin_,
             vp.zmax_,
             vp.cube_edge_length_,
             vp.cell_thresh_,
             vp.dummy2_,
             vp.dummy3_
            );
}

brct_volume_processor_params::
brct_volume_processor_params(const float xmin,
                             const float xmax,
                             const float ymin,
                             const float ymax,
                             const float zmin,
                             const float zmax,
                             const int cube_edge_length,
                             const float cell_thresh,
                             const float dummy2,
                             const float dummy3)
{
  InitParams(xmin, xmax, ymin, ymax, zmin, zmax, cube_edge_length,
             cell_thresh,  dummy2,  dummy3);
}

void brct_volume_processor_params::InitParams(float xmin,
                                              float xmax,
                                              float ymin,
                                              float ymax,
                                              float zmin,
                                              float zmax,
                                              int cube_edge_length,
                                              float cell_thresh,
                                              float dummy2,
                                              float dummy3)
{
  xmin_ = xmin;
  xmax_ = xmax;
  ymin_ = ymin;
  ymax_ = ymax;
  zmin_ = zmin;
  zmax_ = zmax;
  cube_edge_length_ = cube_edge_length;
  cell_thresh_= cell_thresh;
  dummy2_= dummy2;
  dummy3_= dummy3;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool brct_volume_processor_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator<< (vcl_ostream& os, const brct_volume_processor_params& vp)
{
  os << "brct_volume_processor_params:\n[---\n"
     << "xmin " << vp.xmin_ << vcl_endl
     << "xmax " << vp.xmax_ << vcl_endl
     << "ymin " << vp.ymin_ << vcl_endl
     << "ymax " << vp.ymax_ << vcl_endl
     << "zmin " << vp.zmin_ << vcl_endl
     << "zmax " << vp.zmax_ << vcl_endl
     << "cube_edge_length " << vp.cube_edge_length_ << vcl_endl
     << "cell thresh " << vp.cell_thresh_ << vcl_endl
     << "---]" << vcl_endl;
  return os;
}
