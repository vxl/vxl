//:
// \file
// \author Amitha Perera
// \date Feb 2002

#include "rgrl_feature_set.h"

rgrl_feature_set::
rgrl_feature_set()
{
}

rgrl_feature_set::
~rgrl_feature_set()
{
}


// ============================================================================
//                                                      rgrl_feature_set_label 
// ============================================================================

rgrl_feature_set_label::
rgrl_feature_set_label( const vcl_string& name )
  : name_( name )
{
}


bool
rgrl_feature_set_label::
operator==( rgrl_feature_set_label const& other ) const
{
  return this->name_ == other.name_;
}


bool
rgrl_feature_set_label::
operator!=( rgrl_feature_set_label const& other ) const
{
  return this->name_ != other.name_;
}


bool
rgrl_feature_set_label::
operator<( rgrl_feature_set_label const& other ) const
{
  return this->name_ < other.name_;
}
