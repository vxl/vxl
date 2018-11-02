// This is core/vgl/algo/vgl_h_matrix_3d_compute.h
#ifndef vgl_h_matrix_3d_compute_h_
#define vgl_h_matrix_3d_compute_h_
//:
// \file
// \brief contains class vgl_h_matrix_3d_compute
// \author Ozge C. Ozcanli
// \date June 24, 2010
//
// Abstract interface for classes that compute projective transformations
// from point correspondences.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>

class vgl_h_matrix_3d_compute
{
 public:
  vgl_h_matrix_3d_compute() : verbose_(false) {}
  virtual ~vgl_h_matrix_3d_compute() = default;

  // set this to true for verbose run-time information
  void verbose(bool v) { verbose_ = v; }

  virtual int minimum_number_of_correspondences() const = 0;

  // Compute methods :
  //
  // They are implemented in terms of the pure virtual compute_ methods.

  //: homography from matched points
  bool compute(std::vector<vgl_homg_point_3d<double> > const& points1,
               std::vector<vgl_homg_point_3d<double> > const& points2,
               vgl_h_matrix_3d<double>& H)
  {
    return compute_p(points1, points2, H);
  }

  //: homography from matched points - return h_matrix
  vgl_h_matrix_3d<double>
  compute(std::vector<vgl_homg_point_3d<double> > const& p1,
          std::vector<vgl_homg_point_3d<double> > const& p2)
  { vgl_h_matrix_3d<double> H; compute_p(p1, p2, H); return H; }

 protected:
  bool verbose_;
  virtual bool compute_p(std::vector<vgl_homg_point_3d<double> > const& points1,
                         std::vector<vgl_homg_point_3d<double> > const& points2,
                         vgl_h_matrix_3d<double>& H) = 0;
};

#endif // vgl_h_matrix_3d_compute_h_
