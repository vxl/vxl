// This is brl/bseg/sdet/sdet_fit_lines.cxx
#include "sdet_fit_lines.h"
//:
// \file
// \verbatim
//  Modifications
//   Bing Yu 1/23/2008 ---- make sure line is not length 0 in function fit_lines()
// \endverbatim

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
sdet_fit_lines::sdet_fit_lines(sdet_fit_lines_params& flp)
  : sdet_fit_lines_params(flp), fitter_(vgl_fit_lines_2d<double>())
{
}

// Default Destructor
sdet_fit_lines::~sdet_fit_lines()
= default;

//-------------------------------------------------------------------------
//: Set the edges to be processed
//
void sdet_fit_lines::set_edges(std::vector<vtol_edge_2d_sptr> const& edges)
{
  segs_valid_ = false;
  edges_=edges;
}

//--------------------------------------------------------------------------
//:
//  Convert each digital curve to a set of vgl_point_2d<double> and add
//  them to vgl linear regression fitter point set. A set of line segments
//  are computed that fit the point set within a specified mean square
//  tolerance.  The resulting vgl_line_segment_2d<double> segments
//  are converted back to vsol geometry.
//
bool sdet_fit_lines::fit_lines()
{
  if (segs_valid_)
    return false;
  if (!edges_.size())
    return false;
  line_segs_.clear();
  fitter_.set_min_fit_length(min_fit_length_);
  fitter_.set_rms_error_tol(rms_distance_);
  for (auto & edge : edges_)
  {
    vsol_curve_2d_sptr c = edge->curve();
    vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
    if (!dc)
      continue;
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

    std::vector<vgl_line_segment_2d<double> >& segs = fitter_.get_line_segs();
    for (auto & seg : segs)
      line_segs_.push_back(seg);
  }
  segs_valid_ = true;
  return true;
}

//-------------------------------------------------------------------------
//: Get the line segments
//
std::vector<vsol_line_2d_sptr> sdet_fit_lines::get_line_segs()
{
  std::vector<vsol_line_2d_sptr> ret;
  if(!segs_valid_)
    this->fit_lines();
  for (auto & line_seg : line_segs_)
    {
      vsol_line_2d_sptr line = new vsol_line_2d(line_seg);
      // make sure the start point and end point of line are not the same
      if ( line->length() != 0.0 )
        ret.push_back(line);
    }
  return ret;
}
void sdet_fit_lines::get_line_segs(std::vector<vsol_line_2d_sptr>& lines)
{
  lines = this->get_line_segs();
}
void sdet_fit_lines::
get_line_segs(std::vector<vgl_line_segment_2d<double> >& lines)
{
  lines = line_segs_;
}

//----------------------------------------------------------
//: Clear internal storage
//
void sdet_fit_lines::clear()
{
  fitter_.clear();
  edges_.clear();
  line_segs_.clear();
  segs_valid_ = false;
}
