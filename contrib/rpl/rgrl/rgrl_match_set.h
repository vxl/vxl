#ifndef rgrl_match_set_h_
#define rgrl_match_set_h_
//:
// \file
// \brief  Represents a set of matches for a given feature type
// \author Amitha Perera
// \date 14 Nov 2002

#include <vcl_vector.h>

#include "rgrl_feature.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_object.h"

#include <vcl_iosfwd.h>

class rgrl_transformation;

// Forward declaration. Used internally by rgrl_match_set.
class rgrl_match_set_from;

// Iterators
class rgrl_match_set_from_iterator;
class rgrl_match_set_from_to_iterator;

class rgrl_match_set_const_from_iterator;
class rgrl_match_set_const_from_to_iterator;

//:
// For each feature in the "from" set, there is a set of features in
// the "to" set. Each "from" point is indexed by an index \a i. For
// each "from" point, there is a set of "to" points, indexed by \a j,
// where the range of \a j depends on \a i. A specific
// feature-to-feature correspondence is therefore given by (\a i, \a
// j).  A "from" point could be repeated with a different index,
// allowing a many-to-many matching.
//
// While one can request the set of "to" matches corresponding to a
// given "from" match, there is currently no functionality to provide
// the reverse. This functionality may be added in future, however, so
// please use with this in mind.
//
class rgrl_match_set
  : public rgrl_object
{
 public:
  typedef unsigned int                        size_type;
  typedef rgrl_match_set_from_iterator        from_iterator;
  typedef rgrl_match_set_const_from_iterator  const_from_iterator;

 public:
  //:  construct an empty match set, specifying the feature type
  rgrl_match_set( const vcl_type_info& feature_type );

  //:  construct an empty match set, specifying the feature type
  rgrl_match_set( const vcl_type_info& from_type,
                  const vcl_type_info& to_type );

  //: The number of "from" features.
  size_type from_size() const;

  const_from_iterator from_begin() const;

  const_from_iterator from_end() const;

  from_iterator from_begin();

  from_iterator from_end();

  //: Remove all the matches from this set.
  void clear();

  //: write out a match set
  void write( vcl_ostream& os ) const;
  
  //: read in a match set
  bool read( vcl_istream& is );
  
  //: Add from feature, the transformed feature, and its matching "to" features.
  //
  void add_feature_and_matches( rgrl_feature_sptr                      from_feature,
                                rgrl_feature_sptr                      mapped_feature,
                                vcl_vector< rgrl_feature_sptr > const& matching_to );

  //: Add from feature, the transformed feature, and its matching "to" features, and the signature weight.
  //
  void add_feature_matches_and_weights( rgrl_feature_sptr                      from_feature,
                                        rgrl_feature_sptr                      mapped_feature,
                                        vcl_vector< rgrl_feature_sptr > const& matching_to,
                                        vcl_vector< double > const&            signature_weights );

  //: Add from feature, the transformed feature, and its matching "to" features, and ALL weights.
  //
  void add_feature_matches_and_weights( rgrl_feature_sptr                      from_feature,
                                        rgrl_feature_sptr                      mapped_feature,
                                        vcl_vector< rgrl_feature_sptr > const& matching_to,
                                        vcl_vector< double > const&            sig_wgts,
                                        vcl_vector< double > const&            geo_wgts,
                                        vcl_vector< double > const&            cum_wgts );

  //: Add from feature and its matching "to" feature.
  //
  // Convenience method for adding a one-to-one correspondence. The \a
  // wgt parameter is used to initialize all the weights for the
  // correspondence (geometric, signature, and cumulative weights).
  //
  void add_feature_and_match( rgrl_feature_sptr                      from_feature,
                              rgrl_feature_sptr                      mapped_feature,
                              rgrl_feature_sptr                      matching_to,
                              double                                 wgt = 1.0 );

  //: Update the cached mapped_from_feature using \a trans.
  //
  // This will remap each from_feature using \a trans and store the
  // result in \a mapped_from_feature.
  //
  void remap_from_features( rgrl_transformation const& trans );

  //  CS (9/20/2003): I am not at all sure that the idea of a single
  //  feature type in a match set is really a good idea, especially
  //  given the use of the error projector.

  //: to access feature types
  //
  const vcl_type_info& from_feature_type() const {return *from_type_;}
  const vcl_type_info& to_feature_type() const {return *to_type_;}

  unsigned int num_constraints_per_match() const;

  //: reserve space for a number of matches
  //
  void reserve( unsigned i ) {
    from_features_.reserve( i );
    xformed_from_features_.reserve(i);
    matches_and_weights_.reserve(i);
  }

 private:
  //: Holds the details of match.
  class match_info
  {
   public:
    //:
    // geometric and cumulative weights are set to -1 to allow
    //  a safety check on their usage.
    //
    match_info( rgrl_feature_sptr to_feat );

    //:
    // All the weights are initialized
    //
    match_info( rgrl_feature_sptr to_feat,
                double geometric_wgt,
                double signature_wgt,
                double cumulative_wgt = 0);

    //:  Initialize the signature weight only
    //
    match_info( rgrl_feature_sptr to_feat,
                double signature_wgt );

    rgrl_feature_sptr to_feature;
    double geometric_weight;
    double signature_weight;
    double cumulative_weight;
  };

  void set_num_constraints_per_match() const;

 private:
  friend class rgrl_match_set_from_iterator;
  friend class rgrl_match_set_const_from_iterator;
  friend class rgrl_match_set_from_to_iterator;
  friend class rgrl_match_set_const_from_to_iterator;


  const vcl_type_info* from_type_;
  const vcl_type_info* to_type_;
  mutable unsigned int num_constraints_per_match_;
  vcl_vector< rgrl_feature_sptr > from_features_;
  vcl_vector< rgrl_feature_sptr > xformed_from_features_;
  vcl_vector< vcl_vector< match_info > > matches_and_weights_;
};

