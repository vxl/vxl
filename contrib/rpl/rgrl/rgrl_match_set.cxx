#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "rgrl_match_set.h"
//:
// \file
// \brief  Represents a set of matches.
// \author Amitha Perera
// \date 14 Nov 2002
// \verbatim
// Modifications
//      Nov 2008 J Becker: Change to use the new feature reader.
// \endverbatim

#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_transformation.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

rgrl_match_set::
rgrl_match_set(  )
  : from_type_( nullptr ),
    to_type_( nullptr ),
    num_constraints_per_match_( 0 )
{
}

rgrl_match_set::
rgrl_match_set( const std::type_info& feature_type )
  : from_type_( &feature_type ),
    to_type_( &feature_type ),
    num_constraints_per_match_( 0 )
{
}

rgrl_match_set::
rgrl_match_set( const std::type_info& from_type,
                const std::type_info& to_type,
                rgrl_feature_set_label  from_label,
                rgrl_feature_set_label  to_label )
  : from_type_( &from_type ),
    to_type_( &to_type ),
    from_label_(std::move( from_label )),
    to_label_(std::move( to_label )),
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
  return { this, 0 };
}


rgrl_match_set::const_from_iterator
rgrl_match_set::
from_end() const
{
  return { this, from_features_.size() };
}


rgrl_match_set::from_iterator
rgrl_match_set::
from_begin()
{
  return { this, 0 };
}


