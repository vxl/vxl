// This is gel/vdgl/vdgl_fit_lines.cxx
#include "vdgl_fit_lines.h"
//:
// \file
#include <vsol/vsol_line_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>

//--------------------------------------------------------------------------
//:
//  Convert each digital curve to a set of vgl_point_2d<double> and add
//  them to vgl linear regression fitter point set. A set of line segments
//  are computed that fit the point set within a specified mean square
//  tolerance.  The resulting vgl_line_segment_2d<double> segments
//  are converted back to vsol geometry.
//
bool vdgl_fit_lines::fit_lines()
{
  if (line_segs_.size() > 0) // fit_lines() has already been called
    return true;
  if (!curves_.size())
    return false;
  fitter_.set_min_fit_length(min_fit_length_);
  fitter_.set_rms_error_tol(rms_distance_);
  for (vcl_vector<vdgl_digital_curve_sptr>::iterator eit = curves_.begin();
       eit != curves_.end(); eit++)
  {
    vdgl_digital_curve_sptr dc = (*eit);
    vdgl_interpolator_sptr intp = dc->get_interpolator();
    vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
    fitter_.clear();
    int nedgl = ec->size();
    for (int i=0; i<nedgl; i++)
    {
      vgl_point_2d<double> p((*ec)[i].x(), (*ec)[i].y());
      fitter_.add_point(p);
    }

    fitter_.fit();
    vcl_vector<vgl_line_segment_2d<double> >& segs = fitter_.get_line_segs();
    for (vcl_vector<vgl_line_segment_2d<double> >::iterator sit=segs.begin();
         sit != segs.end(); sit++)
    {
      vsol_line_2d_sptr line = new vsol_line_2d(*sit);
      line_segs_.push_back(line);
    }
  }
  return true;
}
