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
  double sigma;
  internal_fcn( begin, end, sigma );
  return sigma;
}


double
rrel_muset_obj::fcn( vect_const_iter begin, vect_const_iter end,
                   double /*scale*/,
                   vnl_vector<double>* /*param_vector*/ ) const
{
  double sigma;
  internal_fcn( begin, end, sigma );
  return sigma;
}


double
rrel_muset_obj::scale( vect_const_iter begin, vect_const_iter end ) const
{
  double sigma;
  internal_fcn( begin, end, sigma );
  return sigma;
}


void
rrel_muset_obj::internal_fcn( vect_const_iter begin, vect_const_iter end,
                              double& sigma_est ) const
{
  // Calculate the absolute residuals and sort them.
  vcl_vector<double> abs_residuals;
  abs_residuals.reserve( end - begin );
  for( ; begin != end; ++begin ) {
    abs_residuals.push_back( vnl_math_abs( *begin ) );
  }
  vcl_sort( abs_residuals.begin(), abs_residuals.end() );

  unsigned int num_residuals = abs_residuals.size();
  bool at_start=true;
  unsigned int best_k = 0;
  double best_sk = 0;
  unsigned int prev_k = 0;
  double sum_residuals=0;
  double best_sum = 0;
  double best_objective = 0;

  //  Find the best k
  for ( double frac=min_frac_; frac<=max_frac_+0.00001; frac+=frac_inc_ ) {
    unsigned int k = vnl_math_rnd( frac*num_residuals );
    if( k>num_residuals ) k=num_residuals;
    if( k<=0 ) k=1;
    if( k == prev_k )  continue;

    for ( unsigned int i=prev_k; i<k; ++i ) {
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
    prev_k = k;
  }

  //  Refine the scale estimate
  if ( ! use_sk_refine_ ) {
    sigma_est = best_sk;
  }
  else {
    unsigned int new_n = 0;
    while( new_n<num_residuals && abs_residuals[new_n] < 3*best_sk )
      ++new_n;
    if( best_k > new_n ) best_k = new_n;
    sigma_est = best_sum / table_->muset_divisor(best_k, new_n);
  }
}
