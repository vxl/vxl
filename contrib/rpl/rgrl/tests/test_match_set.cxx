#include <vector>
#include <iostream>
#include <algorithm>
#include <testlib/testlib_test.h>

#include <rgrl/rgrl_match_set.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_vector.h>

#include <rgrl/rgrl_feature_point.h>


namespace {

typedef std::vector< rgrl_feature_sptr >  vec_feature;
typedef std::vector< rgrl_feature_sptr >::iterator  vec_feature_iter;

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
  return unsigned(std::find( data.begin(), data.end(), point ) - data.begin());
}

bool
is_in( vec_feature const& data, rgrl_feature_sptr const& point )
{
  return std::find( data.begin(), data.end(), point ) != data.end();
}

template< class match_set_type, class from_iter_type>
bool
test_one_to_one_contents( match_set_type* ms,
                          from_iter_type* /*unused*/,
                          vec_feature const& from_pts,
                          vec_feature const& to_pts,
                          std::vector<double> const& wgts )
{
  typedef typename from_iter_type::to_iterator   to_iter_type;

  bool okay = true;
  unsigned count = 0; // number of correspondences
  for ( from_iter_type fi = ms->from_begin(); fi != ms->from_end(); ++fi ) {
    if ( ! is_in( from_pts, fi.from_feature() ) ) {
      std::cout << "From feature isn't in original list of features\n";
      okay = false;
    } else {
      unsigned idx = index_in( from_pts, fi.from_feature() );

      if ( fi.mapped_from_feature() ) {
        std::cout << "From feature "<<idx<<": mapped from feature is wrong\n";
        okay = false;
      }

      // number of correspondences for this "from" feature
      unsigned num_matches = 0;
      for ( to_iter_type ti = fi.begin(); ti != fi.end(); ++ti ) {
        ++num_matches;
        ++count;
      }
      if ( num_matches != 1 ) {
        std::cout << "From feature "<<idx<<": has " << num_matches << " matches, not 1\n";
        okay = false;
      } else if ( fi.begin().to_feature() != to_pts[idx] ) {
        std::cout << "From feature "<<idx<<": has incorrect to feature\n";
        okay = false;
      } else if ( fi.begin().geometric_weight() != wgts[idx] ) {
        std::cout << "From feature "<<idx<<": has incorrect geometric weight\n";
        okay = false;
      } else if ( fi.begin().signature_weight() != wgts[idx] ) {
        std::cout << "From feature "<<idx<<": has incorrect signature weight\n";
        okay = false;
      } else if ( fi.begin().cumulative_weight() != wgts[idx] ) {
        std::cout << "From feature "<<idx<<": has incorrect cumulative weight\n";
        okay = false;
      }
    }
  }
  if ( count != from_pts.size() ) {
    std::cout << "Total number of matches in set ("<<count<<") != actual number of matches ("<<from_pts.size()<<")\n";
    okay = false;
  }

  return okay;
}

void
test_one_to_one_correspondence()
{
  std::cout << "Testing one to one correspondence\n";

  std::vector< rgrl_feature_sptr > from_pts;
  std::vector< rgrl_feature_sptr > to_pts;
  std::vector< double > wgts;

  unsigned num_corres = random_count( 100 );
  std::cout << "Creating " << num_corres << " correspondences\n";
  for ( unsigned i=0; i < num_corres; ++i ) {
    from_pts.        push_back( new rgrl_feature_point( random_2d_vector() ) );
    to_pts.          push_back( new rgrl_feature_point( random_2d_vector() ) );
    wgts.            push_back( random_count( 10 ) );
  }

  rgrl_match_set_sptr ms = new rgrl_match_set( rgrl_feature_point::type_id() );
  TEST( "Constructing match set" , ms && ms->from_size(), 0);

  testlib_test_begin( "Adding the features" );
  for ( unsigned i=0; i < from_pts.size(); ++i ) {
    ms->add_feature_and_match( from_pts[i], nullptr, to_pts[i], wgts[i] );
  }
  testlib_test_perform( true );

  TEST( "Iterating over all the data",
        test_one_to_one_contents( ms.as_pointer(), (rgrl_match_set::from_iterator*)nullptr,
                                  from_pts, to_pts, wgts ), true);

  TEST( "Iterating over all the data (const iterator)",
        test_one_to_one_contents( (rgrl_match_set const*)ms.as_pointer(), (rgrl_match_set::const_from_iterator*)nullptr,
                                  from_pts, to_pts, wgts ), true);
}

} // end anonymous namespace

static void test_match_set()
{
  test_one_to_one_correspondence();
}

TESTMAIN(test_match_set);
