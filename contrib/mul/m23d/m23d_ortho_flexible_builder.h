//:
// \file
// \author Tim Cootes
// \brief Algorithm to estimate 3D shape basis from multiple 2D views

#ifndef m2d3_ortho_flexible_builder_h_
#define m2d3_ortho_flexible_builder_h_

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>

//: Algorithm to estimate 3D shape basis from multiple 2D views.
//  ses the method of Xiao, Chai and Kanade (CVPR 2004).
//  Generates projection matrix, P, and 3D point matrix P3D, such that
//  P2D = P.P3D.  Uncertainty in projection is constrained by setting
//  the projection for the first shape to the identity (1 0 0; 0 1 0)
//
//  The ambiguity in the sign of the z ordinates is fixed by arranging that
//  the first non-zero z ordinate should be negative.
//
//  Note that the 2D point sets will be normalised so that their CoG is
//  at the origin.  The resulting 3D shape bases will also have their
//  CoG at the origin.
class m23d_ortho_flexible_builder
{
  private:
    //: 3(1+m) x np matrix, each column containing one 3D point
    vnl_matrix<double> P3D_;

    //: The 2ns x 3(1+m) projection matrix
    //  Each 2x3 sub-matrix is a scaled orthographic projection matrix
    vnl_matrix<double> P_;

    //: The CoG of each shape supplied to reconstruct()
    //  This has been subtracted from each example to centre on the origin
    //  Centred data stored in P2Dc_
    vcl_vector<vgl_point_2d<double> > cog_;

    //: Centred version of the 2D views supplied to reconstruct()
    //  Each 2D shape has been translated so that it's CoG is at the origin
    vnl_matrix<double> P2Dc_;

    //: The 2ns x 3 matrix. Each 2x3 block is the projection for a given view.
    //  Each 2x3 sub-matrix is a scaled orthographic projection matrix
    vnl_matrix<double> pure_P_;

    //: ns x (m+1) matrix, each row of which contains the weights for a shape
    vnl_matrix<double> coeffs_;

    //: Mean 3D shape as a 3 x np matrix
    vnl_matrix<double> mean_shape_;

    //: Mean coefficients
    vnl_vector<double> mean_coeffs_;

    //: Modify projection matrices so they are scaled orthographic projections
    //  P = s(I|0)*R
    void make_pure_projections();

    //: Compute the mean 3D shape
    void compute_mean(vnl_matrix<double>& mean_shape,
                       vnl_vector<double>& mean_coeffs);

  public:
    //: Reconstruct approximate structure of 3D points given multiple 2D views
    //  Data assumed to be scaled orthographic projections
    //  The result is stored in the shape_3d() matrix.
    //  The estimated projection matrices are stored in the projections() matrix
    //  However, the projection matrices are not necessarily consistent.
    //  Call refine() to ensure consistancy, (or use reconstruct(P2D,n_modes)
    //  This is exposed to aid testing and debugging.
    //  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
    void partial_reconstruct(const vnl_matrix<double>& P2D, unsigned n_modes);

    //: Reconstruct structure of 3D points given multiple 2D views
    //  Data assumed to be scaled orthographic projections
    //  The result is stored in the shape_3d() matrix.
    //  The estimated projection matrices are stored in the projections() matrix
    //  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
    void reconstruct(const vnl_matrix<double>& P2D, unsigned n_modes);

    //: Return 3(m+1) x np matrix, each column containing modes for one 3D point
    //  Points are centred on the origin.
    //  Each 3 rows contains one of the (1+m) bases.
    const vnl_matrix<double>& shape_3d() const { return P3D_; };


    //: The 2ns x 3(m+1) projection matrix
    //  Each 2x3 sub-matrix is a scaled orthographic projection matrix
    const vnl_matrix<double>& projections() const { return P_; };

    //: Centred version of the 2D views supplied to reconstruct()
    //  Each 2D shape has been translated so that it's CoG is at the origin
    const vnl_matrix<double>& centred_views() const { return P2Dc_; }

    //: The 2ns x 3 matrix. Each 2x3 block is the projection for a given view.
    //  Each 2x3 sub-matrix is a scaled orthographic projection matrix
    const vnl_matrix<double>& pure_projections() const { return pure_P_; }

    //: ns x (m+1) matrix, each row of which contains the weights for a shape
    const vnl_matrix<double>& coeffs() const { return coeffs_; }

    //: Refine estimates of projection and structure
    void refine();

    //: Apply rotation matrices to each 3 columns of P (and inverse to rows of B)
    // Matrix selected so that projection matrices in each 3 cols have same
    // effective rotation.
    void correct_coord_frame(vnl_matrix<double>& P,
                            vnl_matrix<double>& B);

    //: Compute correction matrix so that M1.G has suitable projection properties
    void compute_correction(const vnl_matrix<double>& M1,
                            vnl_matrix<double>& G);

    //: Return mean 3D shape as a 3 x np matrix
    const vnl_matrix<double>& mean_shape() const { return mean_shape_; }

    //: Mean coefficients
    const vnl_vector<double>& mean_coeffs() const { return mean_coeffs_; }
};

#endif // m2d3_ortho_flexible_builder_h_
