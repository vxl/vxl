// This is rpl/rrel/rrel_homography2d_est_aff.cxx
#include <rrel/rrel_homography2d_est_aff.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>


rrel_homography2d_est_aff :: rrel_homography2d_est_aff( const vcl_vector< vgl_homg_point_2d<double> > & from_pts,
                                                        const vcl_vector< vgl_homg_point_2d<double> > & to_pts )
  : rrel_homography2d_est( from_pts, to_pts, 6)
{
    // Do nothing here
}

rrel_homography2d_est_aff :: rrel_homography2d_est_aff( const vcl_vector< vnl_vector<double> > & from_pts,
                                                        const vcl_vector< vnl_vector<double> > & to_pts )
  : rrel_homography2d_est( from_pts, to_pts, 6)
{
    // Do nothing here
}

rrel_homography2d_est_aff::~rrel_homography2d_est_aff()
{
    // Do nothing here
}

void
rrel_homography2d_est_aff :: homography_to_parameters(const vnl_matrix<double>& m,
                                                      vnl_vector<double>&       p) const
{
    p(0) = m(0, 0) / m(2, 2);
    p(1) = m(0, 1) / m(2, 2);
    p(2) = m(0, 2) / m(2, 2);
    p(3) = m(1, 0) / m(2, 2);
    p(4) = m(1, 1) / m(2, 2);
    p(5) = m(1, 2) / m(2, 2);
    p(6) = 0.0;
    p(7) = 0.0;
    p(8) = 1.0;
}

void
rrel_homography2d_est_aff :: parameters_to_homography(const vnl_vector<double>& p,
                                                      vnl_matrix<double>&       m) const
{
    m(0, 0) = p(0) / p(8);
    m(0, 1) = p(1) / p(8);
    m(0, 2) = p(2) / p(8);
    m(1, 0) = p(3) / p(8);
    m(1, 1) = p(4) / p(8);
    m(1, 2) = p(5) / p(8);
    m(2, 0) = 0.0;
    m(2, 1) = 0.0;
    m(2, 2) = 1.0;
}
