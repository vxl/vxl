// This is brl/bseg/sdet/sdet_fit_conics.cxx
#include "sdet_fit_conics.h"
//:
// \file

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_conic_segment_2d.h>
#include <vsol/vsol_conic_2d.h>
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
//
sdet_fit_conics::sdet_fit_conics(sdet_fit_conics_params& flp)
  : sdet_fit_conics_params(flp), fitter_(vgl_fit_conics_2d<double>())
{
}

//:Default Destructor
sdet_fit_conics::~sdet_fit_conics()
= default;

//-------------------------------------------------------------------------
//: Set the edges to be processed
//
void sdet_fit_conics::set_edges(std::vector<vtol_edge_2d_sptr> const& edges)
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
bool sdet_fit_conics::fit_conics()
{
  if (segs_valid_)
    return false;
  if (!edges_.size())
    return false;
  fitter_.set_min_fit_length(min_fit_length_);
  fitter_.set_rms_error_tol(rms_distance_);
  for (std::vector<vtol_edge_2d_sptr>::iterator eit = edges_.begin();
       eit != edges_.end(); eit++)
  {
    vsol_curve_2d_sptr c = (*eit)->curve();
    vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
    if (!dc)
      continue;
    vdgl_interpolator_sptr intp = dc->get_interpolator();
    vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
    fitter_.clear();
    int nedgl = ec->size();
    if (nedgl<min_fit_length_)
      continue;
    for (int i=0; i<nedgl; i++)
    {
      vgl_point_2d<double> p((*ec)[i].x(), (*ec)[i].y());
      fitter_.add_point(p);
    }

    fitter_.fit();
    std::vector<vgl_conic_segment_2d<double> >& segs = fitter_.get_conic_segs();
    for (std::vector<vgl_conic_segment_2d<double> >::iterator sit=segs.begin();
         sit != segs.end(); sit++)
    {
      vsol_conic_2d_sptr conic = new vsol_conic_2d(*sit);
      //std::cout << "Fitted a conic of type " << conic->real_conic_type() << '\n';
      //adding a condition on aspect ratio
      if (conic->real_type() != vsol_conic_2d::real_ellipse)
      {
          conic_segs_.push_back(conic);
      }
      else
      {
        double cx, cy, width, height, angle;
        conic->ellipse_parameters(cx,cy,angle,width,height);
        if (width/height < aspect_ratio_)
          conic_segs_.push_back(conic);
      }
    }
  }
  segs_valid_ = true;
  return true;
}

//-------------------------------------------------------------------------
//: Get the conic segments
//
std::vector<vsol_conic_2d_sptr>& sdet_fit_conics::get_conic_segs()
{
  if (segs_valid_)
    return conic_segs_;
  this->fit_conics();
  return conic_segs_;
}

//----------------------------------------------------------
//: Clear internal storage
//
void sdet_fit_conics::clear()
{
  fitter_.clear();
  edges_.clear();
  conic_segs_.clear();
  segs_valid_ = false;
}

