#ifndef vgl_h_matrix_2d_optimize_lmq_h_
#define vgl_h_matrix_2d_optimize_lmq_h_
//:
// \file
// \author  J.L. Mundy Jan 05, 2005
// \brief contains class vgl_h_matrix_2d_optimize_lmq
//
// vgl_h_matrix_2d_optimize_lmq uses the Levenberg-Marquardt algorithm
// to refine an initial value for vgl_h_matrix_2d, given a set of corresponding
// points or lines.  The number of points/lines must be greater than four,
// since for four or fewer points/lines the projective mapping is exact.
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vgl/algo/vgl_h_matrix_2d_optimize.h>

class vgl_h_matrix_2d_optimize_lmq : public vgl_h_matrix_2d_optimize
{
 public:
  //:Constructor from initial homography to be optimized
  vgl_h_matrix_2d_optimize_lmq(vgl_h_matrix_2d<double> const& initial_h);
 protected:
  //internal utilities
  //:the main routine for carrying out the optimization. (used by the others)
  bool optimize_h(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                  vcl_vector<vgl_homg_point_2d<double> > const& points2,
                  vgl_h_matrix_2d<double> const& h_initial, 
                  vgl_h_matrix_2d<double>& h_optimized);

  //: compute from matched points
  virtual
  bool optimize_p(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                 vcl_vector<vgl_homg_point_2d<double> > const& points2,
                 vgl_h_matrix_2d<double>& H);

  //:compute from matched lines
  virtual
  bool optimize_l(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                 vcl_vector<vgl_homg_line_2d<double> > const& lines2,
                 vgl_h_matrix_2d<double>& H);

  //:compute from matched points and lines
  virtual
  bool optimize_pl(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                  vcl_vector<vgl_homg_point_2d<double> > const& points2,
                  vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                  vcl_vector<vgl_homg_line_2d<double> > const& lines2,
                  vgl_h_matrix_2d<double>& H);

 public:
  int minimum_number_of_correspondences() const { return 5; }
};

#endif // vgl_h_matrix_2d_optimize_lmq_h_
