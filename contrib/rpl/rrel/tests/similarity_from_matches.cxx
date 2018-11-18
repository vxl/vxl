#include <iostream>
#include <utility>
#include <vector>
#include "similarity_from_matches.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_det.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

double noise( double sigma );

// Assume throughout that the format of the parameter vector is a, b, tx, ty

similarity_from_matches::similarity_from_matches( std::vector<image_point_match>  matches )
  : rrel_estimation_problem(2,2),
    matches_(std::move(matches))
{
  assert( matches_.size() >= 2 );

  int old_point_id = matches_[0].point_id_;
  num_points_to_match_ = 1;
  for ( unsigned int i=1; i<matches_.size(); ++i )
  {
    if ( matches_[i].point_id_ != old_point_id )
    {
      num_points_to_match_ ++ ;
      old_point_id = matches_[i].point_id_;
    }
  }
  assert( num_points_to_match_ >= 2 );
}

bool
similarity_from_matches::fit_from_minimal_set( const std::vector<int>& match_indices,
                                               vnl_vector<double>& params ) const
{
  assert( match_indices.size() == 2 );
  if ( matches_[match_indices[0]].point_id_ == matches_[match_indices[1]].point_id_ ||
       (matches_[match_indices[0]].to_loc_ - matches_[match_indices[1]].to_loc_).squared_magnitude()
       < 1.0e-6 )
  {
    std::cerr << "identical from points or to points: returning false\n";
    return false;
  }

  vnl_matrix_fixed<double,4,4> X( 0.0 );
  vnl_vector_fixed<double,4> U;
  for ( int i=0; i<2; ++i )
  {
    // std::cout << "i = " << i << ", match_indices[i] = " << match_indices[i] << '\n'
    //          << "from_loc_ = " << matches_[match_indices[i]].from_loc_
    //          << ", to_loc_ = " << matches_[match_indices[i]].to_loc_ << '\n';
    double x = matches_[match_indices[i]].from_loc_[0];
    double y = matches_[match_indices[i]].from_loc_[1];
    double u = matches_[match_indices[i]].to_loc_[0];
    double v = matches_[match_indices[i]].to_loc_[1];
    X( 2*i, 0 ) = X( 2*i+1, 1 ) = x;
    X( 2*i, 1 ) = -y; X( 2*i+1, 0 ) = y;
    X( 2*i, 2) = X( 2*i+1, 3 ) = 1.0;
    U( 2*i ) = u;
    U( 2*i+1 ) = v;
  }

  // std::cout << "X matrix = " << X << '\n'
  //          << "U vector = " << U << '\n';

  if ( vnl_det(X) == 0.0 )
  {
    std::cout << "Rank is < 4, fit_from_minimal_set() returns false\n";
    return false;
  }

  params = (vnl_inverse(X) * U).as_ref(); // length 4
  return true;
}


unsigned int
similarity_from_matches::num_samples( ) const
{
  return matches_.size();
}

static
double calc_residual( const vnl_vector<double>& params, const image_point_match& match )
{
  assert(params.size() >= 4);
  double from_x = match.from_loc_[0];
  double from_y = match.from_loc_[1];
  vnl_vector_fixed<double,2> trans;
  trans[0] = params[0] * from_x - params[1] * from_y + params[2];
  trans[1] = params[1] * from_x + params[0] * from_y + params[3];
  return (trans - match.to_loc_).two_norm();
}


void
similarity_from_matches::compute_residuals( const vnl_vector<double>& params,
                                            std::vector<double>& residuals ) const
{
  if ( residuals.size() != matches_.size() )
    residuals.resize( matches_.size() );
  for ( unsigned int i =0; i<matches_.size(); ++i )
    residuals[i] = calc_residual( params, matches_[i] );
}


