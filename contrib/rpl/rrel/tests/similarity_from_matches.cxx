
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

#include <rrel/tests/similarity_from_matches.h>

// Assume throughout that the format of the parameter vector is a, b, tx, ty

similarity_from_matches::similarity_from_matches( const vcl_vector<image_point_match> matches )
  : rrel_estimation_problem(2,2),
    matches_(matches)
{
  assert( matches_.size() >= 2 );

  int old_point_id = matches_[0].point_id_;
  num_points_to_match_ = 1;
  for ( unsigned int i=1; i<matches_.size(); ++i ) {
    if ( matches_[i].point_id_ != old_point_id ) {
      num_points_to_match_ ++ ;
      old_point_id = matches_[i].point_id_;
    }
  }
  assert( num_points_to_match_ >= 2 );
}

bool 
similarity_from_matches::fit_from_minimal_set( const vcl_vector<int>& match_indices,
                                               vnl_vector<double>& params ) const
{
  assert( match_indices.size() == 2 );
  if ( matches_[match_indices[0]].point_id_ == matches_[match_indices[1]].point_id_ ||
       (matches_[match_indices[0]].to_loc_ - matches_[match_indices[1]].to_loc_).squared_magnitude()
       < 1.0e-6 ) {
    vcl_cout << "identical from points or to points: returning false" << vcl_endl;
    return false;
  }

  vnl_matrix<double> X( 4, 4, 0.0 );
  vnl_vector<double> U(4);
  for ( int i=0; i<2; ++i ) {
    // vcl_cout << "i = " << i << ", match_indices[i] = " << match_indices[i] << "\n";
    // vcl_cout << "from_loc_ = " << matches_[match_indices[i]].from_loc_
    //          << ", to_loc_ = " << matches_[match_indices[i]].to_loc_ << vcl_endl;
    double x = matches_[match_indices[i]].from_loc_.x();
    double y = matches_[match_indices[i]].from_loc_.y();
    double u = matches_[match_indices[i]].to_loc_.x();
    double v = matches_[match_indices[i]].to_loc_.y();
    X( 2*i, 0 ) = X( 2*i+1, 1 ) = x;
    X( 2*i, 1 ) = -y; X( 2*i+1, 0 ) = y;
    X( 2*i, 2) = X( 2*i+1, 3 ) = 1.0;
    U( 2*i ) = u;
    U( 2*i+1 ) = v;
  }

  // vcl_cout << "X matrix = " << X << "\n"
  //          << "U vector = " << U << vcl_endl;

  vnl_svd<double> svd( X );
  if ( svd.rank() < 4 ) { 
    vcl_cout << "Rank is " << svd.rank() << ", returning false" << vcl_endl;
    return false;
  }

  params = svd.inverse() * U;
  return true;
}


unsigned int 
similarity_from_matches::num_samples( ) const
{
  return matches_.size();
}

double calc_residual( const vnl_vector<double>& params, const image_point_match& match )
{
  double from_x = match.from_loc_.x();
  double from_y = match.from_loc_.y();
  vnl_vector_fixed<double,2> trans;
  trans.x() = params[0] * from_x - params[1] * from_y + params[2];
  trans.y() = params[1] * from_x + params[0] * from_y + params[3];
  return (trans - match.to_loc_).two_norm();
}



void
similarity_from_matches::compute_residuals( const vnl_vector<double>& params,
                                            vcl_vector<double>& residuals ) const
{
  if ( residuals.size() != matches_.size() )
    residuals.resize( matches_.size() );
  for ( unsigned int i =0; i<matches_.size(); ++i )
    residuals[i] = calc_residual( params, matches_[i] );
}


void 
similarity_from_matches::compute_weights( const vcl_vector<double>& residuals,
                                          const rrel_wls_obj* obj,
                                          double scale,
                                          vcl_vector<double>& weights ) const

{
  // First compute the weights as normal.
  rrel_estimation_problem::compute_weights( residuals, obj, scale, weights );

  // Then augment them.
  unsigned int i=0;
  while ( i<matches_.size() ) {
    int start = i;
    double sum_weights = 0;
    for ( ; i<matches_.size() && matches_[start].point_id_ == matches_[i].point_id_; ++ i )
      sum_weights += weights[i];
    if ( sum_weights > 0 )
      for ( unsigned int j=start; j<i; ++j )
        weights[j] = weights[j] * weights[j] / sum_weights;
  }
}

bool 
similarity_from_matches::weighted_least_squares_fit( vnl_vector<double>& params,
                                                     vnl_matrix<double>& cofact,
                                                     vcl_vector<double>* weights ) const
{
  assert( params.size() == 4 );

  vnl_matrix<double> wXtX( 4, 4, 0.0 );
  vnl_matrix<double> Xi( 2, 4 );
  Xi( 0, 2 ) = Xi( 1, 3) = 1;
  Xi( 0, 3 ) = Xi( 1, 2) = 0;
  vnl_vector<double> wXtu(4, 0.0);

  for ( unsigned int i=0; i<matches_.size(); ++i ) {
    // vcl_cout << "i = " << i << ". from_loc_ = " << matches_[i].from_loc_
    //          << ", to_loc_ = " << matches_[i].to_loc_ << vcl_endl;
    Xi( 0, 0 ) = Xi( 1, 1 ) = matches_[i].from_loc_.x(); 
    Xi( 1, 0 ) = matches_[i].from_loc_.y();
    Xi( 0, 1 ) = -Xi(1, 0 );

    // vcl_cout << "Xi = " << Xi << vcl_endl;
    wXtX += (*weights)[i] * Xi.transpose() * Xi;
    wXtu += (*weights)[i] * Xi.transpose() * matches_[i].to_loc_;
  }

  // vcl_cout << "wXtX matrix = " << wXtX << "\n"
  //          << "wXtu vector = " << wXtu << vcl_endl;

  vnl_svd<double> svd( wXtX );
  if ( svd.rank() < 4 ) { 
    vcl_cout << "Rank is " << svd.rank() << ", returning false" << vcl_endl;
    return false;
  }

  params = svd.inverse() * wXtu;
  cofact = svd.inverse();
  if ( vnl_math_abs(params[0]) < 1.0e-6 && vnl_math_abs(params[1]) < 1.0e-6 ) {
    vcl_cout << "Transformation is degenerate" << vcl_endl;
    return false;
  }
  return true;
}


