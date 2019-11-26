// This is rpl/rrel/rrel_muset_obj.cxx
#include <limits>
#include <iostream>
#include <vector>
#include <algorithm>
#include "rrel_muset_obj.h"
//
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <cassert>
#include "vnl/vnl_math.h"
#include <rrel/rrel_muse_table.h>

rrel_muset_obj::rrel_muset_obj( int max_n,
                                bool use_sk_refine )
  : use_sk_refine_( use_sk_refine ), muse_type_( RREL_MUSE_TRIMMED ),
    table_owned_(true)
{
  table_ = new rrel_muse_table(max_n);

  // Set the parameters to their default values.
  set_min_inlier_fraction();
  set_max_inlier_fraction();
  set_inlier_fraction_increment();
}


rrel_muset_obj::rrel_muset_obj( rrel_muse_table* table,
                                bool use_sk_refine )
  : use_sk_refine_(use_sk_refine), muse_type_( RREL_MUSE_TRIMMED ),
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
  if (table_owned_)
    delete table_;
}

double
rrel_muset_obj::fcn( vect_const_iter begin, vect_const_iter end,
                     vect_const_iter /*scale begin*/,
                     vnl_vector<double>* /*param_vector*/ ) const
{
  double sigma;
  int best_k;
  internal_fcn( begin, end, sigma, best_k );
  return sigma;
}


double
rrel_muset_obj::fcn( vect_const_iter begin, vect_const_iter end,
                     double /*scale*/,
                     vnl_vector<double>* /*param_vector*/ ) const
{
  double sigma;
  int best_k;
  internal_fcn( begin, end, sigma, best_k );
  return sigma;
}


double
rrel_muset_obj::scale( vect_const_iter begin, vect_const_iter end ) const
{
  double sigma;
  int best_k;
  internal_fcn( begin, end, sigma, best_k );
  return sigma;
}


