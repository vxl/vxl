#include "rgrl_match_set.h"
//:
// \file
// \brief  Represents a set of matches.
// \author Amitha Perera
// \date 14 Nov 2002

#include "rgrl_feature_sptr.h"
#include <rgrl/rgrl_transformation.h>
#include <vnl/vnl_fastops.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

rgrl_match_set::
rgrl_match_set( const vcl_type_info& feature_type )
  : from_type_( &feature_type ),
    to_type_( &feature_type ),
    num_constraints_per_match_( 0 )
{
}

rgrl_match_set::
rgrl_match_set( const vcl_type_info& from_type,
                const vcl_type_info& to_type )
  : from_type_( &from_type ),
    to_type_( &to_type ),
    num_constraints_per_match_( 0 )
{
}


rgrl_match_set::size_type
rgrl_match_set::
from_size() const
{
  return from_features_.size();
}


rgrl_match_set::const_from_iterator
rgrl_match_set::
from_begin() const
{
  return const_from_iterator( this, 0 );
}


rgrl_match_set::const_from_iterator
rgrl_match_set::
from_end() const
{
  return const_from_iterator( this, from_features_.size() );
}


rgrl_match_set::from_iterator
rgrl_match_set::
from_begin()
{
  return from_iterator( this, 0 );
}


rgrl_match_set::from_iterator
rgrl_match_set::
from_end()
{
  return from_iterator( this, from_features_.size() );
}


void
rgrl_match_set::
clear()
{
  from_features_.clear();
  xformed_from_features_.clear();
  matches_and_weights_.clear();
}


void
rgrl_match_set ::
add_feature_and_matches( rgrl_feature_sptr                      from_feature,
                         rgrl_feature_sptr                      mapped_feature,
                         vcl_vector< rgrl_feature_sptr > const& matching_to )
{
  vcl_vector< rgrl_feature_sptr >::const_iterator to_itr;

  //  Check to make sure all features involve the same feature types

  assert ( from_feature->is_type( *(this->from_type_) ) );
  for ( to_itr = matching_to.begin(); to_itr != matching_to.end(); ++to_itr ) {
    assert ( (*to_itr)->is_type( *(this->to_type_ ) ) );
  }

  // Add from and xformed from

  from_features_.push_back( from_feature );
  xformed_from_features_.push_back( mapped_feature );

  // Add matches, with the initial sig weights computed by the from features.

  vcl_vector<match_info> blank;
  matches_and_weights_.push_back( blank );
  for ( to_itr = matching_to.begin(); to_itr != matching_to.end(); ++to_itr )
  {
    double sig_wgt = (*to_itr)->absolute_signature_weight( mapped_feature );
    matches_and_weights_.back().push_back( match_info( *to_itr, sig_wgt ));
  }
}


void
rgrl_match_set ::
add_feature_matches_and_weights( rgrl_feature_sptr                      from_feature,
                                 rgrl_feature_sptr                      mapped_feature,
                                 vcl_vector< rgrl_feature_sptr > const& matching_to,
                                 vcl_vector< double > const&            signature_weights )
{
  vcl_vector< rgrl_feature_sptr >::const_iterator to_itr;

  //  Check to make sure all features involve the same feature types

  assert ( from_feature->is_type( *(this->from_type_ ) ) );
  for ( to_itr = matching_to.begin(); to_itr != matching_to.end(); ++to_itr ) {
    assert ( (*to_itr)->is_type( *(this->to_type_ ) ) );
  }

  // Make sure the

  assert ( matching_to.size() == signature_weights.size() );

  // Add from and xformed from

  from_features_.push_back( from_feature );
  xformed_from_features_.push_back( mapped_feature );

  // Add matches, with default initial weights.

  vcl_vector<match_info> blank;
  vcl_vector<double>::const_iterator s_itr;
  matches_and_weights_.push_back( blank );
  for ( to_itr = matching_to.begin(), s_itr = signature_weights.begin();
        to_itr != matching_to.end(); ++to_itr,  ++s_itr )
  {
    matches_and_weights_.back().push_back( match_info( *to_itr, *s_itr ));
  }
}

