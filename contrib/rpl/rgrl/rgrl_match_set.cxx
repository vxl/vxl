#include "rgrl_match_set.h"
//:
// \file
// \brief  Represents a set of matches.
// \author Amitha Perera
// \date 14 Nov 2002

#include "rgrl_feature_sptr.h"

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
  matches_and_weights_.back().reserve( matching_to.size() );
  for ( to_itr = matching_to.begin(), s_itr = signature_weights.begin();
        to_itr != matching_to.end(); ++to_itr,  ++s_itr )
  {
    matches_and_weights_.back().push_back( match_info( *to_itr, *s_itr ));
  }
}

void
rgrl_match_set ::
add_feature_matches_and_weights( rgrl_feature_sptr                      from_feature,
                                 rgrl_feature_sptr                      mapped_feature,
                                 vcl_vector< rgrl_feature_sptr > const& matching_to,
                                 vcl_vector< double > const&            sig_wgts,
                                 vcl_vector< double > const&            geo_wgts,
                                 vcl_vector< double > const&            cum_wgts )
{
  vcl_vector< rgrl_feature_sptr >::const_iterator to_itr;

  //  Check to make sure all features involve the same feature types

  assert ( from_feature->is_type( *(this->from_type_ ) ) );
  for ( to_itr = matching_to.begin(); to_itr != matching_to.end(); ++to_itr ) {
    assert ( (*to_itr)->is_type( *(this->to_type_ ) ) );
  }

  // Make sure the

  assert ( matching_to.size() == sig_wgts.size() );
  assert ( matching_to.size() == geo_wgts.size() );
  assert ( matching_to.size() == cum_wgts.size() );

  // Add from and xformed from

  from_features_.push_back( from_feature );
  xformed_from_features_.push_back( mapped_feature );

  // Add matches, with default initial weights.
  //
  vcl_vector<match_info> blank;
  matches_and_weights_.push_back( blank );
  vcl_vector< vcl_vector< match_info > >::reverse_iterator back_it 
    = matches_and_weights_.rbegin();

  const unsigned size = matching_to.size();
  back_it->reserve( size);
  for( unsigned i=0; i<size; ++i ) {
    back_it->push_back( match_info( matching_to[i], geo_wgts[i], 
                                    sig_wgts[i], cum_wgts[i] ) );
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
//                                                             stream operator
//

//: stream output
void
rgrl_match_set::
write( vcl_ostream& os ) const
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  // size
  os << this->from_size() << vcl_endl;
  
  for( FIter fi=this->from_begin(); fi!=this->from_end(); ++fi ) {

    // from feature
    fi.from_feature()->write( os );
    
    // mapped feature
    fi.mapped_from_feature()->write( os );
    
    // to size
    os << fi.size() << vcl_endl;
    
    for( TIter ti=fi.begin(); ti!=fi.end(); ++ti )  {
      os << ti.signature_weight() << ' ' 
         << ti.geometric_weight() << ' ' 
         << ti.cumulative_weight() << vcl_endl;
      
      // to feature
      ti.to_feature()->write( os );
    }
    os << vcl_endl;
  }
  
  os << "\n\n";
  //return os;
} 
    
//: stream input
bool
rgrl_match_set::
read( vcl_istream& is )
{
  double sig, geo, cum;
  vcl_vector<double> sig_wgts, geo_wgts, cum_wgts;
  vcl_vector<rgrl_feature_sptr>  tos;
  rgrl_feature_sptr from, mapped, one;
  bool to_set_feature_type = true;
  
  // from size
  int from_size=-1;
  is >> from_size;
  if( !is || from_size<0 ) {
    vcl_cerr << "Error(" << __FILE__ <<"): cannot read from size" << vcl_endl;
    return false;
  }
  
  for( int i=0; i<from_size; ++i ) {
    
    // from feature
    from = rgrl_feature_reader( is );
      
    // mapped feature
    mapped = rgrl_feature_reader( is );
    if( !is || !from || !mapped ) {
      vcl_cerr << "Error(" << __FILE__ <<"): cannot read from feature" << vcl_endl;
      return false;
    }

    // to size
    int to_size=-1;
    is >> to_size;
    if( !is || to_size<0 ) {
      vcl_cerr << "Error(" << __FILE__ <<"): cannot read to feature size" << vcl_endl;
      return false;
    }
    
    // reset
    sig_wgts.clear();
    geo_wgts.clear();
    cum_wgts.clear();
    tos.clear();
    
    for( int i=0; i<to_size; ++i ) {
      is >> sig >> geo >> cum;
      if( !is ) {
        vcl_cerr << "Error(" << __FILE__ <<"): cannot read wgts" << vcl_endl;
        return false;
      }
      
      sig_wgts.push_back( sig );
      geo_wgts.push_back( geo );
      cum_wgts.push_back( cum );
      
      one = rgrl_feature_reader( is );
      if( !is || !one ) {
        vcl_cerr << "Error(" << __FILE__ <<"): cannot read to features" << vcl_endl;
        return false;
      }
      tos.push_back( one );
    }
    
    // set feature type
    if( to_set_feature_type && !tos.empty() ) {
      //set flag
      to_set_feature_type = false; 
      
      from_type_ = &(from->type_id());
      to_type_ = &(tos[0]->type_id());
    }

    // add features and weights
    this->add_feature_matches_and_weights( from, mapped, tos, sig_wgts, geo_wgts, cum_wgts );

  }
  
  // set num_constraints
  set_num_constraints_per_match();
  
  return true;
}

//: stream output
vcl_ostream& 
operator<< ( vcl_ostream& os, rgrl_match_set const& set )
{
  set.write( os );
  return os;
}

//: stream input
vcl_istream& 
operator>> ( vcl_istream& is, rgrl_match_set& set )
{
  bool ret = set.read( is );
  assert( ret );
  return is;
}

// ---------------------------------------------------------------------------
//                                                                  match info
//

rgrl_match_set::match_info::
match_info( rgrl_feature_sptr to_feat )
  : to_feature( to_feat ),
    geometric_weight( -1.0 ),
    signature_weight( 1.0 ),
    cumulative_weight( -1.0 )
{
}


rgrl_match_set::match_info::
match_info( rgrl_feature_sptr to_feat,
            double geometric_wgt,
            double signature_wgt,
            double cumulative_wgt )
  : to_feature( to_feat ),
    geometric_weight( geometric_wgt ),
    signature_weight( signature_wgt ),
    cumulative_weight( cumulative_wgt )
{
}


rgrl_match_set::match_info::
match_info( rgrl_feature_sptr to_feat,
            double signature_wgt )
  : to_feature( to_feat ),
    geometric_weight( -1.0 ),
    signature_weight( signature_wgt ),
    cumulative_weight( -1.0 )
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


rgrl_feature_sptr const&
rgrl_match_set_const_from_iterator::
from_feature() const
{
  return this->match_set_->from_features_[this->index_];
}


rgrl_feature_sptr const&
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


rgrl_feature_sptr const&
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
