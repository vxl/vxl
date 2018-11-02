// This is core/vpgl/algo/vpgl_em_compute_5_point.hxx
#ifndef vpgl_em_compute_5_point_hxx_
#define vpgl_em_compute_5_point_hxx_
//:
// \file
#include <iostream>
#include <cstdlib>
#include "vpgl_em_compute_5_point.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_complex_eigensystem.h>
#include <vnl/algo/vnl_svd.h>

/*-----------------------------------------------------------------------*/
template <class T>
bool vpgl_em_compute_5_point<T>::compute(
        const std::vector<vgl_point_2d<T> > &right_points,
        const vpgl_calibration_matrix<T> &k_right,
        const std::vector<vgl_point_2d<T> > &left_points,
        const vpgl_calibration_matrix<T> &k_left,
        std::vector<vpgl_essential_matrix<T> > &ems) const
{
    std::vector<vgl_point_2d<T> > normed_right_points, normed_left_points;

    normalize(right_points, k_right, normed_right_points);
    normalize(left_points, k_left, normed_left_points);

    return compute(normed_right_points, normed_left_points, ems);
}


template <class T>
bool vpgl_em_compute_5_point<T>::compute(
    const std::vector<vgl_point_2d<T> > &normed_right_points,
    const std::vector<vgl_point_2d<T> > &normed_left_points,
    std::vector<vpgl_essential_matrix<T> > &ems) const
{
    // Check that we have the right number of points
    if (normed_right_points.size() != 5 || normed_left_points.size() != 5) {
        if (verbose) {
            std::cerr<<"Wrong number of input points!\n" <<
                "right_points has "<<normed_right_points.size() <<
                " and left_points has "<<normed_left_points.size() << '\n';
        }
        return false;
    }

    // Compute the null space basis of the epipolar constraint matrix
    std::vector<vnl_vector_fixed<T,9> > basis;
    compute_nullspace_basis(normed_right_points, normed_left_points, basis);

    // Using this basis, we now can compute the polynomial constraints
    // on the E matrix.
    std::vector<vnl_real_npolynomial> constraints;
    compute_constraint_polynomials(basis, constraints);

    // Find the groebner basis
    vnl_matrix<double> groebner_basis(10, 10);
    compute_groebner_basis(constraints, groebner_basis);

    // Action matrix
    vnl_matrix<double> action_matrix(10, 10);
    compute_action_matrix(groebner_basis, action_matrix);

    // Finally, use the action matrix to compute the essential matrices,
    // one possibility for each real eigenvalue of the action matrix
    compute_e_matrices(basis, action_matrix, ems);

    return true;
}


//-------------------------------------------------------------------------
//:
//Normalization is the process of left-multiplying by the inverse of the
// calibration matrix.
template <class T>
void vpgl_em_compute_5_point<T>::normalize(
    const std::vector<vgl_point_2d<T> > &points,
    const vpgl_calibration_matrix<T> &k,
    std::vector<vgl_point_2d<T> > &normed_points) const
{
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        vgl_point_2d<T> p = k.map_to_focal_plane(points[i]);
        normed_points.push_back(vgl_point_2d<T>(p.x(), p.y()));
    }
}


//:
// Constructs the 5x9 epipolar constraint matrix based off the constraint
// that q1' * E * q2 = 0, and fills the vectors x, y, z and
// w with the nullspace basis for this matrix
template <class T>
void vpgl_em_compute_5_point<T>::compute_nullspace_basis(
    const std::vector<vgl_point_2d<T> > &right_points,
    const std::vector<vgl_point_2d<T> > &left_points,
    std::vector<vnl_vector_fixed<T, 9> > &basis) const
{
    // Create the 5x9 epipolar constraint matrix
    vnl_matrix<T> A(5, 9);

    for (int i = 0; i < 5; ++i) {
        A.put(i, 0, right_points[i].x() * left_points[i].x());
        A.put(i, 1, right_points[i].y() * left_points[i].x());
        A.put(i, 2, left_points[i].x());

        A.put(i, 3, right_points[i].x() * left_points[i].y());
        A.put(i, 4, right_points[i].y() * left_points[i].y());
        A.put(i, 5, left_points[i].y());

        A.put(i, 6, right_points[i].x());
        A.put(i, 7, right_points[i].y());
        A.put(i, 8, 1.0);
    }

    // Find four vectors that span the right nullspace of the matrix.
    // Do this using SVD.
    vnl_svd<T> svd(A, tolerance);
    vnl_matrix<T> V = svd.V();

    // The null space is spanned by the last four columns of V.
    for (int i = 5; i < 9; ++i) {
        vnl_vector_fixed<T,9> basis_vector;

        for (int j = 0; j < 9; ++j) {
            basis_vector[j] = V.get(j, i);
        }

        basis.push_back(basis_vector);
    }
}


