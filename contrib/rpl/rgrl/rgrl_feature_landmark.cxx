// \file
// \author Amitha Perera
// \date Feb 2002

#include "rgrl_feature_landmark.h"
#include "rgrl_transformation.h"
#include "rgrl_cast.h"

#include <vnl/vnl_math.h>

typedef vcl_vector< vnl_vector<double> > vec_vec_type;

rgrl_feature_landmark::
rgrl_feature_landmark( vnl_vector<double> const& loc,
                       vcl_vector< vnl_vector<double> > const& outgoing_directions)
  : location_( loc ),
    error_proj_( loc.size(), loc.size(), vnl_matrix_identity ),
    outgoing_directions_( outgoing_directions )
{
  assert(outgoing_directions_.size() > 0);
}


rgrl_feature_landmark::
rgrl_feature_landmark( rgrl_feature_landmark const& other )
  : location_( other.location_ ),
    error_proj_( other.error_proj_ ),
    outgoing_directions_( other.outgoing_directions_ )
{
}

vnl_vector<double> const&
rgrl_feature_landmark::
location() const
{
  return location_;
}

vnl_matrix<double> const&
rgrl_feature_landmark::
error_projector() const
{
  return error_proj_;
}

unsigned int
rgrl_feature_landmark::
num_constraints() const
{
  return location_.size();
}

rgrl_feature_sptr 
rgrl_feature_landmark::
transform( rgrl_transformation const& xform ) const
{
  rgrl_feature_landmark* result = new rgrl_feature_landmark( *this );

  // Transform the location
  //
  xform.map_location( this->location_, result->location_ );

  // Transform each of the direction vectors
  //
  vec_vec_type::const_iterator fitr = this->outgoing_directions_.begin();
  vec_vec_type::const_iterator fend = this->outgoing_directions_.end();
  vec_vec_type::iterator titr = result->outgoing_directions_.begin();
  for( ; fitr != fend; ++fitr, ++titr ) {
    xform.map_direction( this->location_, *fitr, *titr );
  }

  return result;
}

double 
rgrl_feature_landmark::
absolute_signature_weight( rgrl_feature_sptr other ) const
{
  if ( !other->is_type( rgrl_feature_landmark::type_id() ) )
    return 1;

  rgrl_feature_landmark* other_ptr = rgrl_cast<rgrl_feature_landmark *>(other);
  const vcl_vector<vnl_vector<double> >& sig_P = this->outgoing_directions_;
  const vcl_vector<vnl_vector<double> >& sig_Q = other_ptr->outgoing_directions_;
  
  int ones = vnl_math_min(sig_P.size(), sig_Q.size());
  double max;
  if (sig_P.size() < sig_Q.size()) {
    vbl_array_2d<bool> invalid(sig_P.size(), sig_Q.size(), false);
    max = max_similarity(sig_P, sig_Q, ones, invalid);
  }
  else {
    vbl_array_2d<bool> invalid(sig_Q.size(), sig_P.size(), false);
    max = max_similarity(sig_Q, sig_P, ones, invalid);
  }
  
  return vcl_pow( 0.5*max/ones, 100 );
}


// A resursive function to calculate the max similarity of two landmarks
//
double
rgrl_feature_landmark::
max_similarity(const vcl_vector<vnl_vector<double> >& u,
               const vcl_vector<vnl_vector<double> >& v,
               int count, 
               const vbl_array_2d<bool>& invalid) const
{
  if (count == 0) return 0.0;

  double max = 0.0;
  for (unsigned int j = 0; j < v.size(); j++) {
    if (!invalid(count-1,j)) {
      vbl_array_2d<bool> invalid2(invalid);
      for ( unsigned k = 0; k < u.size(); k++)
        invalid2.put(k,j,true);
      for ( unsigned k = 0; k < v.size(); k++)
        invalid2.put(count-1,k,true);
      double sum  = dot_product(u[count-1],v[j]) + 1 +
        max_similarity(u, v, count-1, invalid2);
      if (max < sum) max = sum;
    }
  }
  return max ;
}
