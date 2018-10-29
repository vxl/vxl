#include <iostream>
#include <algorithm>
#include <cmath>
#include <testlib/testlib_test.h>

#include <vcl_compiler.h>
#include <cassert>
#include <rgrl/rgrl_util.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_random.h>

namespace {

vnl_random mz_random;

// each direction is a row in dirs
vnl_matrix< double >
normal_space( vnl_matrix< double > const& dirs )
{
  vnl_svd<double> dirs_svd( dirs );
  vnl_matrix< double > normal = dirs_svd.nullspace();
  assert ( normal.columns() == dirs.columns() - dirs.rows() );
  return normal;
}

// get the possible region ( reference to rgrl_util.cxx )
void
get_region( vnl_vector< double > const & center,
            std::vector< vnl_vector< double > > const& basis_dirs,
            vnl_vector< double > const& basis_radii,
            vnl_vector< double > & x0,
            vnl_vector< double > & x1 )
{
  unsigned dimension = basis_radii.size();
  std::vector< vnl_vector<double> > corner_points;
  int num_corners = vnl_math::rnd( std::exp( dimension * std::log(2.0) ));
  corner_points.reserve( num_corners );

  //  1b. Since the dimension is computed dynamically, we can't do the
  //  obvious thing of forming for loops, with one loop per dimension.
  //  Instead we build up the corner vectors one dimension at a time.
  //  Each dimension is built by taking the partially-built vectors
  //  from the previous dimensions and adding +/- r * dir to each
  //  vector.  This doubles the number of vectors stored in each
  //  iteration of the outer loop.  This is an iterative version of
  //  what would normally be at least designed as a recursive
  //  procedure.

  vnl_vector<double> zero_vector( dimension, 0.0 );
  corner_points.push_back( zero_vector );

  for ( unsigned int i=0; i<dimension; ++i )
  {
    // 1b(i). For each current / partially built corner vector, create a
    // new corner by adding r * dir, then add -r * dir to the
    // current vector.

    vnl_vector<double> offset = basis_dirs[i] * basis_radii[i];
    unsigned curr_size = corner_points.size();
    for ( unsigned int j=0; j<curr_size; ++j )
    {
      corner_points.push_back( corner_points[j] + offset );
      corner_points[j] -= offset;
    }
  }

  //  1c. Form a bounding box by taking the min and max over the
  //  corners.  At this point, the bounding box is in centered
  //  coordinates, not in absolute coordinates.

  x0.set_size( dimension ); x0.fill( 0.0 );
  x1.set_size( dimension ); x1.fill( 0.0 );

  for (auto & corner_point : corner_points) {
    for ( unsigned int j=0; j<dimension; ++j )  {
      if ( x0[j] > corner_point[j] )
        x0[j] = corner_point[j];
      if ( x1[j] < corner_point[j] )
        x1[j] = corner_point[j];
    }
  }
  x0 += center;
  x1 += center;
}

void
generate_random_data( unsigned dim,
                      vnl_vector< double > & center,
                      std::vector< vnl_vector< double > > & basis_dirs,
                      vnl_vector< double > & basis_radii )
{
  center.set_size( dim );
  for ( unsigned i = 0; i < dim; ++i )
    center[ i ] = mz_random.drand32( 1.0, 100 );

  basis_dirs.resize( 0 );
//    if ( dim == 3 ) {
//       vnl_vector< double > dir( 3, 0.0 );
//       dir[ 0 ] = 1;
//       basis_dirs.push_back( dir );
//       dir[ 0 ] = 0; dir[ 1 ] = 1;
//       basis_dirs.push_back( dir );
//       dir[ 1 ] = 0; dir[ 2 ] = 1;
//       basis_dirs.push_back( dir );
//    }
//    else {
  vnl_vector< double > tangent( dim );
  double norm = 1;
  for ( unsigned i = 0; i < dim - 1; ++i ) {
    tangent[ i ] = mz_random.drand32( -norm, norm );
    norm = std::sqrt( norm * norm - tangent[ i ] * tangent[ i ] );
  }
  tangent[ dim - 1 ] = ( mz_random.drand32( -1, 1 ) < 0 ) ? -std::sqrt( norm ) : std::sqrt( norm );
  basis_dirs.push_back( tangent );

  while ( dim >  basis_dirs.size() ) {
    vnl_matrix< double > dirs( basis_dirs.size(), dim );
    for ( unsigned i = 0; i < basis_dirs.size(); ++i ) {
      dirs.set_row( i, basis_dirs[ i ] );
    }
    vnl_vector< double > normal = normal_space( dirs ).get_column( 0 );
    if ( mz_random.drand32( -1.0, 1.0 ) < 0 )
      normal /= normal[ dim - 1 ];
    for (const auto & basis_dir : basis_dirs)
      assert ( inner_product( normal, basis_dir ) < 10e-3 );
    basis_dirs.push_back( normal );
  }
//    }

  basis_radii.set_size( dim );
  double radii = mz_random.drand32( 0.5, 5 );
  //double radii = 2.0;
  for ( unsigned i = 0; i < dim; ++i ) {
    basis_radii[ i ] = radii;
  }
}

void
test_util_extract_region_locations()
{
  std::cout << " util extract region locations\n";
  {
    std::cout << " 1D case :" << std::endl;
    unsigned dim = 1;
    vnl_vector< double > center;
    std::vector< vnl_vector< double > > basis_dirs;
    vnl_vector< double > basis_radii;
    generate_random_data( dim, center, basis_dirs, basis_radii );

    std::vector< vnl_vector< int > > pixel_locations;

    std::cout << center << '\n' << basis_dirs[0] << '\n' << basis_radii  << std::endl;
    rgrl_util_extract_region_locations( center, basis_dirs, basis_radii, pixel_locations );

    // calculate the correct result
    std::vector< vnl_vector< int > > true_locations;
    vnl_vector< double > x0( dim ), x1( dim );
    get_region( center, basis_dirs, basis_radii, x0, x1 );

    vnl_vector< double > double_pt( dim );
    vnl_vector< int > int_pt( dim ) ;
    for ( int i = (int) std::ceil(x0[ 0 ]); i <= (int)std::floor(x1[ 0 ]); ++i ) {
      double_pt[ 0 ] = i;
      bool good_point = true;
      for ( unsigned n = 0; n < basis_dirs.size(); ++n ) {
        if ( std::abs( inner_product( double_pt - center, basis_dirs[ n ] ) ) > basis_radii[ n ] ) {
          good_point = false;
          break;
        }
      }
      if ( good_point ) {
        int_pt[ 0 ] = i;
        true_locations.push_back( int_pt  );
      }
    }

    std::cout << " number of true pixels: " << true_locations.size() << std::endl;

    bool test_pass = true;
    if ( true_locations.size() != pixel_locations.size() )
      test_pass = false;
    else {
      for (const auto & true_location : true_locations) {
        if ( std::find( pixel_locations.begin(), pixel_locations.end(), true_location ) == pixel_locations.end() ) {
          std::cout << " point " << true_location << " is not extracted" << std::endl;
          test_pass = false;
          break;
        }
      }
    }

    TEST( " extract 1D region result is correct " , test_pass, true );
  }
  {
    std::cout << " 2D case :" << std::endl;
    unsigned dim = 2;
    vnl_vector< double > center;
    std::vector< vnl_vector< double > > basis_dirs;
    vnl_vector< double > basis_radii;
    generate_random_data( dim, center, basis_dirs, basis_radii );

    std::vector< vnl_vector< int > > pixel_locations;
    rgrl_util_extract_region_locations( center, basis_dirs, basis_radii, pixel_locations );

    // calculate the correct result
    std::vector< vnl_vector< int > > true_locations;
    vnl_vector< double > x0( dim ), x1( dim );
    get_region( center, basis_dirs, basis_radii, x0, x1 );

    vnl_vector< double > double_pt( dim );
    vnl_vector< int > int_pt( dim ) ;
    for ( int i = (int) std::ceil( x0[ 0 ] ); i <= (int) std::floor( x1[ 0 ] ); ++i ) {
      double_pt[ 0 ] = i;
      for ( int j = (int) x0[ 1 ]; j <= (int) x1[ 1 ]; ++j ) {
        double_pt[ 1 ] = j;
        bool good_point = true;
        for ( unsigned n = 0; n < basis_dirs.size(); ++n ) {
          if ( std::abs( inner_product( double_pt - center, basis_dirs[ n ] ) ) > basis_radii[ n ] ) {
            good_point = false;
            break;
          }
        }
        if ( good_point ) {
          int_pt[ 0 ] = i; int_pt[ 1 ] = j;
          true_locations.push_back( int_pt );
        }
      }
    }

    std::cout << " number of true pixels: " << true_locations.size() << std::endl;

    bool test_pass = true;
    if ( true_locations.size() != pixel_locations.size() )
      test_pass = false;
    else {
      for (const auto & true_location : true_locations) {
        if ( std::find( pixel_locations.begin(), pixel_locations.end(), true_location ) == pixel_locations.end() ) {
          std::cout << " point " << true_location << " is not extracted" << std::endl;
          test_pass = false;
          break;
        }
      }
    }

    TEST( " extract 2D region result is correct " , test_pass, true );
  }
  {
    std::cout << " 3D case :" << std::endl;
    unsigned dim = 3;
    vnl_vector< double > center;
    std::vector< vnl_vector< double > > basis_dirs;
    vnl_vector< double > basis_radii;
    generate_random_data( dim, center, basis_dirs, basis_radii );

    std::vector< vnl_vector< int > > pixel_locations;
    rgrl_util_extract_region_locations( center, basis_dirs, basis_radii, pixel_locations );

    // calculate the correct result
    std::vector< vnl_vector< int > > true_locations;
    vnl_vector< double > x0( dim ), x1( dim );
    get_region( center, basis_dirs, basis_radii, x0, x1 );

    vnl_vector< double > double_pt( dim );
    vnl_vector< int > int_pt( dim ) ;
    for ( int i = (int) std::ceil( x0[ 0 ] ); i <= (int) std::floor( x1[ 0 ] ); ++i ) {
      double_pt[ 0 ] = i;
      for ( int j = (int) x0[ 1 ]; j <= (int) x1[ 1 ]; ++j ) {
        double_pt[ 1 ] = j;
        for ( int k = (int) x0[ 2 ]; k <= (int) x1[ 2 ]; ++k ) {
          double_pt[ 2 ] = k;
          bool good_point = true;
          for ( unsigned n = 0; n < basis_dirs.size(); ++n ) {
            if ( std::abs( inner_product( double_pt - center, basis_dirs[ n ] ) ) > basis_radii[ n ] ) {
              good_point = false;
              break;
            }
          }
          if ( good_point ) {
            int_pt[ 0 ] = i; int_pt[ 1 ] = j; int_pt[ 2 ] = k;
            true_locations.push_back( int_pt );
          }
        }
      }
    }

    std::cout << " number of true pixels: " << true_locations.size() << std::endl;

    bool test_pass = true;
    if ( true_locations.size() != pixel_locations.size() )
      test_pass = false;
    else {
      for (const auto & true_location : true_locations) {
        if ( std::find( pixel_locations.begin(), pixel_locations.end(), true_location ) == pixel_locations.end() ) {
          std::cout << " point " << true_location << " is not extracted" << std::endl;
          test_pass = false;
          break;
        }
      }
    }

    TEST( " extract 3D region result is correct " , test_pass, true );
  }
}

} // end anonymous namespace

static void test_rgrl_util()
{
  test_util_extract_region_locations();
}

TESTMAIN(test_rgrl_util);
