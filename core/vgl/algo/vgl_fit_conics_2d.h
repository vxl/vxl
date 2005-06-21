// This is core/vgl/algo/vgl_fit_conics_2d.h
#ifndef vgl_fit_conics_2d_h_
#define vgl_fit_conics_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Fits a contiguous set of conic segments to a sampled curve
// \author J.L. Mundy 
// \date June 18, 2005
//
//  The parameters are:
//  -  min_length - the smallest number of points to fit with an conic
//  -  tol - the threshold on mean square distance from points to the conic
//  -  line_thresh - threshold for prefering a line over an conic
//
//  An conic segment is incrementally fit to the curve until the tolerance
//  is exceeded. When the tolerance is exceeded, the conic segment is 
//  output and a new conic fit is started.
//
// \verbatim
//  Modifications
//   none
// \endverbatim
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_conic_segment_2d.h>

template <class T>
class vgl_fit_conics_2d
{
  // Data Members--------------------------------------------------------------
 protected:
  vcl_vector<vgl_point_2d<T> > curve_;
  vcl_vector<vgl_conic_segment_2d<T> > segs_;
  unsigned int min_length_;
  T tol_;
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  vgl_fit_conics_2d(const unsigned min_length = 10,
                    const T tol = 0.01);

  ~vgl_fit_conics_2d(){};

  // Operations----------------------------------------------------------------

  //: set parameters
  void set_min_fit_length(const unsigned min_fit_length){min_length_ = min_fit_length;}
  void set_rms_error_tol(const T rms_error_tol){tol_ = rms_error_tol;}

  //: add a point to the curve
  void add_point(vgl_point_2d<T> const &p);
  void add_point(const T x, const T y);

  //: add an entire curve
  void add_curve(vcl_vector<vgl_point_2d<T> > const & curve){curve_=curve;}

  //: clear internal data
  void clear();

  //: the fitting method
  bool fit();

  // Data Access---------------------------------------------------------------
  vcl_vector<vgl_point_2d<T> >& get_points(){return curve_;}
  vcl_vector<vgl_conic_segment_2d<T> >& get_conic_segs(){return segs_;}

 protected:
  //:output an conic that fits from start to end
  void output(const unsigned start_index, const unsigned end_index,
              vgl_conic<T> const& conic);
};

#define VGL_FIT_CONICS_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_conics_2d.txx first"

#endif // vgl_fit_conics_2d_h_
