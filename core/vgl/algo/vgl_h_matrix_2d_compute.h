//--*-c++-*--
#ifndef vgl_h_matrix_2d_compute_h_
#define vgl_h_matrix_2d_compute_h_
//:
// \file
//
// Abstract interface for classes that compute plane-to-plane
// projectivities from point and line correspondences.
//
// \verbatim
// Modifications:
//   08-02-98 FSM
//      1. Added virtual compute methods that actually take arguments :
//         generic estimator using points, lines or both.
//      2. Obsoleted bool compute(vgl_h_matrix_2d<double>  *). So don't use it!
//      3. made arguments to compute method 'const ... &',
//         thereby potentially breaking the code of certain other people.
//
//  Mar 24, 2003 Modifications to move to vgl algo
// \endverbatim
#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
class vgl_h_matrix_2d_compute {
public:
  vgl_h_matrix_2d_compute() : verbose_(false) { }
  virtual ~vgl_h_matrix_2d_compute() { }

  // set this to true for verbose run-time information
  void verbose(bool v) { verbose_ = v; }

  // fsm@robots.ox.ac.uk
  virtual int minimum_number_of_correspondences() const = 0;

  // Compute methods :
  //
  // Some use point correspondences, some use line
  // correspondences, some use both. They are implemented
  // in terms of the compute_(p|l|pl) methods.

  //: homography from matched points 
  bool compute(vcl_vector<vgl_homg_point_2d<double> > const& points1,
               vcl_vector<vgl_homg_point_2d<double> > const& points2,
               vgl_h_matrix_2d<double>& h  );

 //: homography from matched lines
  bool compute(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
               vcl_vector<vgl_homg_line_2d<double> > const& lines2, 
               vgl_h_matrix_2d<double>& h);

  //: homography from matched points and lines
  bool compute(vcl_vector<vgl_homg_point_2d<double> > const& points1,
               vcl_vector<vgl_homg_point_2d<double> > const& points2,
               vcl_vector<vgl_homg_line_2d<double> > const& lines1,
               vcl_vector<vgl_homg_line_2d<double> > const& lines2,
               vgl_h_matrix_2d<double>& h);

  //: homography from matched points - return h_matrix
  vgl_h_matrix_2d<double>  
  compute(vcl_vector<vgl_homg_point_2d<double> > const& points1,
          vcl_vector<vgl_homg_point_2d<double> > const& points2);

  //: homography from matched lines - return h_matrix
  vgl_h_matrix_2d<double>  
  compute(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
          vcl_vector<vgl_homg_line_2d<double> > const& lines2);

  //: homography from matched points and lines - return h_matrix
  vgl_h_matrix_2d<double>
  compute(vcl_vector<vgl_homg_point_2d<double> > const& points1,
          vcl_vector<vgl_homg_point_2d<double> > const& points2,
          vcl_vector<vgl_homg_line_2d<double> > const& lines1,
          vcl_vector<vgl_homg_line_2d<double> > const& lines2);

protected:
  bool verbose_;
  virtual bool compute_p(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                         vcl_vector<vgl_homg_point_2d<double> > const& points2,
                         vgl_h_matrix_2d<double>& H);

  virtual bool compute_l(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                         vcl_vector<vgl_homg_line_2d<double> > const& lines2,
                         vgl_h_matrix_2d<double>& H);
  virtual 
  bool compute_pl(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                  vcl_vector<vgl_homg_point_2d<double> > const& points2,
                  vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                  vcl_vector<vgl_homg_line_2d<double> > const& lines2, 
                  vgl_h_matrix_2d<double>& H);

};

#endif // vgl_h_matrix_2d_compute_h_
