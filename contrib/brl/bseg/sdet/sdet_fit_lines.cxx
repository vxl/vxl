// This is brl/bseg/sdet/sdet_fit_lines.cxx
#include "sdet_fit_lines.h"
//:
// \file
#include <vcl_cstdlib.h>   // for vcl_abs(int) and vcl_qsort()
#include <vsol/vsol_line_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
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
sdet_fit_lines::sdet_fit_lines(sdet_fit_lines_params& flp)
  : sdet_fit_lines_params(flp), fitter_(vgl_fit_lines_2d<double>())
{
}

//:Default Destructor
sdet_fit_lines::~sdet_fit_lines()
{
}

//-------------------------------------------------------------------------
//: Set the edges to be processed
//
void sdet_fit_lines::set_edges(vcl_vector<vtol_edge_2d_sptr> const& edges)
{
  segs_valid_ = false;
  edges_=edges;
}

//--------------------------------------------------------------------------
//: extract a set of vsol_point_2d(s)
bool sdet_fit_lines::fit_lines()
{
  if (segs_valid_)
    return false;
  if(!edges_.size())
    return false;
  fitter_.set_min_fit_length(min_fit_length_);
  fitter_.set_rms_error_tol(rms_distance_);
  for(vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges_.begin();
      eit != edges_.end(); eit++)
    {
      vsol_curve_2d_sptr c = (*eit)->curve();
      vdgl_digital_curve_sptr dc = c->cast_to_digital_curve();
      if(!dc)
        continue;
      vdgl_interpolator_sptr intp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
      fitter_.clear();
     int nedgl = ec->size();
      for(int i=0; i<nedgl; i++)
        {
          vgl_point_2d<double> p((*ec)[i].x(), (*ec)[i].y());
          fitter_.add_point(p);
        }
      
      fitter_.fit();
      vcl_vector<vgl_line_segment_2d<double> >& segs = fitter_.get_line_segs();
      for(vcl_vector<vgl_line_segment_2d<double> >::iterator sit=segs.begin();
          sit != segs.end(); sit++)
        {
          vsol_line_2d_sptr line = new vsol_line_2d(*sit);
          line_segs_.push_back(line);
        }
    }
  segs_valid_ = true;
  return true;
}    
//-------------------------------------------------------------------------
//: Get the line segments
//
vcl_vector<vsol_line_2d_sptr>& sdet_fit_lines::get_line_segs()
{
  if(segs_valid_)
    return line_segs_;
  this->fit_lines();
  return line_segs_;
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

