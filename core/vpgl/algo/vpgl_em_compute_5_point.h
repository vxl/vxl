// This is core/vpgl/algo/vpgl_em_compute_5_point.h
#ifndef vpgl_em_compute_5_point_h_
#define vpgl_em_compute_5_point_h_
//:
// \file
// \brief The 5 point algorithm as described by David Nister for computing an essential matrix from point correspondences.
// \author Noah Snavely, ported to VXL by Andrew Hoelscher
// \date April 24, 2011
//
// \verbatim
//  Modifications
//      August 31, 2011  Andrew Hoelscher   Added a ransac routine
// \endverbatim

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_real_npolynomial.h>

#include <vgl/vgl_point_2d.h>

#include <vpgl/vpgl_essential_matrix.h>
#include <vpgl/vpgl_calibration_matrix.h>


template <class T>
class vpgl_em_compute_5_point
{
  public:

    vpgl_em_compute_5_point(): verbose(false), tolerance(0.0001) { }
    vpgl_em_compute_5_point(bool v): verbose(v), tolerance(0.0001) { }
    vpgl_em_compute_5_point(bool v, double t): verbose(v), tolerance(t) { }

    //: Compute from two sets of corresponding points.
    // Puts the resulting matrix into em, returns true if successful.
    // Each of right_points and left_points must contain exactly 5 points!
    // This function returns a set of potential solutions, generally 10.
    // Each of these solutions is appropriate to use as RANSAC hypthosis.
    //
    // The points must be normalized!! Use the function below to avoid
    // normalizing the points yourself.
    bool compute( const std::vector<vgl_point_2d<T> > &normed_right_points,
                  const std::vector<vgl_point_2d<T> > &normed_left_points,
                  std::vector<vpgl_essential_matrix<T> > &ems) const;

    //Same as above, but performs the normalization using the two
    // calibration matrices.
    bool compute( const std::vector<vgl_point_2d<T> > &right_points,
                  const vpgl_calibration_matrix<T> &k_right,
                  const std::vector<vgl_point_2d<T> > &left_points,
                  const vpgl_calibration_matrix<T> &k_left,
                  std::vector<vpgl_essential_matrix<T> > &ems) const;

  protected:
    const bool verbose;
    const double tolerance;

    void normalize(
        const std::vector<vgl_point_2d<T> > &points,
        const vpgl_calibration_matrix<T> &k,
        std::vector<vgl_point_2d<T> > &normed_points) const;

    void compute_nullspace_basis(
        const std::vector<vgl_point_2d<T> > &right_points,
        const std::vector<vgl_point_2d<T> > &left_points,
        std::vector<vnl_vector_fixed<T, 9> > &basis) const;

    void compute_constraint_polynomials(
        const std::vector<vnl_vector_fixed<T,9> > &basis,
        std::vector<vnl_real_npolynomial> &constraints) const;

    void compute_groebner_basis(
        const std::vector<vnl_real_npolynomial> &constraints,
        vnl_matrix<double> &groebner_basis) const;

    void compute_action_matrix(
        const vnl_matrix<double> &groebner_basis,
        vnl_matrix<double> &action_matrix) const;

    void compute_e_matrices(
        const std::vector<vnl_vector_fixed<T, 9> > &basis,
        const vnl_matrix<double> &action_matrix,
        std::vector<vpgl_essential_matrix<T> > &ems) const;

    double get_coeff(
        const vnl_real_npolynomial &p,
        unsigned int x_p, unsigned int y_p, unsigned int z_p) const;
};


template <class T>
class vpgl_em_compute_5_point_ransac
{
    public:
        vpgl_em_compute_5_point_ransac() :
            num_rounds(512u), inlier_threshold(2.25), verbose(false) { }

        vpgl_em_compute_5_point_ransac(unsigned nr, double trsh, bool v) :
            num_rounds(nr), inlier_threshold(trsh), verbose(v) { }

    bool compute(
        std::vector<vgl_point_2d<T> > const& right_points,
        vpgl_calibration_matrix<T> const& right_k,
        std::vector<vgl_point_2d<T> > const& left_points,
        vpgl_calibration_matrix<T> const& left_k,

        vpgl_essential_matrix<T> &best_em) const;


    private:
        const unsigned num_rounds;
        const double inlier_threshold;
        const bool verbose;
};

#endif // vpgl_em_compute_5_point_h_
