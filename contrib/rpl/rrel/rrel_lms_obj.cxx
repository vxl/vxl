// This is rpl/rrel/rrel_lms_obj.cxx
#include "rrel_lms_obj.h"
#include "rrel_util.h"

#include <vnl/vnl_math.h>

#include <vcl_algorithm.h>
#include <vcl_vector.h>

#include <vcl_cassert.h>

rrel_lms_obj::rrel_lms_obj( unsigned int num_sam_inst, double inlier_frac )
  : num_sam_inst_( num_sam_inst ),
    inlier_frac_( inlier_frac )
{
}

rrel_lms_obj::~rrel_lms_obj()
{
}


double
rrel_lms_obj::fcn( vect_const_iter begin, vect_const_iter end,
                   vect_const_iter /*scale begin*/,
                   vnl_vector<double>* /*param_vector*/ ) const
{
  return fcn( begin, end, 0.0, (vnl_vector<double>*)0 );
}


double
rrel_lms_obj::fcn( vect_const_iter begin, vect_const_iter end,
                   double /*scale*/,
                   vnl_vector<double>* /*param_vector*/ ) const
{
  // 1. We need to sort the squared residuals.

  // Since we know that vect_const_iter is vector::iterator, we can
  // use the size of the incoming vector to preallocate the sq_res
  // vector. If/when this becomes more general, the we can remove the
  // preallocation if necessary. The cost of a few reallocs as we
  // compute the squared residuals are unlikely to seriously affect
  // anything.

  vcl_vector<double> sq_res;
  sq_res.reserve( end - begin );
  for ( ; begin != end; ++begin ) {
    sq_res.push_back( (*begin) * (*begin) );
  }

  unsigned int num_residuals = sq_res.size();
  assert( num_residuals >= num_sam_inst_ );

  // 2. Find the index of the "median value" if the residuals are sorted.

  unsigned int index;
  if ( inlier_frac_ == 0.5 ) 
    index = (num_residuals-num_sam_inst_)/2 + num_sam_inst_;
  else
    index = vnl_math_rnd( (num_residuals-num_sam_inst_)*inlier_frac_ ) + num_sam_inst_;
  if ( index >= num_residuals ) index = num_residuals-1;

  // 3. Sort the squared residuals and extract the "median".
  vcl_vector<double>::iterator loc = sq_res.begin() + index;
  vcl_nth_element( sq_res.begin(), loc, sq_res.end() );

  return *loc;
}

double
rrel_lms_obj::scale( vect_const_iter begin, vect_const_iter end ) const
{
  // Work on a copy of the vector, since the MAD scale estimator
  // will change the content of the vector
  vcl_vector<double> vec_copy;
  vec_copy.reserve( end - begin );
  for ( ; begin != end; ++begin ) {
    vec_copy.push_back( *begin);
  }

  return rrel_util_median_abs_dev_scale( vec_copy.begin(), vec_copy.end(), num_sam_inst_);
}
