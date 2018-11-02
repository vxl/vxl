// This is brl/bseg/sdet/sdet_fit_lines.h
#ifndef sdet_fit_lines_h_
#define sdet_fit_lines_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for fitting line segments to edges
//
//  This line fitting algorithm is based on the original Charlie Rothwell
//  detector as ported by FSM in osl.  This version does incremental
//  fitting to a chain of points and produces a
//  std::vector<vgl_line_segments_2d>. The actual fitting algorithm is
//  in vgl/algo and therefore should be of wider applicability.
//
// \author
//  J.L. Mundy - April 10, 2003
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vgl/vgl_line_segment_2d.h>
#include <sdet/sdet_fit_lines_params.h>

class sdet_fit_lines : public sdet_fit_lines_params
{
 public:
  //:Constructors/destructor
  sdet_fit_lines(sdet_fit_lines_params& rpp);

  ~sdet_fit_lines() override;
  //: Process methods
  bool fit_lines();
  void clear();
  //: Accessors
  void set_edges(std::vector<vtol_edge_2d_sptr> const& edges);
  std::vector<vsol_line_2d_sptr> get_line_segs();
  void get_line_segs(std::vector<vsol_line_2d_sptr>& lines);
  void get_line_segs(std::vector<vgl_line_segment_2d<double> >& lines);

 protected:
  //:protected methods

  //:members
  bool segs_valid_;      //process state flag
  std::vector<vtol_edge_2d_sptr> edges_;// the input edges
  vgl_fit_lines_2d<double> fitter_;//the fitting class
  std::vector<vgl_line_segment_2d<double> > line_segs_;
};

#endif // sdet_fit_lines_h_