void
rgrl_match_set ::
add_feature_and_match( rgrl_feature_sptr from_feature,
                       rgrl_feature_sptr mapped_feature,
                       rgrl_feature_sptr matching_to,
                       double            wgt )
{
  //  Check to make sure all features involve the same feature types

  assert ( from_feature->is_type( *(this->from_type_ ) ) );
  assert ( matching_to->is_type( *(this->to_type_ ) ) );

  // Add from and xformed from

  from_features_.push_back( from_feature );
  xformed_from_features_.push_back( mapped_feature );

  vcl_vector<match_info> match;
  match.push_back( match_info( matching_to, wgt, wgt, wgt ) );
  matches_and_weights_.push_back( match );
}


void
rgrl_match_set::
remap_from_features( rgrl_transformation const& trans )
{
  assert ( from_features_.size() == xformed_from_features_.size() );

  vcl_vector<rgrl_feature_sptr>::size_type i = 0;
  for ( ; i < from_features_.size(); ++i ) {
    xformed_from_features_[i] = from_features_[i]->transform( trans );
  }
}

void
rgrl_match_set::
update_geometric_error( rgrl_transformation_sptr const& trans )
{
  assert( trans );
  
  // save computation when transformation is identitcal
  if( trans == prev_xform_cached_ )
    return;
  else
    prev_xform_cached_ = trans;
  
  // valid check
  if( from_features_.empty() ) 
    return;
  
  // allow fast computation?  
  const bool fast_computation = from_features_[0]->allow_fast_computation_on_error();
  if( fast_computation ) {
    // these can be resued
    vnl_vector<double> mapped_loc, diff, proj;
    for( from_iterator fitr=from_begin(); fitr!=from_end(); ++fitr ) 
      if( !fitr.empty() ){
        // map location
        trans->map_location( fitr.from_feature()->location(), mapped_loc );
  
        for( from_iterator::to_iterator titr=fitr.begin(); titr!=fitr.end(); ++titr ) {
          //borrow from rgrl_feature::geometric_error
          diff = mapped_loc;
          diff -= titr.to_feature()->location();
          vnl_fastops::Ab( proj, titr.to_feature()->error_projector(), diff );
          // set error
          titr.set_geometric_error( proj.two_norm() );
        }
      }
  }
  else {
    // re-map from features
    remap_from_features( *trans );

    for( from_iterator fitr=from_begin(); fitr!=from_end(); ++fitr ) 
      if( !fitr.empty() ){
        for( from_iterator::to_iterator titr=fitr.begin(); titr!=fitr.end(); ++titr ) {
          // set error
          double err = titr.to_feature()->geometric_error( *(fitr.mapped_from_feature()) );
          titr.set_geometric_error( err );
        }
      }
  }      
}

unsigned int
rgrl_match_set::
num_constraints_per_match() const
{
  if ( !num_constraints_per_match_ )
    set_num_constraints_per_match();

  return num_constraints_per_match_;
}

void
rgrl_match_set::
set_num_constraints_per_match() const
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  bool has_to_features = false;
  for ( FIter fi = this->from_begin(); fi != this->from_end(); ++fi ) {
    for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
      num_constraints_per_match_ = ti.to_feature()->num_constraints();
      has_to_features = true;
      break;
    }
    if ( has_to_features ) break;
  }
}

// ---------------------------------------------------------------------------
//                                                                  match info
//

rgrl_match_set::match_info::
match_info( rgrl_feature_sptr to_feat )
  : to_feature( to_feat ),
    geometric_weight( -1.0 ),
    signature_weight( 1.0 ),
    cumulative_weight( -1.0 ),
    geometric_residual( 0.0 )
{
}


rgrl_match_set::match_info::
match_info( rgrl_feature_sptr to_feat,
            double geometric_wgt,
            double signature_wgt,
            double cumulative_wgt,
            double geometric_err )
  : to_feature( to_feat ),
    geometric_weight( geometric_wgt ),
    signature_weight( signature_wgt ),
    cumulative_weight( cumulative_wgt ),
    geometric_residual( geometric_err )
{
}


