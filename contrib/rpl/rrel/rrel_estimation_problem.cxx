#include "rrel_estimation_problem.h"

#include <rrel/rrel_wls_obj.h>

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

rrel_estimation_problem::rrel_estimation_problem( unsigned int dof,
                                                  unsigned int num_samples_for_fit )
  : dof_( dof ),
    num_samples_for_fit_( num_samples_for_fit ),
    scale_type_( NONE ),
    single_scale_( 0 ),
    multiple_scales_( 0 )
{
}

rrel_estimation_problem::rrel_estimation_problem( )
  : scale_type_( NONE ),
    single_scale_( 0 ),
    multiple_scales_( 0 )
{
}

rrel_estimation_problem::~rrel_estimation_problem( )
{
  delete multiple_scales_;
}


void
rrel_estimation_problem::compute_weights( const vcl_vector<double>& residuals,
                                          const rrel_wls_obj* obj,
                                          double scale,
                                          vcl_vector<double>& weights ) const
{
  if ( residuals.size() != weights.size())
    weights.resize( residuals.size() );

  switch( scale_type_ ) {
  case NONE:
    obj->wgt( residuals.begin(), residuals.end(), scale, weights.begin() );
    break;
  case SINGLE:
    obj->wgt( residuals.begin(), residuals.end(), single_scale_, weights.begin() );
  case MULTIPLE:
    obj->wgt( residuals.begin(), residuals.end(), multiple_scales_->begin(), weights.begin() );
    break;
  }
}


const vcl_vector<double>&
rrel_estimation_problem::prior_multiple_scales() const
{
  assert( scale_type_ == MULTIPLE );
  return *multiple_scales_;
}

double
rrel_estimation_problem::prior_scale() const
{
  assert( scale_type_ == SINGLE );
  return single_scale_;
}

void
rrel_estimation_problem::set_prior_multiple_scales( const vcl_vector<double>& scales )
{
  if( ! multiple_scales_ ) {
    multiple_scales_ = new vcl_vector<double>;
  }
  *multiple_scales_ = scales;
  scale_type_ = MULTIPLE;
}

void
rrel_estimation_problem::set_prior_scale( double scale )
{
  single_scale_ = scale;
  scale_type_ = SINGLE;
}

void
rrel_estimation_problem::set_no_prior_scale( )
{
  scale_type_ = NONE;
}
