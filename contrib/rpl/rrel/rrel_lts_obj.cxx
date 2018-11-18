// This is rpl/rrel/rrel_lts_obj.cxx
#include <iostream>
#include <algorithm>
#include <vector>
#include "rrel_lts_obj.h"

#include <vnl/vnl_math.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <cassert>

rrel_lts_obj::rrel_lts_obj( unsigned int num_sam_inst, double inlier_frac )
  : num_sam_inst_( num_sam_inst ),
    inlier_frac_( inlier_frac )
{
}

rrel_lts_obj::~rrel_lts_obj() = default;


double
rrel_lts_obj::fcn( vect_const_iter begin, vect_const_iter end,
                   vect_const_iter /*scale begin*/,
                   vnl_vector<double>* /*param_vector*/ ) const
{
  return fcn( begin, end, 0.0, (vnl_vector<double>*)nullptr );
}


double
rrel_lts_obj::fcn( vect_const_iter begin, vect_const_iter end,
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

  std::vector<double> sq_res;
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
    index = vnl_math::rnd( (num_residuals-num_sam_inst_)*inlier_frac_ ) + num_sam_inst_;
  if ( index >= num_residuals ) index = num_residuals-1;

  // 3. Sort the squared residuals so that all the smallest residuals
  // are in positions less than index.
  auto loc = sq_res.begin() + index;
  std::nth_element( sq_res.begin(), loc, sq_res.end() );

  // 4. Sum them up.
  double sum=0;
  for ( unsigned int i=0; i<=index; ++i )
    sum += sq_res[i];

  return sum;
}