//:
// Finds 10 constraint polynomials, based on the following criteria:
// if X, Y, Z and W are the basis vectors, then
// E = xX + yY + zZ + wW for some scalars x, y, z, w. Since these are
// unique up to a scale, we say w = 1;
//
// Furthermore, det(E) = 0, and E*E'*E - .5 * trace(E*E') * E = 0.
// Substituting the original equation into all 10 of the equations
// generated by these two constraints gives us the constraint polynomials.
template <class T>
void vpgl_em_compute_5_point<T>::compute_constraint_polynomials(
    const std::vector<vnl_vector_fixed<T,9> > &basis,
    std::vector<vnl_real_npolynomial> &constraints) const
{
    // Create a polynomial for each entry of E.
    //
    // E = [e11 e12 e13] = x * [ X11 ... ...] + ...
    //     [e21 e22 e23]       [...  ... ...]
    //     [e31 e32 e33]       [...  ... ...]
    //
    // This means e11 = x * X11 + y * Y11 + z * Z11 + W11.
    // Form these polynomials. They will be used in the other constraints.
    std::vector<vnl_real_npolynomial> entry_polynomials(9);
    vnl_vector<double> coeffs(4);

    vnl_matrix<unsigned> exps(4, 4);
    exps.set_identity();
    exps.put(3, 3, 0);

    for (int i = 0; i < 9; ++i) {
        coeffs[0] = basis[0][i];
        coeffs[1] = basis[1][i];
        coeffs[2] = basis[2][i];
        coeffs[3] = basis[3][i];

        entry_polynomials[i].set(coeffs, exps);
    }

    // Create polynomials for the singular value constraint.
    // These nine equations are from the constraint
    // E*E'*E - .5 * trace(E*E') * E = 0. If you want to see these in
    // their full glory, type the following snippet into matlab
    // (not octave, won't work).
    //
    // syms a b c d e f g h i;
    // E = [a b c; d e f; g h i];
    // pretty(2*E*E'*E - trace(E*E')*E)
    //
    // The first polynomial is this:
    //  a(2*a*a+ 2*b*b+ 2*c*c)+ d(2*a*d+ 2*b*e+ 2*c*f)+ g(2*a*g+ 2*b*h+ 2*c*i)
    //  - a(a*a+b*b+c*c+d*d+e*e+f*f+g*g+h*h+i*i)
    // The other polynomials have similar forms.

    // Define a*a + b*b + c*c + d*d + e*e + f*f + g*g + h*h + i*i, a
    // term in all other constraint polynomials
    vnl_real_npolynomial sum_of_squares =
        entry_polynomials[0] * entry_polynomials[0];

    for (int i = 1; i < 9; ++i) {
        sum_of_squares = sum_of_squares +
            entry_polynomials[i] * entry_polynomials[i];
    }

    // Create the first two terms in each polynomial and add it to
    // constraints
    for (int i = 0; i < 9; ++i) {
        constraints.push_back(
            entry_polynomials[i%3] *
                (entry_polynomials[0] * entry_polynomials[3*(i/3) + 0]*2 +
                 entry_polynomials[1] * entry_polynomials[3*(i/3) + 1]*2 +
                 entry_polynomials[2] * entry_polynomials[3*(i/3) + 2]*2)

            - entry_polynomials[i] * sum_of_squares);
    }

    // Now add the next term (there are four terms total)
    for (int i = 0; i < 9; ++i) {
        constraints[i] +=
            entry_polynomials[(i%3) + 3] *
                (entry_polynomials[3] * entry_polynomials[3*(i/3) + 0]*2 +
                 entry_polynomials[4] * entry_polynomials[3*(i/3) + 1]*2 +
                 entry_polynomials[5] * entry_polynomials[3*(i/3) + 2]*2);
    }

    // Last term
    for (int i = 0; i < 9; ++i) {
        constraints[i] = (constraints[i] +
            entry_polynomials[(i%3) + 6] *
                (entry_polynomials[6] * entry_polynomials[3*(i/3) + 0]*2 +
                 entry_polynomials[7] * entry_polynomials[3*(i/3) + 1]*2 +
                 entry_polynomials[8] * entry_polynomials[3*(i/3) + 2]*2)) * .5;
    }

    // Now we are going to create a polynomial from the constraint det(E)= 0.
    // if E = [a b c; d e f; g h i], (E = [0 1 2; 3 4 5; 6 7 8]) then
    // det(E) = (ai - gc) * e +  (bg - ah) * f + (ch - bi) * d.
    // We have a through i in terms of the basis vectors from above, so
    // use those to construct a constraint polynomial, and put it into
    // constraints.

    // (bf - ec) * g = (1*5 - 4*2) * 4
    vnl_real_npolynomial det_term_1 = entry_polynomials[6] *
        (entry_polynomials[1] * entry_polynomials[5] -
         entry_polynomials[2] * entry_polynomials[4]);

    // (cd - fa) * h
    vnl_real_npolynomial det_term_2 = entry_polynomials[7] *
        (entry_polynomials[2] * entry_polynomials[3] -
         entry_polynomials[0] * entry_polynomials[5]);

    // (ae - db) * i
    vnl_real_npolynomial det_term_3 = entry_polynomials[8] *
        (entry_polynomials[0] * entry_polynomials[4] -
         entry_polynomials[1] * entry_polynomials[3]);

    constraints.push_back(det_term_1 + det_term_2 + det_term_3);
}


