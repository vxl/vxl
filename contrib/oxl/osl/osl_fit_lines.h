#ifndef osl_fit_lines_h
#define osl_fit_lines_h
// .NAME osl_fit_lines
// .INCLUDE osl/osl_fit_lines.h
// .FILE osl_fit_lines.cxx
//
// .SECTION Description
// Definition of a class for fitting lines to Canny edge data
// using orthognal regression and incremental fitting.
// The input data comes from a DigitalCurve pointed to by an edge.
// If a fit is found the DigitalCurve is replaced by suitable
// ImplicitDigitalLine, and the topology adjusted.
//
// .SECTION Author:
//       Charlie Rothwell - 29/1/94
//       GE Corporate Research and Development
//
// .SECTION Modifications:
//       20/4/95 CAR: The main call to IncrementalFittoList now has the
//                    optional flags which enables/disables fitting to curves
//                    with previously defined geometries which are not
//                    DigitalCurves. Refitting is the default option.
//
//       JLM - Jan 1996 Changed curves_ type from DigitalCurve
//                      to Curve to reflect restructuring of
//                      ImplicitDigitalLine, ImplicitDigitalEllipse
//
//         27/3/96 CAR: Introduction of pre-conditioning of the data points
//                      prior to fitting. For OrthogRegress for normalization
//                      information.
//         Jan 97 DMR: Added a function that will perform simple
//                      (non-incremental) fitting which can be used in
//                      conjunction with the breaklines code. Also added
//                      code to fit using mean absolute error as cutoff.
//         6 June 97:  In method IncrementalFittoList corrected protection
//                     count ( Dorin Ungureanu)
//         JLM - Dec 97 Moved some member variables to osl_fit_lines_params
//                      added group processing.  Retained old constructors
//                      and methods for compatibility
//         capes Aug 1999 Added ignore_end_edgels parameter which allows user to
//                        crop a small number of edgels from the ends of a segment
//                        before fitting the line. Avoids fitting to the garbage edgels
//                        which are curving away from the line.
//-----------------------------------------------------------------------------
#include <vcl_list.h>
#include <osl/osl_fit_lines_params.h>
#include <osl/osl_topology.h>
#include <osl/osl_canny_port.h>
#include <osl/osl_OrthogRegress.h>

class osl_fit_lines : public osl_fit_lines_params
{
 public:
  osl_fit_lines(osl_fit_lines_params const & params, double scale=1.0, double x0=0.0, double y0=0.0);
  ~osl_fit_lines();

  inline void SetSqrOrMean(bool sm)    { use_square_fit_ = sm; }
  inline void SetMinFitLength(unsigned int l)   { min_fit_length_ = l; }
  inline void SetThreshold(double th)  { threshold_ = th;  }
  inline void SetTheta(double theta)   {  theta_ = theta;  }
  inline void SetIgnoreEndEdgels(unsigned int ignore) {ignore_end_edgels_ = ignore; }

  // NB. these have the undesirable behaviour of deleting the given edges.
  // beware of future changes. use at own risk. complain to fsm
  void incremental_fit_to_list(vcl_list<osl_edge *> *in,
                               vcl_list<osl_edge *> *out);
  void simple_fit_to_list(vcl_list<osl_edge *> *in,
                          vcl_list<osl_edge *> *out);

 //private:
  void SquareIncrementalFit(vcl_list<osl_edgel_chain*> *curves_, osl_edge*);
  void MeanIncrementalFit(vcl_list<osl_edgel_chain*> *curves_, osl_edge*);
  void OutputLine(vcl_list<osl_edgel_chain*> *curves_, int,int, osl_edgel_chain*,float);
  void MergeLines(vcl_list<osl_edgel_chain*> *curves_);
  static float MyGetCost(osl_OrthogRegress const *, int, int, osl_edgel_chain *);
 private:
  osl_OrthogRegress *data_;
  int old_finish_;
};

#endif // osl_fit_lines_h
