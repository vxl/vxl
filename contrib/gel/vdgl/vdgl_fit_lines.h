// This is gel/vdgl/vdgl_fit_lines.h
#ifndef vdgl_fit_lines_h_
#define vdgl_fit_lines_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for fitting line segments to digital curves
//
//  This line fitting algorithm is based on the original Charlie Rothwell
//  detector as ported by FSM in osl.  This version does incremental
//  fitting to a chain of points (given as a digital curve) and produces a
//  vcl_vector<vsol_line_2d_sptr>. The actual fitting algorithm is
//  in vgl/algo and therefore should be of wider applicability.
//
// \author
//  J.L. Mundy - April 10, 2003
//
// \verbatim
//  Modifications
//   May 2004 - Peter Vanroose - ported from sdet; removed dependency on vtol
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_fit_lines_params.h>

class vdgl_fit_lines : public vdgl_fit_lines_params
{
  // members
  vcl_vector<vdgl_digital_curve_sptr> curves_; //!< the input curves
  vcl_vector<vsol_line_2d_sptr> line_segs_;    //!< the output lines
  vgl_fit_lines_2d<double> fitter_;            //!< the fitting class
 public:
  //: constructor from a parameter block (the only way)
  vdgl_fit_lines(vdgl_fit_lines_params& flp)
  : vdgl_fit_lines_params(flp), fitter_(vgl_fit_lines_2d<double>()) {}

  ~vdgl_fit_lines() {}

  //: Set the curves to be processed
  void set_curves(vcl_vector<vdgl_digital_curve_sptr> const& curves) { line_segs_.clear(); curves_=curves; }
  //: Return the line segments
  vcl_vector<vsol_line_2d_sptr>& get_line_segs() { fit_lines(); return line_segs_; }
  //: Clear the internal storage
  void clear() { fitter_.clear(); curves_.clear(); line_segs_.clear(); }

 protected:
  //: Actual process method
  bool fit_lines();
  //: default constructor cannot be used
  vdgl_fit_lines();
};

#endif // vdgl_fit_lines_h_
