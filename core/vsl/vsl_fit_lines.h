#ifndef vsl_fit_lines_h
#define vsl_fit_lines_h
// .NAME vsl_fit_lines
// .INCLUDE vsl/vsl_fit_lines.h
// .FILE vsl_fit_lines.cxx
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
//       JLM - Jan 1996 Changed _curves type from DigitalCurve
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
//         JLM - Dec 97 Moved some member variables to vsl_fit_lines_params
//                      added group processing.  Retained old constructors
//                      and methods for compatibility
//         capes Aug 1999 Added ignore_end_edgels parameter which allows user to 
//                        crop a small number of edgels from the ends of a segment
//                        before fitting the line. Avoids fitting to the garbage edgels
//                        which are curving away from the line.
//-----------------------------------------------------------------------------
#include <vcl/vcl_list.h>
#include <vsl/vsl_fit_lines_params.h>
#include <vsl/vsl_topology.h>
#include <vsl/vsl_canny_port.h>
#include <vsl/vsl_OrthogRegress.h>

class vsl_fit_lines : public vsl_fit_lines_params {
public:
  vsl_fit_lines(vsl_fit_lines_params const & params, double scale=1.0, double x0=0.0, double y0=0.0);
  ~vsl_fit_lines();


  inline void SetSqrOrMean(bool sm)    { _use_square_fit = sm; }
  inline void SetMinFitLength(int l)   { _min_fit_length = l; }
  inline void SetThreshold(double th)  { _threshold = th;  }
  inline void SetTheta(double theta)   {  _theta = theta;  }
  inline void SetIgnoreEndEdgels(int ignore) {_ignore_end_edgels = ignore; }

  // NB. these have the undesirable behaviour of deleting the given edges.
  // beware of future changes. use at own risk. complain to fsm@robots.ox.ac.uk
  void incremental_fit_to_list(vcl_list<vsl_Edge *> *in, 
			       vcl_list<vsl_Edge *> *out);
  void simple_fit_to_list(vcl_list<vsl_Edge *> *in, 
			  vcl_list<vsl_Edge *> *out);
  
//private:
  void SquareIncrementalFit(vcl_list<vsl_EdgelChain*> *_curves, vsl_Edge*);
  void MeanIncrementalFit(vcl_list<vsl_EdgelChain*> *_curves, vsl_Edge*);
  void OutputLine(vcl_list<vsl_EdgelChain*> *_curves, int,int, vsl_EdgelChain*,float);
  void MergeLines(vcl_list<vsl_EdgelChain*> *_curves);
  static float MyGetCost(vsl_OrthogRegress const *, int, int, vsl_EdgelChain *);

  vsl_OrthogRegress *_data;
  int _old_finish;
};

#endif
