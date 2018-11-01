// This is core/vgl/algo/vgl_fit_lines_2d.h
#ifndef vgl_fit_lines_2d_h_
#define vgl_fit_lines_2d_h_
//:
// \file
// \brief Fits a contiguous set of line segments to a sampled curve
// \author J.L. Mundy (reminiscent of Charlie's fit lines)
// \date April 08, 2003
//
//  The parameters are:
//  -  min_length - the smallest number of points to fit with a line seg
//  -  tol - the threshold on mean square distance from points to line seg
//  -  angle - the maximum angle between segments that could be merged
//  A line segment is incrementally fit to the curve until the tolerance
//  is exceeded. The line segment is output and a new line fit is started.
//
// \verbatim
//  Modifications
//   none
// \endverbatim
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>

template <class T>
class vgl_fit_lines_2d
{
  // Data Members--------------------------------------------------------------
 protected:
  bool verbose_;
  std::vector<vgl_point_2d<T> > curve_;
  std::vector<vgl_line_segment_2d<T> > segs_;
  std::vector<int> curve_indices_;
  unsigned int min_length_;
  T tol_;
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  vgl_fit_lines_2d(unsigned int min_length = 10, T tol = 0.15);

  ~vgl_fit_lines_2d() = default;

  // Operations----------------------------------------------------------------
  void set_verbose(bool verbose){verbose_ = verbose;}
  //: set parameters
  void set_min_fit_length(unsigned int min_fit_length){min_length_ = min_fit_length;}
  void set_rms_error_tol(T rms_error_tol){tol_ = rms_error_tol;}

  //: add a point to the curve
  void add_point(vgl_point_2d<T> const &p);
  void add_point(T x, T y);

  //: add an entire curve
  void add_curve(std::vector<vgl_point_2d<T> > const & curve){curve_=curve;}

  //: clear internal data
  void clear();

  //: the fitting method
  bool fit();

  // Data Access---------------------------------------------------------------
  std::vector<vgl_point_2d<T> >& get_points(){return curve_;}
  std::vector<vgl_line_segment_2d<T> >& get_line_segs(){return segs_;}
  //: This vector provides an index mapping each curve point to the line it belongs to
  //  An index of -1 indicates the curve point was not used in any line estimate
  std::vector<int>& get_indices() {return curve_indices_;}
 protected:
  //:output a line that fits from start to end
  void output(unsigned int start_index, unsigned int end_index);
};

#define VGL_FIT_LINES_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_lines_2d.hxx instead"

#endif // vgl_fit_lines_2d_h_
