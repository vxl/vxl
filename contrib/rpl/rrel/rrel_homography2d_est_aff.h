#ifndef rrel_homography2d_est_aff_h_
#define rrel_homography2d_est_aff_h_

#include <rrel/rrel_homography2d_est.h>

//: Subclass of the generalized 8-DOF homography estimator for affine
//  transformations (6 DOF).


class rrel_homography2d_est_aff : public rrel_homography2d_est
{
public:

  //: Constructor from vgl_homg_point_2d's
  rrel_homography2d_est_aff( const vcl_vector< vgl_homg_point_2d<double> > & from_pts,
                             const vcl_vector< vgl_homg_point_2d<double> > & to_pts );

  //: Constructor from vnl_vectors
  rrel_homography2d_est_aff( const vcl_vector< vnl_vector<double> > & from_pts,
                             const vcl_vector< vnl_vector<double> > & to_pts );

  //: Destructor.
  virtual ~rrel_homography2d_est_aff();

  //: Convert a homography to a linear parameter list (for estimation).
  virtual void  homography_to_parameters(const vnl_matrix<double>&  m,
                                         vnl_vector<double>&        p) const;

  //: Convert a linear parameter list (from estimation) to a homography.
  virtual void  parameters_to_homography(const vnl_vector<double>&  p,
                                         vnl_matrix<double>&        m) const;
};

#endif // rrel_homography2d_est_aff_h_