rgrl_match_set::from_iterator
rgrl_match_set::
from_end()
{
  return { this, from_features_.size() };
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
add_feature_and_matches( const rgrl_feature_sptr&                      from_feature,
                         const rgrl_feature_sptr&                      mapped_feature,
                         std::vector< rgrl_feature_sptr > const& matching_to )
{
  std::vector< rgrl_feature_sptr >::const_iterator to_itr;

  //  Check to make sure all features involve the same feature types

  assert ( from_feature->is_type( *(this->from_type_) ) );
  for ( to_itr = matching_to.begin(); to_itr != matching_to.end(); ++to_itr ) {
    assert ( (*to_itr)->is_type( *(this->to_type_ ) ) );
  }

  // Add from and xformed from

  from_features_.push_back( from_feature );
  xformed_from_features_.push_back( mapped_feature );

  // Add matches, with the initial sig weights computed by the from features.

  std::vector<match_info> blank;
  matches_and_weights_.push_back( blank );
  for ( to_itr = matching_to.begin(); to_itr != matching_to.end(); ++to_itr )
  {
    double sig_wgt = (*to_itr)->absolute_signature_weight( mapped_feature );
    matches_and_weights_.back().push_back( match_info( *to_itr, sig_wgt ));
  }
}


void
rgrl_match_set ::
add_feature_matches_and_weights( const rgrl_feature_sptr&                      from_feature,
                                 const rgrl_feature_sptr&                      mapped_feature,
                                 std::vector< rgrl_feature_sptr > const& matching_to,
                                 std::vector< double > const&            signature_weights )
{
  std::vector< rgrl_feature_sptr >::const_iterator to_itr;

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

  std::vector<match_info> blank;
  std::vector<double>::const_iterator s_itr;
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
add_feature_matches_and_weights( const rgrl_feature_sptr&                      from_feature,
                                 const rgrl_feature_sptr&                      mapped_feature,
                                 std::vector< rgrl_feature_sptr > const& matching_to,
                                 std::vector< double > const&            sig_wgts,
                                 std::vector< double > const&            geo_wgts,
                                 std::vector< double > const&            cum_wgts )
{
  std::vector< rgrl_feature_sptr >::const_iterator to_itr;

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
  std::vector<match_info> blank;
  matches_and_weights_.push_back( blank );
  auto back_it
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
add_feature_and_match( const rgrl_feature_sptr& from_feature,
                       const rgrl_feature_sptr& mapped_feature,
                       rgrl_feature_sptr matching_to,
                       double            wgt )
{
  //  Check to make sure all features involve the same feature types

  assert ( from_feature->is_type( *(this->from_type_ ) ) );
  assert ( matching_to->is_type( *(this->to_type_ ) ) );

  // Add from and xformed from

  from_features_.push_back( from_feature );
  xformed_from_features_.push_back( mapped_feature );

  std::vector<match_info> match;
  match.emplace_back( matching_to, wgt, wgt, wgt );
  matches_and_weights_.push_back( match );
}


void
rgrl_match_set::
remap_from_features( rgrl_transformation const& trans )
{
  assert ( from_features_.size() == xformed_from_features_.size() );

  std::vector<rgrl_feature_sptr>::size_type i = 0;
  for ( ; i < from_features_.size(); ++i ) {
    xformed_from_features_[i] = from_features_[i]->transform( trans );
  }
}

void
rgrl_match_set::
remap_only_location( rgrl_transformation const& trans )
{
  assert ( from_features_.size() == xformed_from_features_.size() );

  std::vector<rgrl_feature_sptr>::size_type i = 0;
  vnl_vector<double> mapped_loc;
  for ( ; i < from_features_.size(); ++i ) {

    // remap only location
    trans.map_location( from_features_[i]->location(), mapped_loc );
    xformed_from_features_[i]->set_location( mapped_loc );

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
write( std::ostream& os ) const
{
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  // size
  os << this->from_size() << std::endl;

  for( FIter fi=this->from_begin(); fi!=this->from_end(); ++fi ) {

    // from feature
    fi.from_feature()->write( os );

    // mapped feature
    fi.mapped_from_feature()->write( os );

    // to size
    os << fi.size() << std::endl;

    for( TIter ti=fi.begin(); ti!=fi.end(); ++ti )  {
      os << ti.signature_weight() << ' '
         << ti.geometric_weight() << ' '
         << ti.cumulative_weight() << std::endl;

      // to feature
      ti.to_feature()->write( os );
    }
    os << std::endl;
  }

  os << "\n\n";
  //return os;
}

//using anonymous namespace to restrict the use
namespace{
  struct sort_node {

    unsigned ind_;
    rgrl_feature_sptr fea_;

    sort_node()
      : ind_(0)
    { }

    sort_node( unsigned i, const rgrl_feature_sptr& f)
      : ind_(i), fea_(f)
    { }

    bool operator<( const sort_node& rhs ) const
    {
      const vnl_vector<double>&  loc = fea_->location();
      const vnl_vector<double>&  rhs_loc = rhs.fea_->location();

      if( loc[0] < rhs_loc[0] )
        return true;
      else if( loc[0] > rhs_loc[0] )
        return false;
      else
        return loc[1] < rhs_loc[1];
    }
  };
}

//: stream output
void
rgrl_match_set::
write_sorted( std::ostream& os ) const
{
  std::vector< sort_node > nodes;

  for( unsigned i=0; i<from_features_.size(); ++i ){
    nodes.emplace_back( i, from_features_[i] );
  }
  std::sort( nodes.begin(), nodes.end() );

  os << from_features_.size() << std::endl;

  unsigned index;
  for(auto & node : nodes){

    index = node.ind_;

    // output the index(th) match
    from_features_[index]->write( os );
    xformed_from_features_[index]->write( os );

    const std::vector<match_info>& this_match = matches_and_weights_[index];
    // to size
    os << this_match.size() << std::endl;

    for(const auto & ti : this_match)  {
      os << ti.signature_weight << ' '
         << ti.geometric_weight << ' '
         << ti.cumulative_weight << std::endl;

      // to feature
      ti.to_feature->write( os );
    }
    os << std::endl;
  }

  os << "\n\n";
}

//: stream input
bool
rgrl_match_set::
read( std::istream& is )
{
  double sig, geo, cum;
  std::vector<double> sig_wgts, geo_wgts, cum_wgts;
  std::vector<rgrl_feature_sptr>  tos;
  rgrl_feature_sptr from, mapped, one;
  bool to_set_feature_type = true;

  // from size
  int from_size=-1;
  is >> from_size;
  if( !is || from_size<0 ) {
    std::cerr << "Error(" << __FILE__ <<"): cannot read from size" << std::endl;
    return false;
  }

  for( int i=0; i<from_size; ++i ) {

    // from feature
    from = rgrl_feature_reader::read( is );

    // mapped feature
    mapped = rgrl_feature_reader::read( is );
    if( !is || !from || !mapped ) {
      std::cerr << "Error(" << __FILE__ <<"): cannot read from feature" << std::endl;
      return false;
    }

    // to size
    int to_size=-1;
    is >> to_size;
    if( !is || to_size<0 ) {
      std::cerr << "Error(" << __FILE__ <<"): cannot read to feature size" << std::endl;
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
        std::cerr << "Error(" << __FILE__ <<"): cannot read wgts" << std::endl;
        return false;
      }

      sig_wgts.push_back( sig );
      geo_wgts.push_back( geo );
      cum_wgts.push_back( cum );

      one = rgrl_feature_reader( is );
      if( !is || !one ) {
        std::cerr << "Error(" << __FILE__ <<"): cannot read to features" << std::endl;
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
std::ostream&
operator<< ( std::ostream& os, rgrl_match_set const& set )
{
  set.write( os );
  return os;
}

//: stream input
std::istream&
operator>> ( std::istream& is, rgrl_match_set& set )
{
  bool ret = set.read( is );
  assert( ret );
  return is;
}

// ---------------------------------------------------------------------------
//                                                                  match info
//

rgrl_match_set::match_info::
match_info( const rgrl_feature_sptr& to_feat )
  : to_feature( to_feat ),
    geometric_weight( -1.0 ),
    signature_weight( 1.0 ),
    cumulative_weight( -1.0 )
{
}


rgrl_match_set::match_info::
match_info( const rgrl_feature_sptr& to_feat,
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
match_info( const rgrl_feature_sptr& to_feat,
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
rgrl_match_set_from_iterator() = default;;


rgrl_match_set_from_iterator&
rgrl_match_set_from_iterator::
operator++()
{
  ++this->index_;
  return *this;
}

rgrl_match_set_from_iterator
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
                              std::vector< rgrl_feature_sptr >::size_type ind )
  : match_set_( ms ),
    index_( ind )
{
}


// ---------------------------------------------------------------------------
//                                                            from to iterator
//


rgrl_match_set_from_to_iterator::
rgrl_match_set_from_to_iterator() = default;;


rgrl_match_set_from_to_iterator&
rgrl_match_set_from_to_iterator::
operator++()
{
  ++itr_;
  return *this;
}

rgrl_match_set_from_to_iterator
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
rgrl_match_set_const_from_iterator() = default;;

rgrl_match_set_const_from_iterator::
rgrl_match_set_const_from_iterator( rgrl_match_set_from_iterator const& from_iter )
  : match_set_( from_iter.match_set_ ),
    index_( from_iter.index_ )
{

}

rgrl_match_set_const_from_iterator&
rgrl_match_set_const_from_iterator::
operator++()
{
  ++this->index_;
  return *this;
}

rgrl_match_set_const_from_iterator
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
                                    std::vector< rgrl_feature_sptr >::size_type ind )
  : match_set_( ms ),
    index_( ind )
{
}


// ---------------------------------------------------------------------------
//                                                      const from to iterator
//

rgrl_match_set_const_from_to_iterator::
rgrl_match_set_const_from_to_iterator() = default;;

//: copy constructor
//  it is used to convert from_to_iterator to const type
rgrl_match_set_const_from_to_iterator::
rgrl_match_set_const_from_to_iterator( rgrl_match_set_from_to_iterator const& to_iter )
  : itr_( to_iter.itr_ )
{
}
rgrl_match_set_const_from_to_iterator&
rgrl_match_set_const_from_to_iterator::
operator++()
{
  ++itr_;
  return *this;
}

rgrl_match_set_const_from_to_iterator
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
