#include <rrel/rrel_estimation_problem.h>

#include <rrel/rrel_wls_obj.h>

#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

rrel_estimation_problem::rrel_estimation_problem( unsigned int dof,
                                                  unsigned int num_samples_for_fit )
  : dof_( dof ),
    num_samples_for_fit_( num_samples_for_fit )
{
}

rrel_estimation_problem::rrel_estimation_problem( )
{
}

rrel_estimation_problem::~rrel_estimation_problem( )
{
}

bool 
rrel_estimation_problem::fit_from_minimal_set( const vcl_vector<int>& /* point_indices */, 
                                               vnl_vector<double>& /* params */ ) const
{ 
  vcl_cerr << "FAILURE: Problem doesn't implement fit_from_minimal_sample. Try a different search technique." << vcl_endl;
  vcl_abort();
}


void
rrel_estimation_problem::compute_weights( const vcl_vector<double>& residuals,
                                          const rrel_wls_obj* obj,
                                          double scale,
                                          vcl_vector<double>& weights ) const
{
  if ( residuals.size() != weights.size())
    weights.resize( residuals.size() );

  if( has_prior_scale() ) {
    obj->wgt( residuals.begin(), residuals.end(), prior_scales().begin(), weights.begin() );
  } else {
    obj->wgt( residuals.begin(), residuals.end(), scale, weights.begin() );
  }
}


const vcl_vector<double>&
rrel_estimation_problem::prior_scales() const
{
  vcl_cerr << "FAILURE: Problem doesn't implement prior_scales()." << vcl_endl;
  vcl_abort();
}


bool
rrel_estimation_problem::weighted_least_squares_fit( vnl_vector<double>& /*params*/,
                                                     vnl_matrix<double>& /*norm_covar*/,
                                                     vcl_vector<double>* /*weights*/ ) const
{
  vcl_cerr << "FAILURE: Problem doesn't implement weighted_least_squares_fit. Try a different search technique." << vcl_endl;
  vcl_abort();
}

