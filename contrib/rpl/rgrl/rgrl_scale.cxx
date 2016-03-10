//:
// \file
// \author Amitha Perera
// \date   Feb 2003
// \verbatim
// Modifications
//      Nov 2008 J Becker: Modified read so that one can easily read in a vector of scales.
// \endverbatim

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
write( std::ostream& os ) const
{
  // write out geometric scale
  if( has_geometric_scale_ ) {

    os << "GEOMETRIC_SCALE" << std::endl;
    os << ((geo_scale_type_==prior)?"PRIOR":"ESTIMATE") << std::endl;
    os << geometric_scale_ << std::endl;
  }

  // write out signature covariance
  if( has_signature_inv_covar_ ) {

    os << "SIGNATURE_INV_COVARIANCE" << std::endl;
    os << signature_inv_covar_.rows() << " "
      << signature_inv_covar_.cols() << std::endl;
    os << signature_inv_covar_ << std::endl;
  }
}

bool
rgrl_scale::
read( std::istream& is )
{
  std::streampos pos;
  static const std::string white_chars = " \t\r";
  std::string tag;
  has_geometric_scale_ = false;
  has_signature_inv_covar_ = false;

  // continue when stream does not reach the end
  // and when the scales are not all read
  while( is && !is.eof() && (!has_geometric_scale_ || !has_signature_inv_covar_))  {
    pos = is.tellg();
    std::getline( is, tag );

    if( tag.empty() || tag.find_first_not_of( white_chars ) == std::string::npos )
      continue;

    // geometric scale
    if( tag.find("GEOMETRIC_SCALE") != std::string::npos ) {
      if ( !has_geometric_scale_ && !has_signature_inv_covar_ ) {
        // get scale type
        std::string type_str;
        std::getline( is, type_str );
        if( type_str.find("PRIOR") != std::string::npos )
          geo_scale_type_ = prior;
        else if( type_str.find("ESTIMATE") != std::string::npos )
          geo_scale_type_ = estimate;
        else {  // cannot handle
          WarningMacro( "Cannot parse this line for geometric scale type: " << type_str << std::endl );
          return false;
        }

        // get scale
        is >> geometric_scale_;
        if( !is ) {
          WarningMacro( "Cannot parse geometric scale." << std::endl );
          return false;
        }

        // set flag
        has_geometric_scale_ = true;
      } else {
        is.seekg( pos );
        return true;
      }
    } else if ( tag.find("SIGNATURE_INV_COVARIANCE") != std::string::npos ) {
      if( !has_signature_inv_covar_ ) {
        // signature covariance
        int nrow = -1;
        int ncol = -1;

        // get number of rows and cols
        is >> nrow >> ncol;
        if( !is || nrow<=0 || ncol<=0 ) {
          WarningMacro( "Cannot parse the number of rows and columns." << std::endl );
          return false;
        }

        signature_inv_covar_.set_size( nrow, ncol );
        is >> signature_inv_covar_;

        if( !is ) {
          WarningMacro( "Cannot parse signature covariance" << std::endl );
          return false;
        }

        // set the flag
        has_signature_inv_covar_ = true;
      } else {
        is.seekg( pos );
        return true;
      }
    } else {// cannot handle
      WarningMacro( "Cannot parse this line for tags: " << tag << std::endl );
      return false;
    }
  }

  // succeeded
  return true;
}

//: output operator
std::ostream&
operator<<( std::ostream& ofs, rgrl_scale const& scale )
{
  scale.write(ofs);
  return ofs;
}

//: input operator
std::istream&
operator>>( std::istream& ifs, rgrl_scale& scale )
{
  scale.read(ifs);
  return ifs;
}