rgrl_match_set::match_info::
match_info( rgrl_feature_sptr to_feat,
            double signature_wgt )
  : to_feature( to_feat ),
    geometric_weight( -1.0 ),
    signature_weight( signature_wgt ),
    cumulative_weight( -1.0 ),
    geometric_residual( 0.0 )
{
}


// ---------------------------------------------------------------------------
//                                                               from iterator
//

rgrl_match_set_from_iterator::
rgrl_match_set_from_iterator()
{
};


rgrl_match_set_from_iterator&
rgrl_match_set_from_iterator::
operator++()
{
  ++this->index_;
  return *this;
}

rgrl_match_set_from_iterator&
rgrl_match_set_from_iterator::
operator+(int RHS)
{
  this->index_ += RHS;
  return *this;
}

bool
rgrl_match_set_from_iterator::
operator==( const rgrl_match_set_from_iterator& other ) const
{
  return this->index_ == other.index_;
}


bool
rgrl_match_set_from_iterator::
operator!=( const rgrl_match_set_from_iterator& other ) const
{
  return this->index_ != other.index_;
}


rgrl_match_set_from_iterator::to_iterator
rgrl_match_set_from_iterator::
begin() const
{
  return to_iterator( this->match_set_->matches_and_weights_[this->index_].begin() );
}


rgrl_match_set_from_iterator::to_iterator
rgrl_match_set_from_iterator::
end() const
{
  return to_iterator( this->match_set_->matches_and_weights_[this->index_].end() );
}


rgrl_match_set_from_iterator::size_type
rgrl_match_set_from_iterator::
size() const
{
  return this->match_set_->matches_and_weights_[this->index_].size();
}


bool
rgrl_match_set_from_iterator::
empty() const
{
  return this->size() == 0;
}


rgrl_feature_sptr
rgrl_match_set_from_iterator::
from_feature() const
{
  return this->match_set_->from_features_[this->index_];
}


rgrl_feature_sptr
rgrl_match_set_from_iterator::
mapped_from_feature() const
{
  return this->match_set_->xformed_from_features_[this->index_];
}


rgrl_match_set_from_iterator::
rgrl_match_set_from_iterator( rgrl_match_set* ms,
                              vcl_vector< rgrl_feature_sptr >::size_type ind )
  : match_set_( ms ),
    index_( ind )
{
}


// ---------------------------------------------------------------------------
//                                                            from to iterator
//


rgrl_match_set_from_to_iterator::
rgrl_match_set_from_to_iterator()
{
};


rgrl_match_set_from_to_iterator&
rgrl_match_set_from_to_iterator::
operator++()
{
  ++itr_;
  return *this;
}

rgrl_match_set_from_to_iterator&
rgrl_match_set_from_to_iterator::
operator+(int RHS)
{
  itr_ += RHS;
  return *this;
}


bool
rgrl_match_set_from_to_iterator::
operator==( const rgrl_match_set_from_to_iterator& other ) const
{
  return this->itr_ == other.itr_;
}


bool
rgrl_match_set_from_to_iterator::
operator!=( const rgrl_match_set_from_to_iterator& other ) const
{
  return this->itr_ != other.itr_;
}


rgrl_feature_sptr
rgrl_match_set_from_to_iterator::
to_feature() const
{
  return itr_->to_feature;
}


double
rgrl_match_set_from_to_iterator::
geometric_weight() const
{
  return itr_->geometric_weight;
}


double
rgrl_match_set_from_to_iterator::
signature_weight() const
{
  return itr_->signature_weight;
}


double
rgrl_match_set_from_to_iterator::
cumulative_weight( ) const
{
  return itr_->cumulative_weight;
}


void
rgrl_match_set_from_to_iterator::
set_geometric_weight( double geom_wgt )
{
  itr_->geometric_weight = geom_wgt;
}


void
rgrl_match_set_from_to_iterator::
set_signature_weight( double sig_wgt )
{
  itr_->signature_weight = sig_wgt;
}


