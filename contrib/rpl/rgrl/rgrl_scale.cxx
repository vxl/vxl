//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_scale.h"


rgrl_scale::
rgrl_scale()
  : has_geometric_scale_( false ),
    has_signature_inv_covar_( false )
{
}


bool
rgrl_scale::
has_geometric_scale() const
{
  return has_geometric_scale_;
}


double
rgrl_scale::
geometric_scale( ) const
{
  return geometric_scale_;
}


bool
rgrl_scale::
has_signature_inv_covar() const
{
  return has_signature_inv_covar_;
}


vnl_matrix<double> const&
rgrl_scale::
signature_inv_covar() const
{
  return signature_inv_covar_;
}


void
rgrl_scale::
set_scales( double                     geometric_scale,
            vnl_matrix<double>  const& signature_inv_covar,
            type new_type )
{
  set_geometric_scale( geometric_scale );
  set_signature_inv_covar( signature_inv_covar );
  set_geo_scale_type( new_type );
}


void
rgrl_scale::
set_geometric_scale( double scale, type new_type )
{
  geometric_scale_ = scale;
  has_geometric_scale_ = true;
  set_geo_scale_type( new_type );
}


void
rgrl_scale::
set_signature_inv_covar( vnl_matrix<double> const& covar )
{
  signature_inv_covar_ = covar;
  has_signature_inv_covar_ = true;
}

void
rgrl_scale::
write( vcl_ostream& os ) const
{
  // write out geometric scale
  if( has_geometric_scale_ ) {

    os << "GEOMETRIC_SCALE" << vcl_endl;
    os << ((geo_scale_type_==prior)?"PRIOR":"ESTIMATE") << vcl_endl;
    os << geometric_scale_ << vcl_endl;
  }
  
  // write out signature covariance
  if( has_signature_inv_covar_ ) {
    
    os << "SIGNATURE_INV_COVARIANCE" << vcl_endl;
    os << signature_inv_covar_.row() << " " 
      << signature_inv_covar_.cols() << vcl_endl;
    os << signature_inv_covar_ << vcl_endl;
  }
}

bool
rgrl_scale::
read( vcl_istream& is )
{
  vcl_string tag;
  
  // continue when stream does not reach the end 
  // and when the scales are not all read
  while( is && !is.eof() && (!has_geometric_scale_ || !has_signature_inv_covar_))  {
    
    vcl_getline( is, tag );
    
    // geometric scale
    if( tag.find("GEOMETRIC_SCALE") != vcl_string::npos ) {
      
      // get scale type
      vcl_string type_str;
      vcl_getline( is, type_str );
      if( type_str.find("PRIOR") != vcl_string::npos ) 
        geo_scale_type_ = prior;
      else if( type_str.find("ESTIMATE") != vcl_string::npos ) 
        geo_scale_type_ = estimate;
      else {  // cannot handle
        WarningMacro( "Cannot parse this line for geometric scale type: " << type_str << vcl_endl );
        return false;
      }
      
      // get scale
      is >> geometric_scale_;
      if( !is ) {
        WarningMacro( "Cannot parse geometric scale." << vcl_endl );
        return false;
      }
      
      // set flag
      has_geometric_scale_ = true;
    
    } else if ( tag.find("SIGNATURE_INV_COVARIANCE") != vcl_string::npos ) {
      
      // signature covariance
      int nrow = -1;
      int ncol = -1;
      
      // get number of rows and cols
      is >> nrow >> ncol;
      if( !is || nrow<=0 || ncol<=0 ) {
        WarningMacro( "Cannot parse the number of rows and columns." << vcl_endl );
        return false;
      }
      
      signature_inv_covar_.set_size( nrow, ncol );
      is >> signature_inv_covar_;

      if( !is ) {
        WarningMacro( "Cannot parse signature covariance" << vcl_endl );
        return false;
      }
      
      // set the flag
      has_signature_inv_covar_ = true;
      
    } else {// cannot handle
      WarningMacro( "Cannot parse this line for tags: " << tag << vcl_endl );
      return false;
    }
  }
  
  // succeeded
  return true;
}

