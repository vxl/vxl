// This is core/vgl/algo/vgl_h_matrix_2d_optimize.h
#ifndef vgl_h_matrix_2d_optimize_h_
#define vgl_h_matrix_2d_optimize_h_
//:
// \file
// \author J.L. Mundy Jan. 5, 2005
// \brief Refine an initial 2d homography by minimizing projection error
//
// Abstract interface for classes that optimize plane-to-plane
// projectivities from point and line correspondences.
//
// \verbatim
//  Modifications None
// \endverbatim
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h> // for vcl_sqrt
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

// Note that the least squares function is over-parametrized
// with 9 parameters rather than the minimum 8. See Hartley and Zisserman
// p. 94.
class projection_lsqf : public vnl_least_squares_function
{
  unsigned n_;
  vcl_vector<vgl_homg_point_2d<double> > from_points_;
  vcl_vector<vgl_point_2d<double> > to_points_;

 public:
  projection_lsqf(vcl_vector<vgl_homg_point_2d<double> > const& from_points,
                  vcl_vector<vgl_homg_point_2d<double> > const& to_points)
  : vnl_least_squares_function(9, 2*from_points.size(), no_gradient)
  {
    n_ = from_points.size();
    assert(n_==to_points.size());
    for (unsigned i = 0; i<n_; ++i)
    {
      from_points_.push_back(from_points[i]);
      to_points_.push_back(to_points[i]);
    }
  }

  ~projection_lsqf() {}

  //: compute the projection error given a set of h parameters.
  // The residuals required by f are the Euclidean x and y coordinate
  // differences between the projected from points and the
  // corresponding to points.
  void f(const vnl_vector<double>& hv, vnl_vector<double>& proj_err)
  {
    assert(hv.size()==9);
    assert(proj_err.size()==2*n_);
    // project and compute residual
    vgl_h_matrix_2d<double> h(hv.data_block());
    unsigned k = 0;
    for (unsigned i = 0; i<n_; ++i, k+=2)
    {
      vgl_homg_point_2d<double> to_proj = h(from_points_[i]);
      vgl_point_2d<double> p_proj(to_proj);
      double xp = to_points_[i].x(), yp = to_points_[i].y();
      double xproj = p_proj.x(), yproj = p_proj.y();
      proj_err[k]=(xp-xproj);  proj_err[k+1]=(yp-yproj);
    }
  }
};


class vgl_h_matrix_2d_optimize
{
 public:
  vgl_h_matrix_2d_optimize(vgl_h_matrix_2d<double> const& initial_h) 
    : verbose_(false), trace_(false), ftol_(1e-9), gtol_(1e-9),
    htol_(1e-9), max_iter_(2000), initial_h_(initial_h){}

  virtual ~vgl_h_matrix_2d_optimize() {}

  //: set this to true for verbose run-time information
  void set_verbose(bool v) { verbose_ = v; }

  //: Termination tolerance on the gradient of the projection error
  void set_trace(bool trace){trace_ = trace;}

  //: Termination tolerance on change in the solution
  void set_htol(const double htol){htol_ = htol;}

  //: Termination tolerance on the sum of squared projection errors
  // The optimization is done in a normalized coordinate frame with
  // unity point domain radius.
  void set_ftol(const double ftol){ftol_ = ftol;}

  //: Termination tolerance on the gradient of the projection error
  void set_gtol(const double gtol){gtol_ = gtol;}

  virtual int minimum_number_of_correspondences() const = 0;

  // Optimize methods :
  //
  // Some use point correspondences, some use line
  // correspondences, some use both. They are implemented
  // in terms of the pure virtual optimize_(p|l|pl) methods.

  //: optimize homography from matched points
  bool optimize(vcl_vector<vgl_homg_point_2d<double> > const& points1,
               vcl_vector<vgl_homg_point_2d<double> > const& points2,
               vgl_h_matrix_2d<double>& H)
  {
    return optimize_p(points1, points2, H);
  }

 //: optimize homography from matched lines
  bool optimize(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
               vcl_vector<vgl_homg_line_2d<double> > const& lines2,
               vgl_h_matrix_2d<double>& H)
  {
    return optimize_l(lines1, lines2, H);
  }

  //: optimize homography from matched points and lines
  bool optimize(vcl_vector<vgl_homg_point_2d<double> > const& points1,
               vcl_vector<vgl_homg_point_2d<double> > const& points2,
               vcl_vector<vgl_homg_line_2d<double> > const& lines1,
               vcl_vector<vgl_homg_line_2d<double> > const& lines2,
               vgl_h_matrix_2d<double>& H)
  {
    return optimize_pl(points1, points2, lines1, lines2, H);
  }

  //: optimize homography from matched points - return h_matrix
  vgl_h_matrix_2d<double>
  optimize(vcl_vector<vgl_homg_point_2d<double> > const& p1,
          vcl_vector<vgl_homg_point_2d<double> > const& p2)
  { vgl_h_matrix_2d<double> H; optimize_p(p1, p2, H); return H; }

  //: optimize homography from matched lines - return h_matrix
  vgl_h_matrix_2d<double>
  optimize(vcl_vector<vgl_homg_line_2d<double> > const& l1,
          vcl_vector<vgl_homg_line_2d<double> > const& l2)
  { vgl_h_matrix_2d<double> H; optimize_l(l1, l2, H); return H; }

  //: optimize homography from matched points and lines - return h_matrix
  vgl_h_matrix_2d<double>
  optimize(vcl_vector<vgl_homg_point_2d<double> > const& p1,
          vcl_vector<vgl_homg_point_2d<double> > const& p2,
          vcl_vector<vgl_homg_line_2d<double> > const& l1,
          vcl_vector<vgl_homg_line_2d<double> > const& l2)
  { vgl_h_matrix_2d<double>  H; optimize_pl(p1, p2, l1, l2, H); return H; }

 protected:
  bool verbose_;
  bool trace_;
  double ftol_;
  double gtol_;
  double htol_;
  int max_iter_;
  vgl_h_matrix_2d<double> initial_h_;
  virtual bool optimize_p(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                         vcl_vector<vgl_homg_point_2d<double> > const& points2,
                         vgl_h_matrix_2d<double>& H) = 0;

  virtual bool optimize_l(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                         vcl_vector<vgl_homg_line_2d<double> > const& lines2,
                         vgl_h_matrix_2d<double>& H) = 0;

  virtual bool optimize_pl(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                          vcl_vector<vgl_homg_point_2d<double> > const& points2,
                          vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                          vcl_vector<vgl_homg_line_2d<double> > const& lines2,
                          vgl_h_matrix_2d<double>& H) = 0;
};

#endif // vgl_h_matrix_2d_optimize_h_
