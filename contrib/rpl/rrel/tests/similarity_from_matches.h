#ifndef similarity_from_matches_h_
#define similarity_from_matches_h_

//  Test support class that gives an example of a non-unique matching
//  problem.  This may eventually be moved to rrel/examples.

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>
#include <rrel/rrel_estimation_problem.h>
#include <rrel/rrel_wls_obj.h>

class image_point_match
{
 public:
  vnl_vector_fixed<double,2> from_loc_;
  vnl_vector_fixed<double,2> to_loc_;
  int point_id_;   // competing matches share the same point_id
 public:
  image_point_match() = default;
  image_point_match( const vnl_vector_fixed<double,2>& from,
                     const vnl_vector_fixed<double,2>& to,
                     int id )
    : from_loc_(from), to_loc_(to), point_id_(id) {}
  image_point_match( const image_point_match& old ) { *this = old; }

  image_point_match & operator= ( const image_point_match& old ) = default;
};

class similarity_from_matches : public rrel_estimation_problem
{
  std::vector<image_point_match> matches_;
  int num_points_to_match_;
 public:
  similarity_from_matches() : rrel_estimation_problem(2,2) {}
  similarity_from_matches( std::vector<image_point_match>  matches );
  ~similarity_from_matches() override = default;
  unsigned int num_unique_samples( ) const override{ return num_points_to_match_; }
  unsigned int num_samples( ) const override;
  bool fit_from_minimal_set( const std::vector<int>& match_indices,
                                     vnl_vector<double>& params ) const override;
  void compute_residuals( const vnl_vector<double>& params,
                                  std::vector<double>& residuals ) const override;
  void compute_weights( const std::vector<double>& residuals,
                                const rrel_wls_obj* obj,
                                double scale,
                                std::vector<double>& weights ) const override;
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                           vnl_matrix<double>& cofact,
                                           const std::vector<double>* weights=nullptr ) const override;
};

void
generate_similarity_matches( const vnl_vector<double>& params,
                             double sigma,
                             std::vector<image_point_match>& matches );

#endif // similarity_from_matches_h_
