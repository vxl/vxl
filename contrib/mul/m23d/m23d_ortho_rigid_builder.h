//:
// \file
// \author Tim Cootes
// \brief Implementation of the Tomasi & Kanade reconstruction algorithm

#ifndef m2d3_ortho_rigid_builder_h_
#define m2d3_ortho_rigid_builder_h_

#include <vnl/vnl_matrix.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>

//: Reconstruct 3D point positions given multiple 2D orthographic projections.
//  Implementation of the Tomasi & Kanade reconstruction algorithm.
//  Generates projection matrix, P, and 3D point matrix P3D, such that
//  P2D = P.P3D.  Uncertainty in projection is constrained by setting
//  the projection for the first shape to the identity (1 0 0; 0 1 0)
//
//  The ambiguity in the sign of the z ordinates is fixed by arranging that
//  the first non-zero z ordinate should be negative.
//
//  Note that the 2D point sets will be normalised so that their CoG is
//  at the origin.  The resulting 3D shape will also have its CoG at the origin.
class m23d_ortho_rigid_builder
{
  private:
    //: 3 x np matrix, each column containing one 3D point
    vnl_matrix<double> P3D_;

    //: The 2ns x 3 projection matrix
    //  Each 2x3 sub-matrix is a scaled orthographic projection matrix
    vnl_matrix<double> P_;

    //: The CoG of each shape supplied to reconstruct()
    //  This has been subtracted from each example to centre on the origin
    //  Centred data stored in P2Dc_
    vcl_vector<vgl_point_2d<double> > cog_;

    //: Centred version of the 2D views supplied to reconstruct()
    //  Each 2D shape has been translated so that it's CoG is at the origin
    vnl_matrix<double> P2Dc_;

    //: Modify projection matrices so they are scaled orthographic projections
    //  P = s(I|0)*R
    void make_pure_projections();

  public:
    //: Reconstruct structure of 3D points given multiple 2D views
    //  Data assumed to be scaled orthographic projections
    //  The result is stored in the shape_3d() matrix.
    //  The estimated projection matrices are stored in the projections() matrix
    //  \param P2D 2ns x np matrix. Rows contain alternating x's and y's from 2D shapes
    void reconstruct(const vnl_matrix<double>& P2D);

    //: Return 3 x np matrix, each column containing one 3D point
    //  Points are centred on the origin
    const vnl_matrix<double>& shape_3d() const { return P3D_; };


    //: The 2ns x 3 projection matrix
    //  Each 2x3 sub-matrix is a scaled orthographic projection matrix
    const vnl_matrix<double>& projections() const { return P_; };

    //: Centred version of the 2D views supplied to reconstruct()
    //  Each 2D shape has been translated so that it's CoG is at the origin
    const vnl_matrix<double>& centred_views() const { return P2Dc_; }

    //: Refine estimates of projection and structure
    void refine();
};

#endif // m2d3_ortho_rigid_builder_h_