void
rgrl_match_set_from_to_iterator::
set_cumulative_weight( double cum_wgt )
{
  itr_->cumulative_weight = cum_wgt;
}

void
rgrl_match_set_from_to_iterator::
set_geometric_error( double geom_err )
{
  itr_->geometric_residual = geom_err;
}

rgrl_match_set_from_to_iterator::
rgrl_match_set_from_to_iterator( MatchInfoIter const& itr )
  : itr_( itr )
{
}


// ---------------------------------------------------------------------------
//                                                         const from iterator
//

rgrl_match_set_const_from_iterator::
rgrl_match_set_const_from_iterator()
{
};


rgrl_match_set_const_from_iterator&
rgrl_match_set_const_from_iterator::
operator++()
{
  ++this->index_;
  return *this;
}

rgrl_match_set_const_from_iterator&
rgrl_match_set_const_from_iterator::
operator+(int RHS)
{
  this->index_ += RHS;
  return *this;
}

bool
rgrl_match_set_const_from_iterator::
operator==( const rgrl_match_set_const_from_iterator& other ) const
{
  return this->index_ == other.index_;
}


bool
rgrl_match_set_const_from_iterator::
operator!=( const rgrl_match_set_const_from_iterator& other ) const
{
  return this->index_ != other.index_;
}


rgrl_match_set_const_from_iterator::to_iterator
rgrl_match_set_const_from_iterator::
begin() const
{
  return to_iterator( this->match_set_->matches_and_weights_[this->index_].begin() );
}


rgrl_match_set_const_from_iterator::to_iterator
rgrl_match_set_const_from_iterator::
end() const
{
  return to_iterator( this->match_set_->matches_and_weights_[this->index_].end() );
}


rgrl_match_set_const_from_iterator::size_type
rgrl_match_set_const_from_iterator::
size() const
{
  return this->match_set_->matches_and_weights_[this->index_].size();
}


bool
rgrl_match_set_const_from_iterator::
empty() const
{
  return this->size() == 0;
}


rgrl_feature_sptr
rgrl_match_set_const_from_iterator::
from_feature() const
{
  return this->match_set_->from_features_[this->index_];
}


rgrl_feature_sptr
rgrl_match_set_const_from_iterator::
mapped_from_feature() const
{
  return this->match_set_->xformed_from_features_[this->index_];
}


rgrl_match_set_const_from_iterator::
rgrl_match_set_const_from_iterator( rgrl_match_set const* ms,
                                    vcl_vector< rgrl_feature_sptr >::size_type ind )
  : match_set_( ms ),
    index_( ind )
{
}


// ---------------------------------------------------------------------------
//                                                      const from to iterator
//

rgrl_match_set_const_from_to_iterator::
rgrl_match_set_const_from_to_iterator()
{
};


rgrl_match_set_const_from_to_iterator&
rgrl_match_set_const_from_to_iterator::
operator++()
{
  ++itr_;
  return *this;
}

rgrl_match_set_const_from_to_iterator&
rgrl_match_set_const_from_to_iterator::
operator+(int RHS)
{
  itr_ += RHS;
  return *this;
}

bool
rgrl_match_set_const_from_to_iterator::
operator==( const self_type& other ) const
{
  return this->itr_ == other.itr_;
}


bool
rgrl_match_set_const_from_to_iterator::
operator!=( const self_type& other ) const
{
  return this->itr_ != other.itr_;
}


rgrl_feature_sptr
rgrl_match_set_const_from_to_iterator::
to_feature() const
{
  return itr_->to_feature;
}


double
rgrl_match_set_const_from_to_iterator::
geometric_weight() const
{
  return itr_->geometric_weight;
}


double
rgrl_match_set_const_from_to_iterator::
signature_weight() const
{
  return itr_->signature_weight;
}


double
rgrl_match_set_const_from_to_iterator::
cumulative_weight( ) const
{
  return itr_->cumulative_weight;
}


rgrl_match_set_const_from_to_iterator::
rgrl_match_set_const_from_to_iterator( MatchInfoIter const& itr )
  : itr_( itr )
{
}