void
similarity_from_matches::compute_weights( const std::vector<double>& residuals,
                                          const rrel_wls_obj* obj,
                                          double scale,
                                          std::vector<double>& weights ) const
{
  // First compute the weights as normal.
  rrel_estimation_problem::compute_weights( residuals, obj, scale, weights );

  // Then augment them.
  unsigned int i=0;
  while ( i<matches_.size() )
  {
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
                                                     const std::vector<double>* weights ) const
{
  vnl_matrix_fixed<double,4,4> wXtX(0.0);
  vnl_matrix_fixed<double,2,4> Xi(0.0);  Xi(0,2) = Xi(1,3) = 1.0;
  vnl_vector_fixed<double,4> wXtu(0.0);

  for ( unsigned int i=0; i<matches_.size(); ++i )
  {
    // std::cout << "i = " << i << ". from_loc_ = " << matches_[i].from_loc_
    //          << ", to_loc_ = " << matches_[i].to_loc_ << std::endl;
    Xi( 0, 0 ) = Xi( 1, 1 ) = matches_[i].from_loc_[0];
    Xi( 1, 0 ) = matches_[i].from_loc_[1];
    Xi( 0, 1 ) = -Xi(1, 0 );

    // std::cout << "Xi = " << Xi << std::endl;
    double wt = (*weights)[i];
    wXtX += wt * Xi.transpose() * Xi;
    wXtu += wt * Xi.transpose() * matches_[i].to_loc_;
  }

  // std::cout << "wXtX matrix = " << wXtX << '\n'
  //          << "wXtu vector = " << wXtu << '\n';

  if ( vnl_det(wXtX) == 0.0 )
  {
    std::cout << "Rank is < 4, weighted_least_squares_fit() returns false\n";
    return false;
  }

  cofact = vnl_inverse(wXtX).as_ref(); // size 4x4
  params = cofact * wXtu;
  if ( vnl_math::abs(params[0]) < 1.0e-6 && vnl_math::abs(params[1]) < 1.0e-6 )
  {
    std::cout << "Transformation is degenerate\n";
    return false;
  }
  else
    return true;
}

void
generate_similarity_matches( const vnl_vector<double>& params,
                             double sigma,
                             std::vector<image_point_match>& matches )
{
  assert( params.size() == 4 );
  vnl_matrix_fixed<double,2,2> A;
  vnl_vector_fixed<double,2> t;
  A(0,0) = A(1,1) = params[0];
  A(0,1) = -params[1];  A(1,0) = params[1];
  t[0] = params[2]; t[1] = params[3];

  vnl_vector_fixed<double,2> from_loc;
  vnl_vector_fixed<double,2> to_loc;

  matches.resize(0);

  from_loc[0] = 10;  from_loc[1] = 20;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( 20 * sigma );
  to_loc[1] += noise( 20 * sigma );
  int id = 0;
  matches.emplace_back( from_loc, to_loc, id );  // 0 - id 0 - bad

  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 0;
  matches.emplace_back( from_loc, to_loc, id );  // 1 - id 0 - good

  from_loc[0] = 80;  from_loc[1] = -20;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 1;
  matches.emplace_back( from_loc, to_loc, id );  // 2 - id 1 - good

  from_loc[0] = -50;  from_loc[1] = 84;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 2;
  matches.emplace_back( from_loc, to_loc, id );  // 3 - id 2 - good

  to_loc = A*from_loc + t;
  to_loc[0] += noise( 20 * sigma );
  to_loc[1] += noise( 20 * sigma );
  id = 2;
  matches.emplace_back( from_loc, to_loc, id );  // 4 - id 2 - bad

  from_loc[0] = 75;  from_loc[1] = 62;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( 20 * sigma );
  to_loc[1] += noise( 20 * sigma );
  id = 3;
  matches.emplace_back( from_loc, to_loc, id );  // 5 - id 3 - bad

  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 3;
  matches.emplace_back( from_loc, to_loc, id );  // 6 - id 3 - good

  from_loc[0] = -16;  from_loc[1] = -83;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 4;
  matches.emplace_back( from_loc, to_loc, id );  // 7 - id 4 - good

  from_loc[0] = 32;  from_loc[1] = 7;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 5;
  matches.emplace_back( from_loc, to_loc, id );  // 8 - id 5 - good

  to_loc = A*from_loc + t;
  to_loc[0] += noise( 20 * sigma );
  to_loc[1] += noise( 20 * sigma );
  id = 5;
  matches.emplace_back( from_loc, to_loc, id );  // 9 - id 5 - bad

  from_loc[0] = 132;  from_loc[1] = -25;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 6;
  matches.emplace_back( from_loc, to_loc, id );  // 10 - id 6 - good

  from_loc[0] = -99;  from_loc[1] = 99;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( 20 * sigma );
  to_loc[1] += noise( 20 * sigma );
  id = 7;
  matches.emplace_back( from_loc, to_loc, id );  // 11 - id 7 - bad (all)

  to_loc = A*from_loc + t;
  to_loc[0] += noise( 20 * sigma );
  to_loc[1] += noise( 20 * sigma );
  id = 7;
  matches.emplace_back( from_loc, to_loc, id );  // 12 - id 7 - bad (all)

  from_loc[0] = 65;  from_loc[1] = 99;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 8;
  matches.emplace_back( from_loc, to_loc, id );  // 13 - id 8 - good

  from_loc[0] = 15;  from_loc[1] = -42;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 9;
  matches.emplace_back( from_loc, to_loc, id );  // 14 - id 9 - good

  to_loc = A*from_loc + t;
  to_loc[0] += noise( 20 * sigma );
  to_loc[1] += noise( 20 * sigma );
  id = 9;
  matches.emplace_back( from_loc, to_loc, id );  // 15 - id 9 - bad

  from_loc[0] = 51;  from_loc[1] = -24;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 10;
  matches.emplace_back( from_loc, to_loc, id );  // 16 - id 10 - good

  from_loc[0] = 44;  from_loc[1] = 66;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( 20 * sigma );
  to_loc[1] += noise( 20 * sigma );
  id = 11;
  matches.emplace_back( from_loc, to_loc, id );  // 17 - id 11 - bad

  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 11;
  matches.emplace_back( from_loc, to_loc, id );  // 18 - id 11 - good

  from_loc[0] = 71;  from_loc[1] = -39;
  to_loc = A*from_loc + t;
  to_loc[0] += noise( sigma );
  to_loc[1] += noise( sigma );
  id = 12;
  matches.emplace_back( from_loc, to_loc, id );  // 19 - id 12 - good
}
