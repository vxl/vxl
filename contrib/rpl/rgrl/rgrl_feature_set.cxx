//:
// \file
// \author Amitha Perera
// \date Feb 2002

#include <utility>

#include "rgrl_feature_set.h"

rgrl_feature_set::
rgrl_feature_set(feature_vector  fea_vec, rgrl_feature_set_label  label)
 :  label_(std::move(label)), fea_vec_(std::move(fea_vec))
{
}

rgrl_feature_set::
~rgrl_feature_set() = default;


// ============================================================================
//                                                      rgrl_feature_set_label
// ============================================================================

rgrl_feature_set_label::
rgrl_feature_set_label( std::string  name )
  : name_(std::move( name ))
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