/*-----------------------------------------------------------------------*/
//:
// Returns the coefficient of a term of a vnl_real_npolynomial in three
// variables with an x power of x_p, a y power of y_p and a z power of z_p
template <class T>
double vpgl_em_compute_5_point<T>::get_coeff(
    const vnl_real_npolynomial &p,
    unsigned int x_p,
    unsigned int y_p,
    unsigned int z_p) const
{
    for (unsigned int i = 0; i < p.polyn().rows(); ++i) {
        if (x_p == p.polyn().get(i, 0) &&
            y_p == p.polyn().get(i, 1) &&
            z_p == p.polyn().get(i, 2)) {
            return p.coefficients()[i];
        }
    }
    return -1.0;
}

template <class T>
void vpgl_em_compute_5_point<T>::compute_groebner_basis(
    const std::vector<vnl_real_npolynomial> &constraints,
    vnl_matrix<double> &groebner_basis) const
{
    assert(groebner_basis.rows() == 10);
    assert(groebner_basis.cols() == 10);

    vnl_matrix<double> A(10, 20);

    for (int i = 0; i < 10; ++i) {
        // x3 x2y xy2 y3 x2z xyz y2z xz2 yz2 z3 x2 xy y2 xz yz z2 x  y  z  1
        A.put(i, 0, get_coeff(constraints[i], 3, 0, 0));
        A.put(i, 1, get_coeff(constraints[i], 2, 1, 0));
        A.put(i, 2, get_coeff(constraints[i], 1, 2, 0));
        A.put(i, 3, get_coeff(constraints[i], 0, 3, 0));
        A.put(i, 4, get_coeff(constraints[i], 2, 0, 1));
        A.put(i, 5, get_coeff(constraints[i], 1, 1, 1));
        A.put(i, 6, get_coeff(constraints[i], 0, 2, 1));
        A.put(i, 7, get_coeff(constraints[i], 1, 0, 2));
        A.put(i, 8, get_coeff(constraints[i], 0, 1, 2));
        A.put(i, 9, get_coeff(constraints[i], 0, 0, 3));
        A.put(i, 10, get_coeff(constraints[i], 2, 0, 0));
        A.put(i, 11, get_coeff(constraints[i], 1, 1, 0));
        A.put(i, 12, get_coeff(constraints[i], 0, 2, 0));
        A.put(i, 13, get_coeff(constraints[i], 1, 0, 1));
        A.put(i, 14, get_coeff(constraints[i], 0, 1, 1));
        A.put(i, 15, get_coeff(constraints[i], 0, 0, 2));
        A.put(i, 16, get_coeff(constraints[i], 1, 0, 0));
        A.put(i, 17, get_coeff(constraints[i], 0, 1, 0));
        A.put(i, 18, get_coeff(constraints[i], 0, 0, 1));
        A.put(i, 19, get_coeff(constraints[i], 0, 0, 0));
    }

    // Do a full Gaussian elimination
    for (int i = 0; i < 10; ++i)
    {
        // Make the leading coefficient of row i = 1
        double leading = A.get(i, i);
        A.scale_row(i, 1/leading);

        // Subtract from other rows
        for (int j = i+1; j < 10; ++j) {
            double leading2 = A.get(j, i);
            vnl_vector<double> new_row =
                A.get_row(j) - A.get_row(i) * leading2;

            A.set_row(j, new_row);
        }
    }

    // Now, do the back substitution
    for (int i = 9; i >= 0; --i) {
        for (int j = 0; j < i; ++j) {
            double scale = A.get(j, i);

            vnl_vector<double> new_row =
                A.get_row(j) - A.get_row(i) * scale;

            A.set_row(j, new_row);
        }
    }

    // Copy out results. Since the first 10*10 block of A is the
    // identity (due to the row_reduce), we are interested in the
    // second 10*10 block.
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            groebner_basis.put(i, j, A.get(i, j+10));
        }
    }
}


