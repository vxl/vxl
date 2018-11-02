//:
// \file
// \author Tim Cootes
// \brief Algorithm to estimate 3D shape basis from multiple 2D views

#ifndef m2d3_ortho_flexible_builder_h_
#define m2d3_ortho_flexible_builder_h_

#include <iostream>
#include <vector>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_3d.h>
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
    std::vector<vgl_point_2d<double> > cog_;

    //: Centred version of the 2D views supplied to reconstruct()
    //  Each 2D shape has been translated so that its CoG is at the origin
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

    //: Take copy of 2D points and remove CoG from each
    void set_view_data(const vnl_matrix<double>& P2D);

    //: Decompose centred view data to get initial estimate of shape/projection
    //  Uncertain up to an affine transformation
    void initial_decomposition(unsigned n_modes);

    //: Disambiguate the ambiguity in the sign of the z ordinates
    // First non-zero element should be negative.
    void disambiguate_z();

    //: Modify projection matrices so they are scaled orthographic projections
    //  P = s(I|0)*R
    void make_pure_projections();

    //: Compute the mean 3D shape
    void compute_mean(vnl_matrix<double>& mean_shape,
                      vnl_vector<double>& mean_coeffs);

    //: Return 3d pts given 3*np matrix
    void mat_to_3d_pts(std::vector< vgl_point_3d<double> >& pts,
                       const vnl_matrix<double>& M) const;

 public:

    //: Reconstruct structure from set of 2d pts
    // formulates measurement matrix P2D then calls reconstruct() function
    void reconstruct(const std::vector< std::vector< vgl_point_2d<double> > >& pt_vec_list,
                     const unsigned& n_modes );


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
    //  The first (n_modes+1) views are assumed to define a basis for the
    //  modes.  This might not be ideal.  Use reconstruct() to automatically
    //  select views which form a good basis.
    //  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
    void reconstruct_with_first_as_basis(const vnl_matrix<double>& P2D, unsigned n_modes);

    //: Reconstruct structure of 3D points given multiple 2D views
    //  Data assumed to be scaled orthographic projections
    //  The result is stored in the shape_3d() matrix.
    //  The estimated projection matrices are stored in the projections() matrix
    //  Automatically select views which form a good basis.
    //  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
    void reconstruct(const vnl_matrix<double>& P2D, unsigned n_modes);

    //: Return 3(m+1) x np matrix, each column containing modes for one 3D point
    //  Points are centred on the origin.
    //  Each 3 rows contains one of the (1+m) bases.
    const vnl_matrix<double>& shape_3d() const { return P3D_; }

    //: The 2ns x 3(m+1) projection matrix
    //  Each 2x3 sub-matrix is a scaled orthographic projection matrix
    const vnl_matrix<double>& projections() const { return P_; }

    //: Centred version of the 2D views supplied to reconstruct()
    //  Each 2D shape has been translated so that its CoG is at the origin
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

    //: Return 3D shape i as a 3 x np matrix
    vnl_matrix<double> shape(unsigned i) const;

    //: Mean coefficients
    const vnl_vector<double>& mean_coeffs() const { return mean_coeffs_; }

#if 0
    //: Get back 3d pts rotated and shifted for each frame
    void recon_shapes(std::vector< std::vector< vgl_point_3d<double> > >& pt_vec_list ) const;
#endif // 0

    //: Return 3d pts given 3*np matrix
    void get_shape_3d_pts( std::vector< vgl_point_3d<double> >& pts ) const;
};

#endif // m2d3_ortho_flexible_builder_h_
