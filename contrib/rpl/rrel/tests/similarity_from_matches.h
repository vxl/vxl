#ifndef similarity_from_matches_h_
#define similarity_from_matches_h_

//  Test support class that gives an example of a non-unique matching
//  problem.  This may eventually be moved to rrel/examples.

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <rrel/rrel_estimation_problem.h>
#include <rrel/rrel_wls_obj.h>

class image_point_match {
public:
  image_point_match() {}
  image_point_match( const vnl_vector_fixed<double,2>& from,
                     const vnl_vector_fixed<double,2>& to,
                     int id ) 
    : from_loc_(from), to_loc_(to), point_id_(id) {}
  image_point_match( const image_point_match& old ) { *this = old; }

  const image_point_match& operator= ( const image_point_match& old )
  { from_loc_ = old.from_loc_;
    to_loc_ = old.to_loc_; 
    point_id_ = old.point_id_; 
    return *this; }

public:
  vnl_vector_fixed<double,2> from_loc_;
  vnl_vector_fixed<double,2> to_loc_;
  int point_id_;   // competing matches share the same point_id
};

class similarity_from_matches : public rrel_estimation_problem {
public:
  similarity_from_matches() : rrel_estimation_problem(2,2) {}
  similarity_from_matches( const vcl_vector<image_point_match>& matches );
  ~similarity_from_matches() {}
  virtual unsigned int num_unique_samples( ) const{ return num_points_to_match_; }
  virtual unsigned int num_samples( ) const; 
  virtual bool fit_from_minimal_set( const vcl_vector<int>& match_indices,
                                     vnl_vector<double>& params ) const;
  virtual void compute_residuals( const vnl_vector<double>& params,
                                  vcl_vector<double>& residuals ) const;
  virtual void compute_weights( const vcl_vector<double>& residuals,
                                const rrel_wls_obj* obj,
                                double scale,
                                vcl_vector<double>& weights ) const;
  virtual bool weighted_least_squares_fit( vnl_vector<double>& params,
                                           vnl_matrix<double>& cofact,
                                           const vcl_vector<double>* weights=0 ) const;
private:
  vcl_vector<image_point_match> matches_;
  int num_points_to_match_;
};

void
generate_similarity_matches( const vnl_vector<double>& params,
                             double sigma,
                             vcl_vector<image_point_match>& matches );
#endif