/*-----------------------------------------------------------------------*/
template <class T>
void vpgl_em_compute_5_point<T>::compute_action_matrix(
    const vnl_matrix<double> &groebner_basis,
    vnl_matrix<double> &action_matrix) const
{
    action_matrix.fill(0.0);

    action_matrix.set_row(0, groebner_basis.get_row(0));
    action_matrix.set_row(1, groebner_basis.get_row(1));
    action_matrix.set_row(2, groebner_basis.get_row(2));
    action_matrix.set_row(3, groebner_basis.get_row(4));
    action_matrix.set_row(4, groebner_basis.get_row(5));
    action_matrix.set_row(5, groebner_basis.get_row(7));
    action_matrix *= -1;

    action_matrix.put(6, 0, 1.0);
    action_matrix.put(7, 1, 1.0);
    action_matrix.put(8, 3, 1.0);
    action_matrix.put(9, 6, 1.0);
}


/*------------------------------------------------------------------------*/
template <class T>
void vpgl_em_compute_5_point<T>::compute_e_matrices(
    const std::vector<vnl_vector_fixed<T, 9> > &basis,
    const vnl_matrix<double> &action_matrix,
    std::vector<vpgl_essential_matrix<T> > &ems) const
{
    vnl_matrix<double> zeros(action_matrix.rows(), action_matrix.cols(), 0);
    vnl_complex_eigensystem eigs(action_matrix, zeros);

    for (unsigned int i = 0; i < eigs.W.size(); ++i) {
        if (std::abs(eigs.W[i].imag()) <= tolerance)
        {
            vnl_vector_fixed<T, 9> linear_e;

            double w_inv = 1.0 / eigs.R.get(i, 9).real();
            double x = eigs.R.get(i, 6).real() * w_inv;
            double y = eigs.R.get(i, 7).real() * w_inv;
            double z = eigs.R.get(i, 8).real() * w_inv;

            linear_e =
                x * basis[0] + y * basis[1] + z * basis[2] + basis[3];
            linear_e /= linear_e[8];
            // ignore solutions that are non-finite
            if (!linear_e.is_finite())
                continue;

            ems.push_back(vpgl_essential_matrix<T>(
                vnl_matrix_fixed<T, 3, 3>(linear_e.data_block())));
        }
    }
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
static inline void get_distinct_indices(
    int n, int *idxs, int number_entries)
{
    for (int i = 0; i < n; ++i) {
        bool found = false;
        int idx=0;

        while (!found) {
            found = true;
            idx = std::rand() % number_entries;

            for (int j = 0; j < i; ++j) {
                if (idxs[j] == idx) {
                    found = false;
                    break;
                }
            }
        }

        idxs[i] = idx;
    }
}


template <class T>
bool vpgl_em_compute_5_point_ransac<T>::compute(
    std::vector<vgl_point_2d<T> > const& right_points,
    vpgl_calibration_matrix<T> const& right_k,

    std::vector<vgl_point_2d<T> > const& left_points,
    vpgl_calibration_matrix<T> const& left_k,

    vpgl_essential_matrix<T> &best_em) const
{
    // ----- Test the input
    if ( right_points.size() != left_points.size()) {
        if (verbose) {
            std::cerr
                << "The two vectors of points must be the same size!\n"
                << "right_points is size " << right_points.size()
                << " while left_points is size " << left_points.size()
                << ".\n";
        }
        return false;
    }
    else if (right_points.size() < 5) {
        if (verbose) {
            std::cerr
                << "There need to be at least 5 points to run the "
                << "five-point algorithm!\n"
                << "Input only contained " << right_points.size()
                << " points.\n";
        }
        return false;
    }

    // ----- Good input! Do the ransac
    const size_t num_points = right_points.size();

    unsigned best_inlier_count = 0u;

    vpgl_em_compute_5_point<T> five_point;

    int match_idxs[5];
    for (unsigned int r = 0; r < num_rounds; ++r)
    {
        // Choose 5 random points, and use the 5-point algorithm on
        // these points to find the relative pose.
        std::vector<vgl_point_2d<T> > right_points_to_use;
        std::vector<vgl_point_2d<T> > left_points_to_use;

        get_distinct_indices(5, match_idxs, (int) num_points);

        for (int & match_idx : match_idxs) {
            right_points_to_use.push_back(right_points[match_idx]);
            left_points_to_use.push_back(left_points[match_idx]);
        }
        std::vector<vpgl_essential_matrix<T> > ems;
        five_point.compute(
            right_points_to_use, right_k,
            left_points_to_use, left_k,
            ems);

        // Now test all the essential matrices we've found, using them as
        // RANSAC hypotheses.
        typename std::vector<vpgl_essential_matrix<T> >::const_iterator i;
        for (i = ems.begin(); i != ems.end(); ++i) {
            vpgl_fundamental_matrix<T> f(right_k, left_k, *i);

            vnl_double_3x1 point_r, point_l;

            // Count the number of inliers
            unsigned inlier_count = 0;
            for (unsigned j = 0; j < num_points; ++j) {
                point_r.put(0, 0, right_points[j].x());
                point_r.put(1, 0, right_points[j].y());
                point_r.put(2, 0, 1.0);

                point_l.put(0, 0, left_points[j].x());
                point_l.put(1, 0, left_points[j].y());
                point_l.put(2, 0, 1.0);

                vnl_double_3x1 f_r = f.get_matrix() * point_r;
                vnl_double_3x1 f_l = f.get_matrix().transpose() * point_l;

                // compute normalized distance to line
                double p = (point_r.transpose() * f_l).get(0,0);
                double error = (1.0 / (f_l.get(0,0) * f_l.get(0,0) + f_l.get(1,0) * f_l.get(1,0)) +
                                1.0 / (f_r.get(0,0) * f_r.get(0,0) + f_r.get(1,0) * f_r.get(1,0))) * (p * p);

                if ( error <= inlier_threshold) {
                    ++inlier_count;
                }
            }

            if (best_inlier_count < inlier_count) {
                best_em = *i;
                best_inlier_count = inlier_count;
            }
        }
    }

    return true;
}


#define VPGL_EM_COMPUTE_5_POINT_INSTANTIATE(T) \
template class vpgl_em_compute_5_point<T >; \
template class vpgl_em_compute_5_point_ransac<T >

#endif // vpgl_em_compute_5_point_hxx_
