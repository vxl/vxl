#include <rrel/rrel_muset_obj.h>

#include <rrel/rrel_muse_table.h>

#include <vnl/vnl_math.h>

#include <vcl_vector.h>
#include <vcl_algorithm.h>

rrel_muset_obj::rrel_muset_obj( int max_n, 
                                bool use_sk_refine )
  : use_sk_refine_( use_sk_refine ),
    table_owned_(true)
{
  table_ = new rrel_muse_table(max_n);

  // Set the parameters to their default values.
  set_min_inlier_fraction();
  set_max_inlier_fraction();
  set_inlier_fraction_increment();
}


rrel_muset_obj::rrel_muset_obj( rrel_muse_table* table,
                                bool use_sk_refine)
  : use_sk_refine_(use_sk_refine),
    table_owned_(false),
    table_(table)
{
  // Set the parameters to their default values.
  set_min_inlier_fraction();
  set_max_inlier_fraction();
  set_inlier_fraction_increment();
}

rrel_muset_obj::~rrel_muset_obj()
{
  if (table_owned_) {
    delete table_;
  }
}

double
rrel_muset_obj::fcn( vect_const_iter begin, vect_const_iter end,
                     vect_const_iter /*scale begin*/,
                     vnl_vector<double>* /*param_vector*/ ) const
{
  double sigma, objective;
  internal_fcn( begin, end, objective, sigma );
  return objective;
}


double
rrel_muset_obj::fcn( vect_const_iter begin, vect_const_iter end,
                   double /*scale*/,
                   vnl_vector<double>* /*param_vector*/ ) const
{
  double sigma, objective;
  internal_fcn( begin, end, objective, sigma );
  return objective;
}


double
rrel_muset_obj::scale( vect_const_iter begin, vect_const_iter end ) const
{
  double sigma, objective;
  internal_fcn( begin, end, objective, sigma );
  return sigma;
}


void
rrel_muset_obj::internal_fcn( vect_const_iter begin, vect_const_iter end,
                              double& best_objective, double& sigma_est ) const
{
  // Calculate the absolute residuals and sort them.
  vcl_vector<double> abs_residuals;
  abs_residuals.reserve( end - begin );
  for( ; begin != end; ++begin ) {
    abs_residuals.push_back( vnl_math_abs( *begin ) );
  }
  vcl_sort( abs_residuals.begin(), abs_residuals.end() );

  bool at_start=true;
  int best_k;
  double best_sk;
  int prev_k = -1;
  double sum_residuals=0;
  double best_sum;
  int num_residuals = abs_residuals.size();

  //  Find the best k
  for ( double frac=min_frac_; frac<=max_frac_+0.00001; frac+=frac_inc_ ) {
    int k = vnl_math_rnd( frac*num_residuals ) - 1;
    if ( k>=num_residuals ) k=num_residuals-1;
    if ( k<=0 ) k=1;
    for ( int i=prev_k+1; i<=k; i++ ) {
      sum_residuals += abs_residuals[i];
    }

    double sk = sum_residuals / table_->muset_divisor(k, num_residuals);
    double objective = sk * table_->standard_dev_kth(k, num_residuals) /
      table_->expected_kth(k, num_residuals);

    if ( at_start || objective < best_objective ) {
      at_start = false;
      best_k = k;
      best_sk = sk;
      best_sum = sum_residuals;
      best_objective = objective;
    }
  }

  //  Refine the scale estimate
  if ( ! use_sk_refine_ ) {
    sigma_est = best_sk;
  }
  else {
    int new_n = best_k;
    for ( int i=best_k; i<num_residuals && abs_residuals[i] < 3*best_sk; ++i ) {
      new_n = i;
    }
    sigma_est = best_sum / table_->muset_divisor(best_k, new_n);
    best_objective = sigma_est * table_->standard_dev_kth(best_k, new_n) /
      table_->expected_kth(best_k, new_n);
  }
}
