#include <testlib/testlib_test.h>
#include <rgrl/rgrl_spline.h>
#include <vcl_iostream.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>

namespace {

vnl_random random;

double
spline_1d_value(double u, vnl_vector<double> c)
{
  
  int floor = (int)vcl_floor( u );
  int ceil = (int)vcl_ceil( u );
  u = u - floor;

  if ( floor < 0 || ceil+3 > c.size() )
    return 0;

  return (1-u)*(1-u)*(1-u)/6 * c[ floor ]
    + (3*u*u*u-6*u*u+4)/6 * c[ floor + 1 ]
    + (-3*u*u*u+3*u*u+3*u+1)/6 * c[ floor + 2 ]
    + u*u*u/6 * c[ floor + 3 ];
}

double
spline_2d_value( vnl_vector<double> pt, vnl_vector<double> c, vnl_vector< unsigned > m )
{
  vnl_vector< int > floor( m.size() );
  vnl_vector< int > ceil( m.size() );
  for( unsigned i = 0; i < m.size(); ++i ) {
    floor[ i ] = (int)vcl_floor( pt[ i ] );
    ceil[ i ] = (int)vcl_ceil( pt[ i ] );
    if ( floor[ i ] < 0 || ceil[ i ] > m[ i ] )
      return 0;
  }
  double u = pt[ 0 ] - floor[ 0 ];
  double v = pt[ 1 ] - floor[ 1 ];
  unsigned i = floor[1] * ( m[0] + 3 ) + floor[0];
  
  vnl_vector< double > control_v( 4 );
  vnl_vector< double > control_u( 4 );
  for( unsigned j=0; j<4; ++j ) {
    for( unsigned k=0; k<4; ++k ) {
      control_u[ k ] = c[ i+k ];
    }
    control_v[ j ] = spline_1d_value( u, control_u );
    i += m[0]+3;
  }

  return spline_1d_value( v, control_v );
}

double
spline_3d_value(vnl_vector<double> pt, vnl_vector<double> c, vnl_vector< unsigned > m )
{
  vnl_vector< int > floor( m.size() );
  vnl_vector< int > ceil( m.size() );
  for( unsigned i = 0; i < m.size(); ++i ) {
    floor[ i ] = (int)vcl_floor( pt[ i ] );
    ceil[ i ] = (int)vcl_ceil( pt[ i ] );
    if ( floor[ i ] < 0 || ceil[ i ] > m[ i ] )
      return 0;
  }
  double u = pt[ 0 ] - floor[ 0 ];
  double v = pt[ 1 ] - floor[ 1 ];
  double w = pt[ 2 ] - floor[ 2 ];
  unsigned i = floor[2] * ( m[0]+3 ) * ( m[1]+3 ) + floor[1] * ( m[0] + 3 ) + floor[0];
  unsigned j = floor[2] * ( m[0]+3 ) * ( m[1]+3 ) + floor[1] * ( m[0] + 3 ) + floor[0];
  
  vnl_vector< double > control_w( 4 );
  vnl_vector< double > control_v( 4 );
  vnl_vector< double > control_u( 4 );
  for( unsigned l=0; l<4; ++l ) {
    for( unsigned k=0; k<4; ++k ) {
      for( unsigned n=0; n<4; ++n ) {
        control_u[ n ] = c[ j+n ];
      }
      control_v[ k ] = spline_1d_value( u, control_u );
      j += m[0]+3;
    }
    control_w[ l ] = spline_1d_value( v, control_v );
    i += (m[0]+3)*(m[1]+3);
    j = i;
  }

  return spline_1d_value( w, control_w );
}

void
test_spline()
{
  // test 1D
  {
    vcl_cout << "estimate 1-D spline" << vcl_endl;
    vnl_vector<double> c(5, 1);

    vnl_vector< unsigned > m( 1, 2 );
    c[0] = 1;
    c[1] = 2;
    c[2] = -2;
    c[3] = -1;
    c[4] = 2;
    rgrl_spline spline_1d( m, c );
    
    // boundary points
    {
      vnl_vector<double> pt(1);
      for( double u=0; u<=2.0; u+=0.5 ) {
        pt[ 0 ] = u;
        double return_spline_value = spline_1d.f_x( pt );
        vcl_cout << "\nreturn spline value  f(" << pt << ")=" << return_spline_value ;
        double true_spline_value = spline_1d_value( pt[0], c );
        vcl_cout << "    true spline value  f(" << pt << ")=" << true_spline_value << vcl_endl ;      
        TEST( " test spline value: ", vnl_math_abs(return_spline_value-true_spline_value)<1e-10, true);
      }
    }
    // random point
    {
      vnl_vector<double> pt(1);
      pt[0] = random.drand32(0,2);
      double return_spline_value = spline_1d.f_x( pt );
      vcl_cout << "\nreturn spline value  f(" << pt[0] << ")=" << return_spline_value;
      double true_spline_value = spline_1d_value( pt[0], c );
      vcl_cout << "    true spline value  f(" << pt[0] << ")=" << true_spline_value << vcl_endl;
      
      TEST( " test random spline value: ", vnl_math_abs(return_spline_value - true_spline_value)<1e-10, true);
    }
  }
  // test 2-D
  {
    vcl_cout << "\nestimate 2-D spline" << vcl_endl;
    vnl_vector< unsigned > m(2, 2);
    vnl_vector<double> c(25, 1);
    c[0] = 1; c[1] = 2; c[3] = -2; c[3] = -1; c[4] = 2;
    c[5] = 2; c[6] = -1; c[7] = 0; c[8] = 2; c[9] = 0;
    c[10] = -2; c[11] = 1; c[12] = 1; c[13] = 1; c[14] = 1;
    c[15] = 3; c[16] = 3; c[17] = 1; c[18] = 0; c[19] = -1;
    c[20] = 0; c[21] = 2; c[22] = -1; c[23] = -2; c[24] = -2;
    rgrl_spline spline_2d( m, c );


    // test boundary and other points
    {
      for( double u=0; u<=2.0; u+=0.5 ) {
        for( double v=0; v<=2.0; v+= 0.5 ) {          
          vnl_vector<double> pt(2);
          pt[0] = u;
          pt[1] = v;
          vcl_cout << pt << vcl_endl;
          double return_spline_value = spline_2d.f_x( pt );
          vcl_cout << "\nreturn spline value  f(" << pt<< ")=" << return_spline_value;
          double true_spline_value = spline_2d_value( pt, c, m );
          vcl_cout << "    true spline value  f(" << pt << ")=" << true_spline_value << vcl_endl;
          TEST( "test spline value: ", vnl_math_abs(return_spline_value-true_spline_value)<1e-6, true);
        }
      }
    }
    // test random point
    {
      vnl_vector<double> pt(2);
      pt[0] = random.drand32(0,2);
      pt[1] = random.drand32(0,2);
      vcl_cout << pt << vcl_endl;
      double return_spline_value = spline_2d.f_x( pt );
      vcl_cout << "\nreturn spline value  f(" << pt<< ")=" << return_spline_value;
      double true_spline_value = spline_2d_value( pt, c, m );
      vcl_cout << "    true spline value  f(" << pt << ")=" << true_spline_value << vcl_endl;
      
      TEST( "test random spline value: ", vnl_math_abs(return_spline_value-true_spline_value)<1e-6, true);
    }
  }
  // test 3-D
  {
    vcl_cout << "\nestimate 3-D spline" << vcl_endl;
    vnl_vector< unsigned > m(3, 2);    m[2] = 1;
    vnl_vector<double> c(100, 1);
    c[0] = 1; c[1] = 2; c[3] = -2; c[3] = -1; c[4] = 2;
    c[5] = 2; c[6] = -1; c[7] = 0; c[8] = 2; c[9] = 0;
    c[10] = -2; c[11] = 1; c[12] = 1; c[13] = 1; c[14] = 1;
    c[15] = 3; c[16] = 3; c[17] = 1; c[18] = 0; c[19] = -1;
    c[20] = 0; c[21] = 2; c[22] = -1; c[23] = -2; c[24] = -2;

    c[0] = 2; c[1] = 1; c[3] = -2; c[3] = -1; c[4] = 2;
    c[5] = -2; c[6] = -2; c[7] = 0; c[8] = 2; c[9] = 0;
    c[10] = -3; c[11] = -1; c[12] = 1; c[13] = 1; c[14] = 1;
    c[15] = 3; c[16] = -1; c[17] = 1; c[18] = 0; c[19] = -1;
    c[20] = -1; c[21] = 2; c[22] = -1; c[23] = -2; c[24] = -2;

    c[0] = 1; c[1] = 2; c[3] = 2; c[3] = 1; c[4] = 2;
    c[5] = 2; c[6] = -1; c[7] = 0; c[8] = 2; c[9] = 0;
    c[10] = -2; c[11] = -1; c[12] = 0; c[13] = 1; c[14] = 2;
    c[15] = 3; c[16] = 3; c[17] = 1; c[18] = 0; c[19] = -3;
    c[20] = -1; c[21] = 2; c[22] = -1; c[23] = -2; c[24] = -4;

    c[0] = 1; c[1] = 3; c[3] = -3; c[3] = -1; c[4] = 2;
    c[5] = 2; c[6] = -1; c[7] = 1; c[8] = 2; c[9] = 0;
    c[10] = -2; c[11] = 1; c[12] = 1; c[13] = 1; c[14] = 1;
    c[15] = 2; c[16] = -1; c[17] = 1; c[18] = 0; c[19] = -1;
    c[20] = -1; c[21] = 2; c[22] = -1; c[23] = -2; c[24] = -2;

    rgrl_spline spline_3d( m, c );
    
    vnl_vector<double> pt(3);
    pt[0] = random.drand32( 0, 2 );
    pt[1] = random.drand32( 0, 2 );
    pt[2] = random.drand32( 0, 1 );
    vcl_cout << pt << vcl_endl;
    double return_spline_value = spline_3d.f_x( pt );
    vcl_cout << "\nreturn spline value  f(" << pt<< ")=" << return_spline_value << vcl_endl;
    double true_spline_value = spline_3d_value( pt, c, m );
    vcl_cout << "\ntrue spline value  f(" << pt << ")=" << true_spline_value << vcl_endl;
    
    TEST( " test spline value: ", vnl_math_abs(return_spline_value-true_spline_value)<1e-6, true);
  }

}

void
test_refine_spline()
{ 
  {
    vcl_cout << "\nTest 1d refinement\n";

    vnl_vector< unsigned > m( 1, 5 );
    vnl_vector< unsigned > new_m( 1, 9 );
    rgrl_spline spline( m );
    vnl_vector< double > c( spline.num_of_control_points() );

    for( unsigned i=0; i<spline.num_of_control_points(); ++i ) 
      c[ i ] = random.drand32(0,2);

    spline.set_control_points( c );

    rgrl_spline_sptr refine_spline = spline.refinement( new_m );
//      vcl_cout << "the original control points: " << c << vcl_endl;
//      vcl_cout << "the refined control points: " << refine_spline.get_control_points() << vcl_endl;
    vnl_vector< double > p( 1 );
    for( unsigned i=0; i < 3; ++i ) {
      vcl_cout << "f(" <<p << ")= " << spline.f_x( p ) << " => " << refine_spline->f_x( p*2 ) << "\n";
      p[ 0 ] = random.drand32( 0, ((double)new_m[0])/2 );
      TEST( " test refined spline value: ", vnl_math_abs( spline.f_x( p ) - refine_spline->f_x( p*2 ) )<1e-6, true);
    }
  }
  {
    vcl_cout << "\nTest 2d refinement\n";

    vnl_vector< unsigned > m( 2, 3 );
    vnl_vector< unsigned > new_m( 2, 5 );
    rgrl_spline spline( m );
    vnl_vector< double > c( spline.num_of_control_points() );

    for( unsigned i=0; i<c.size(); ++i ) 
      c[i] = 1;
    //c[ i ] = random.drand32(0,5);

    vcl_cout << "original c " << c << "\n";
    spline.set_control_points( c );

    rgrl_spline_sptr refine_spline = spline.refinement( new_m );
    vcl_cout << vcl_endl;
    vnl_vector< double > p( 2 );
    for( unsigned i=0; i < 2; ++i ) {
      p[ 0 ] = random.drand32( 0, ((double)new_m[0])/2 );
      for( unsigned j=0; j < 2; ++j ) {
        p[ 1 ] = random.drand32( 0, ((double)new_m[1])/2 );
        vcl_cout << "f(" <<p << ")= " << spline.f_x( p ) << " => " << refine_spline->f_x( p*2 ) << "\n";
        TEST( " test refined spline value: ", vnl_math_abs( spline.f_x( p )-refine_spline->f_x( p*2 ) )<1e-6, true);
      }
    }
  }
  {
    vcl_cout << "\nTest 3d refinement\n";

    vnl_vector< unsigned > m( 3, 3 );
    vnl_vector< unsigned > new_m( 3, 5 );
    rgrl_spline spline( m );
    vnl_vector< double > c( spline.num_of_control_points() );

    for( unsigned i=0; i<spline.num_of_control_points(); ++i ) 
      c[ i ] = random.drand32(0,10);

    spline.set_control_points( c );

    rgrl_spline_sptr refine_spline = spline.refinement( new_m );
//      vcl_cout << "the original control points: " << c << vcl_endl;
//      vcl_cout << "the refined control points: " << refine_spline.get_control_points() << vcl_endl;
    vnl_vector< double > p( 3 );
    for( unsigned i=0; i < 2; ++i ) {
      p[ 0 ] = random.drand32( 0, ((double)new_m[0])/2 );
      for( unsigned j=0; j < 2; ++j ) {
        p[ 1 ] = random.drand32( 0, ((double)new_m[1])/2 );
        for( unsigned k=0; k < 2; ++k ) {
          p[ 2 ] = random.drand32( 0, ((double)new_m[2])/2 );
          TEST( " test refined spline value: ", vnl_math_abs( spline.f_x( p ) - refine_spline->f_x( p*2 ) )<1e-6, true);
        }
      }
    }
  }
}

}

MAIN( test_spline )
{
  START(" spline objects " );
  test_spline();
  test_refine_spline();
  SUMMARY();
}
