#include <testlib/testlib_test.h>

#include <rgrl/rgrl_match_set.h>

#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <vnl/vnl_vector.h>

#include <rgrl/rgrl_feature_point.h>


namespace {

typedef vcl_vector< rgrl_feature_sptr >  vec_feature;
typedef vcl_vector< rgrl_feature_sptr >::iterator  vec_feature_iter;

vnl_vector<double>
random_2d_vector( )
{
  vnl_vector<double> v(2);
  v[0] = (rand() - RAND_MAX/2.0);
  v[1] = rand() - RAND_MAX/2.0;
  return v;
}

unsigned
random_count( unsigned max )
{
  return unsigned( (double)rand() / RAND_MAX * max );
}

unsigned
index_in( vec_feature const& data, rgrl_feature_sptr const& point )
{
  return unsigned(vcl_find( data.begin(), data.end(), point ) - data.begin());
}

bool
is_in( vec_feature const& data, rgrl_feature_sptr const& point )
{
  return vcl_find( data.begin(), data.end(), point ) != data.end();
}

template< class match_set_type, class from_iter_type>
bool
test_one_to_one_contents( match_set_type* ms,
                          from_iter_type* /*unused*/,
                          vec_feature const& from_pts,
                          vec_feature const& to_pts,
                          vcl_vector<double> const& wgts )
{
  typedef typename from_iter_type::to_iterator   to_iter_type;

  bool okay = true;
  unsigned count = 0; // number of correspondences
  for ( from_iter_type fi = ms->from_begin(); fi != ms->from_end(); ++fi ) {
    if ( ! is_in( from_pts, fi.from_feature() ) ) {
      vcl_cout << "From feature isn't in original list of features\n";
      okay = false;
    } else {
      unsigned idx = index_in( from_pts, fi.from_feature() );

      if ( fi.mapped_from_feature() ) {
        vcl_cout << "From feature "<<idx<<": mapped from feature is wrong\n";
        okay = false;
      }

      // number of correspondences for this "from" feature
      unsigned num_matches = 0;
      for ( to_iter_type ti = fi.begin(); ti != fi.end(); ++ti ) {
        ++num_matches;
        ++count;
      }
      if ( num_matches != 1 ) {
        vcl_cout << "From feature "<<idx<<": has " << num_matches << " matches, not 1\n";
        okay = false;
      } else if ( fi.begin().to_feature() != to_pts[idx] ) {
        vcl_cout << "From feature "<<idx<<": has incorrect to feature\n";
        okay = false;
      } else if ( fi.begin().geometric_weight() != wgts[idx] ) {
        vcl_cout << "From feature "<<idx<<": has incorrect geometric weight\n";
        okay = false;
      } else if ( fi.begin().signature_weight() != wgts[idx] ) {
        vcl_cout << "From feature "<<idx<<": has incorrect signature weight\n";
        okay = false;
      } else if ( fi.begin().cumulative_weight() != wgts[idx] ) {
        vcl_cout << "From feature "<<idx<<": has incorrect cumulative weight\n";
        okay = false;
      }
    }
  }
  if ( count != from_pts.size() ) {
    vcl_cout << "Total number of matches in set ("<<count<<") != actual number of matches ("<<from_pts.size()<<")\n";
    okay = false;
  }

  return okay;
}

void
test_one_to_one_correspondence()
{
  vcl_cout << "Testing one to one correspondence\n";

  vcl_vector< rgrl_feature_sptr > from_pts;
  vcl_vector< rgrl_feature_sptr > to_pts;
  vcl_vector< double > wgts;

  unsigned num_corres = random_count( 100 );
  vcl_cout << "Creating " << num_corres << " correspondences\n";
  for ( unsigned i=0; i < num_corres; ++i ) {
    from_pts.        push_back( new rgrl_feature_point( random_2d_vector() ) );
    to_pts.          push_back( new rgrl_feature_point( random_2d_vector() ) );
    wgts.            push_back( random_count( 10 ) );
  }

  testlib_test_begin( "Constructing match set" );
  rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
  testlib_test_perform( ms && ms->from_size() == 0 );

  testlib_test_begin( "Adding the features" );
  for ( unsigned i=0; i < from_pts.size(); ++i ) {
    ms->add_feature_and_match( from_pts[i], 0, to_pts[i], wgts[i] );
  }
  testlib_test_perform( true );

  testlib_test_begin( "Iterating over all the data" );
  testlib_test_perform( test_one_to_one_contents( ms.as_pointer(), (rgrl_match_set::from_iterator*)0,
                                                  from_pts, to_pts, wgts ) );

  testlib_test_begin( "Iterating over all the data (const iterator)" );
  testlib_test_perform( test_one_to_one_contents( (rgrl_match_set const*)ms.as_pointer(), (rgrl_match_set::const_from_iterator*)0,
                                                  from_pts, to_pts, wgts ) );
}

} // end anonymous namespace

MAIN( test_match_set )
{
  START( "match set" );

  test_one_to_one_correspondence();

  SUMMARY();
}