void
rrel_muset_obj::internal_fcn( vect_const_iter begin, vect_const_iter end,
                              double& sigma_est, int& best_k ) const
{
  // Calculate the absolute residuals and sort them.
  std::vector<double> abs_residuals;
  abs_residuals.reserve( end - begin );
  for ( ; begin != end; ++begin ) {
    abs_residuals.push_back( vnl_math::abs( *begin ) );
  }
  std::sort( abs_residuals.begin(), abs_residuals.end() );

  unsigned int num_residuals = abs_residuals.size();
  bool at_start=true;
  double best_sk = 0;
  double best_objective = 0;

  best_k = 0;
  constexpr double min_exp_kth_to_stddev_ratio = 3.0;
  static bool notwarned = true;

  switch ( muse_type_ )
  {
   case RREL_MUSE_TRIMMED:
   {
#ifdef DEBUG
    std::cout << "\nRREL_MUSE_TRIMMED\n";
#endif
    double sum_residuals=0;
    double best_sum = 0;
    int prev_k = 0;

    //  Find the best k
    for ( double frac=min_frac_; frac<=max_frac_+0.00001; frac+=frac_inc_ )
    {
      unsigned int k = vnl_math::rnd( frac*num_residuals );
      if ( k>num_residuals ) k=num_residuals;
      if ( k<=0 ) k=1;
      if ( table_->expected_kth(k, num_residuals) /
           table_->standard_dev_kth(k, num_residuals) < min_exp_kth_to_stddev_ratio )
      {
        if ( notwarned ) {
          std::cerr << "WARNING:  rrel_muset_obj::internal_fcn "
                   << "attempted evaluation at value of k that lead to unstable estimates\n";
          notwarned = false;
        }
        continue;
      }

      for ( unsigned int i=prev_k; i<k; ++i ) {
        sum_residuals += abs_residuals[i];
      }
      double sk = sum_residuals / table_->muset_divisor(k, num_residuals);
      double objective = sk * table_->standard_dev_kth(k, num_residuals) /
                         table_->expected_kth(k, num_residuals);

#ifdef DEBUG
      std::cout << "k = " << k << ", sk = " << sk
               << ", objective = " << objective << '\n';
#endif

      if ( at_start || objective < best_objective ) {
        at_start = false;
        best_k = k;
        best_sk = sk;
        best_sum = sum_residuals;
        best_objective = objective;
      }
      prev_k = k;
    }

    if ( at_start ) {
      std::cerr << "WARNING:  There were NO values of k with stable estimates.\n"
               << "          Setting sigma = +Infinity\n";
      sigma_est = std::numeric_limits<double>::infinity();
      return;
    }

    //  Refine the scale estimate
    if ( ! use_sk_refine_ ) {
      sigma_est = best_sk;
#ifdef DEBUG
      std::cout << "No sk refinement\n";
#endif
    }
    else {
#ifdef DEBUG
      std::cout << "sk refinement\n";
#endif
      unsigned int new_n = best_k;
      while ( new_n<num_residuals && abs_residuals[new_n] < 2.5*best_sk )
        ++new_n;
#ifdef DEBUG
      std::cout << "New n = " << new_n << '\n';
#endif
      sigma_est = best_sum / table_ -> muset_divisor(best_k, new_n);
    }
    break;
   }

   case RREL_MUSE_TRIMMED_SQUARE:
   {
#ifdef DEBUG
    std::cout << "\nRREL_MUSE_TRIMMED_SQUARE\n";
#endif
    double sum_sq_residuals=0;
    double best_sum_sq = 0;
    int prev_k = 0;

    //  Find the best k
    for ( double frac=min_frac_; frac<=max_frac_+0.00001; frac+=frac_inc_ ) {
      unsigned int k = vnl_math::rnd( frac*num_residuals );
      if ( k>num_residuals ) k=num_residuals;
      if ( k<=0 ) k=1;
      if ( table_->expected_kth(k, num_residuals) /
           table_->standard_dev_kth(k, num_residuals) < min_exp_kth_to_stddev_ratio )
        {
          if ( notwarned ) {
            std::cerr << "WARNING:  rrel_muset_obj::internal_fcn attempted evaluation at "
                     << "value of k that lead to unstable estimates\n";
            notwarned = false;
          }
          continue;
        }

      for ( unsigned int i=prev_k; i<k; ++i ) {
        sum_sq_residuals += vnl_math::sqr( abs_residuals[i] );
      }
      double sk = std::sqrt( sum_sq_residuals
                            / table_->muset_sq_divisor(k, num_residuals) );
      double objective = sk * table_->standard_dev_kth(k, num_residuals) /
                         table_->expected_kth(k, num_residuals);

#ifdef DEBUG
      std::cout << "k = " << k << ", sk = " << sk
               << ", objective = " << objective << '\n';
#endif

      if ( at_start || objective < best_objective ) {
        at_start = false;
        best_k = k;
        best_sk = sk;
        best_sum_sq = sum_sq_residuals;
        best_objective = objective;
      }
      prev_k = k;
    }

    if ( at_start ) {
      std::cerr << "WARNING:  There were NO values of k with stable estimates.\n"
               << "          Setting sigma = +Infinity\n";
      sigma_est = std::numeric_limits<double>::infinity();
      return;
    }

    //  Refine the scale estimate
    if ( ! use_sk_refine_ ) {
      sigma_est = best_sk;
#ifdef DEBUG
      std::cout << "No sk refinement\n";
#endif
    }
    else {
#ifdef DEBUG
      std::cout << "sk refinement\n";
#endif
      unsigned int new_n = best_k;
      while ( new_n<num_residuals && abs_residuals[new_n] < 2.5*best_sk )
        ++new_n;
#ifdef DEBUG
      std::cout << "New n = " << new_n << '\n';
#endif
      sigma_est = std::sqrt( best_sum_sq
                            / table_->muset_sq_divisor(best_k, new_n) );
    }
    break;
   }

   case RREL_MUSE_QUANTILE:
   {
#ifdef DEBUG
    std::cout << "\nRREL_MUSE_QUANTILE\n";
#endif
    for ( double frac=min_frac_; frac<=max_frac_+0.00001; frac+=frac_inc_ ) {
      int kk = vnl_math::rnd( frac*num_residuals );
      unsigned int k = ( kk <= 0 )  ? 1 : kk;
      if ( k>num_residuals ) k=num_residuals;
      if ( table_->expected_kth(k, num_residuals) /
           table_->standard_dev_kth(k, num_residuals) < min_exp_kth_to_stddev_ratio )
        {
          if ( notwarned ) {
            std::cerr << "WARNING:  rrel_muset_obj::internal_fcn attempted evaluation at "
                     << "value of k that lead to unstable estimates\n";
            notwarned = false;
          }
          continue;
        }

      double sk = abs_residuals[ k-1 ] / table_->expected_kth(k, num_residuals);
      double objective = sk * table_->standard_dev_kth(k, num_residuals) /
                         table_->expected_kth(k, num_residuals);

#ifdef DEBUG
      std::cout << "k = " << k << ", sk = " << sk
               << ", objective = " << objective << '\n';
#endif

      if ( at_start || objective < best_objective ) {
        best_objective = objective;
        best_k = k;
        best_sk = sk;
        at_start = false;
      }
    }

    if ( at_start ) {
      std::cerr << "WARNING:  There were NO values of k with stable estimates.\n"
               << "          Setting sigma = +Infinity\n";
      sigma_est = std::numeric_limits<double>::infinity();
      return;
    }

    //  Refine the scale estimate
    if ( ! use_sk_refine_ ) {
      sigma_est = best_sk;
#ifdef DEBUG
      std::cout << "No sk refinement\n";
#endif
    }
    else {
#ifdef DEBUG
      std::cout << "sk refinement\n";
#endif
      unsigned int new_n = best_k;
      while ( new_n<num_residuals && abs_residuals[new_n] < 2.5*best_sk )
        ++new_n;
#ifdef DEBUG
      std::cout << "New n = " << new_n << '\n';
#endif
      sigma_est = abs_residuals[ best_k ] / table_->expected_kth(best_k, new_n);
    }
    break;
   }
   default:
    assert(!"invalid muse_type");
  }
}