//: stream output
vcl_ostream& 
operator<< ( vcl_ostream& os, rgrl_match_set const& set );

//: stream input
vcl_istream& 
operator>> ( vcl_istream& is, rgrl_match_set& set );

class rgrl_match_set_from_iterator
{
 private:
  typedef rgrl_match_set::match_info                            match_info;
 public:
  typedef rgrl_match_set_from_to_iterator                       to_iterator;
  typedef vcl_vector< vcl_vector< match_info > >::size_type     size_type;

 public:
  //: Default constructor.
  // A default constructed iterator is, of course, invalid until it is
  // set to point into a rgrl_match_set.
  rgrl_match_set_from_iterator( );

  rgrl_match_set_from_iterator& operator++();
  rgrl_match_set_from_iterator& operator+( int );

  bool operator==( const rgrl_match_set_from_iterator& other ) const;
  bool operator!=( const rgrl_match_set_from_iterator& other ) const;

  //: The beginning of the set of "to" matches for this "from".
  to_iterator begin() const;

  //: The end of the set of "to" matches for this "from".
  to_iterator end() const;

  //: The number of "to" matches for this "from"
  size_type size() const;

  //:
  bool empty() const;

  //:
  rgrl_feature_sptr from_feature() const;

  //:
  rgrl_feature_sptr mapped_from_feature() const;

 private:
  friend class rgrl_match_set;

  // For use by rgrl_match_set
  rgrl_match_set_from_iterator( rgrl_match_set* ms,
                                vcl_vector< rgrl_feature_sptr >::size_type ind );

  rgrl_match_set* match_set_;
  vcl_vector< rgrl_feature_sptr >::size_type index_;
};


class rgrl_match_set_const_from_iterator
{
 private:
  typedef rgrl_match_set::match_info                            match_info;
 public:
  typedef rgrl_match_set_const_from_to_iterator                 to_iterator;
  typedef vcl_vector< vcl_vector< match_info > >::size_type     size_type;

 public:
  //: Default constructor.
  // A default constructed iterator is, of course, invalid until it is
  // set to point into a rgrl_match_set.
  rgrl_match_set_const_from_iterator( );

  rgrl_match_set_const_from_iterator& operator++();
  rgrl_match_set_const_from_iterator operator++( int );
  rgrl_match_set_const_from_iterator& operator+( int );

  bool operator==( const rgrl_match_set_const_from_iterator& other ) const;
  bool operator!=( const rgrl_match_set_const_from_iterator& other ) const;

  //: The beginning of the set of "to" matches for this "from".
  to_iterator begin() const;

  //: The end of the set of "to" matches for this "from".
  to_iterator end() const;

  //: The number of "to" matches for this "from"
  size_type size() const;

  //:
  bool empty() const;

  //:
  rgrl_feature_sptr from_feature() const;

  //:
  rgrl_feature_sptr mapped_from_feature() const;

 private:
  friend class rgrl_match_set;

  // For use by rgrl_match_set
  rgrl_match_set_const_from_iterator( rgrl_match_set const* ms,
                                      vcl_vector< rgrl_feature_sptr >::size_type ind );

  rgrl_match_set const* match_set_;
  vcl_vector< rgrl_feature_sptr >::size_type index_;
};


class rgrl_match_set_from_to_iterator
{
 public:
  rgrl_match_set_from_to_iterator( );

  // default copy constructor and assignment operator does the correct
  // job
  // rgrl_match_set_from_to_iterator( rgrl_match_set_from_to_iterator const& );

  //:
  rgrl_match_set_from_to_iterator& operator++();
  rgrl_match_set_from_to_iterator& operator+(int RHS);

  bool operator==( const rgrl_match_set_from_to_iterator& other ) const;
  bool operator!=( const rgrl_match_set_from_to_iterator& other ) const;

  //:
  rgrl_feature_sptr to_feature() const;

  //:
  double geometric_weight() const;

  //:
  double signature_weight() const;

  //:
  double cumulative_weight() const;

  //:
  void set_geometric_weight( double geom_wgt );

  //:
  void set_signature_weight( double sig_wgt );

  //:
  void set_cumulative_weight( double cum_wgt );

 protected:

  friend class rgrl_match_set_from_iterator;
  typedef rgrl_match_set::match_info match_info;
  typedef vcl_vector< match_info >::iterator MatchInfoIter;
  // for use by rgrl_match_set_from_iterator
  rgrl_match_set_from_to_iterator( MatchInfoIter const& itr );

  MatchInfoIter itr_;
};


class rgrl_match_set_const_from_to_iterator
{
 public:
  typedef rgrl_match_set_const_from_to_iterator self_type;
 public:
  rgrl_match_set_const_from_to_iterator( );

  // default copy constructor and assignment operator does the correct
  // job
  // rgrl_match_set_from_to_iterator( rgrl_match_set_from_to_iterator const& );

  //:
  self_type& operator++();
  self_type& operator+(int RHS);

  bool operator==( const self_type& other ) const;
  bool operator!=( const self_type& other ) const;

  //:
  rgrl_feature_sptr to_feature() const;

  //:
  double geometric_weight() const;

  //:
  double signature_weight() const;

  //:
  double cumulative_weight() const;

 protected:

  friend class rgrl_match_set_const_from_iterator;
  typedef rgrl_match_set::match_info match_info;
  typedef vcl_vector< match_info >::const_iterator MatchInfoIter;
  // for use by rgrl_match_set_from_iterator
  rgrl_match_set_const_from_to_iterator( MatchInfoIter const& itr );

  MatchInfoIter itr_;
};


#endif // rgrl_match_set_h_
