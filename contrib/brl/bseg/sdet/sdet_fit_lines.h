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
//  vcl_vector<vgl_line_segments_2d>. The actual fitting algorithm is 
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
#include <vcl_vector.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <sdet/sdet_fit_lines_params.h>

class sdet_fit_lines : public sdet_fit_lines_params
{
 public:
  //:Constructors/destructor
  sdet_fit_lines(sdet_fit_lines_params& rpp);

  ~sdet_fit_lines();
  //: Process methods
  bool fit_lines();
  void clear();
  //: Accessors
  void set_edges(vcl_vector<vtol_edge_2d_sptr> const& edges);
  vcl_vector<vsol_line_2d_sptr>& get_line_segs();
  void get_line_segs(vcl_vector<vsol_line_2d_sptr>& lines){lines=line_segs_;}
 protected:
  //:protected methods

  //:members
  bool segs_valid_;      //process state flag
  vcl_vector<vtol_edge_2d_sptr> edges_;// the input edges
  vgl_fit_lines_2d<double> fitter_;//the fitting class
  vcl_vector<vsol_line_2d_sptr> line_segs_;
};

#endif // sdet_fit_lines_h_
