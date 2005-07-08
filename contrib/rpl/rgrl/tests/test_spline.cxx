#include <testlib/testlib_test.h>

#include <rgrl/rgrl_spline.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vnl/vnl_random.h>

namespace {

vnl_random random;

double
spline_1d_value(double u, vnl_vector<double> c)
{
  if (u < 0 || u+3 > c.size())
    return 0.0;
  unsigned int f = (unsigned int)vcl_floor( u );

  u -= f; // u is now between 0 and 1
  if (u==0)  // this avoids access to nonexisting c[f+3] when u+3 == c.size()
    return (  c[ f ] + 4 * c[ f + 1 ] + c[ f + 2 ] ) / 6;
  else
    return (  (1-u)*(1-u)*(1-u) * c[ f ]
            + (3*u*u*u-6*u*u+4) * c[ f + 1 ]
            + (-3*u*u*u+3*u*u+3*u+1) * c[ f + 2 ]
            + u*u*u * c[ f + 3 ]
           ) / 6;
}

double
spline_2d_value( vnl_vector<double> pt, vnl_vector<double> c, vnl_vector< unsigned > m )
{
  assert (m.size() == 2);
  assert (pt.size() == 2);
  vnl_vector< unsigned > f( m.size() );
  for (unsigned i = 0; i < m.size(); ++i ) {
    if ( pt[ i ] < 0 || pt[ i ] > m[ i ] )
      return 0;
    f[ i ] = (unsigned int)vcl_floor( pt[ i ] );
  }
  double u = pt[ 0 ] - f[ 0 ]; // a number between 0 and 1
  double v = pt[ 1 ] - f[ 1 ]; // a number between 0 and 1
  unsigned ii = f[1] * ( m[0] + 3 ) + f[0];

  vnl_vector< double > control_v( 4 );
  vnl_vector< double > control_u( 4 );
  for (unsigned j=0; j<4; ++j ) {
    for (unsigned k=0; k<4; ++k )
      if (ii+k < c.size())
        control_u[ k ] = c[ ii+k ];
    control_v[ j ] = spline_1d_value( u, control_u );
    ii += m[0]+3;
  }

  return spline_1d_value( v, control_v );
}

double
spline_3d_value(vnl_vector<double> pt, vnl_vector<double> c, vnl_vector< unsigned > m )
{
  assert (m.size() == 3);
  assert (pt.size() == 3);
  vnl_vector< unsigned > f( m.size() );
  for (unsigned i = 0; i < m.size(); ++i ) {
    if ( pt[ i ] < 0 || pt[ i ] > m[ i ] )
      return 0;
    f[ i ] = (unsigned int)vcl_floor( pt[ i ] );
  }
  double u = pt[ 0 ] - f[ 0 ]; // a number between 0 and 1
  double v = pt[ 1 ] - f[ 1 ]; // a number between 0 and 1
  double w = pt[ 2 ] - f[ 2 ]; // a number between 0 and 1
  unsigned ii = ( f[2] * ( m[1]+3 ) + f[1] ) * ( m[0] + 3 ) + f[0];

  vnl_vector< double > control_w( 4 );
  vnl_vector< double > control_v( 4 );
  vnl_vector< double > control_u( 4 );
  for (unsigned l=0; l<4; ++l ) {
    unsigned j = ii;
    for (unsigned k=0; k<4; ++k ) {
      for (unsigned n=0; n<4; ++n )
        if (j+n < c.size())
          control_u[ n ] = c[ j+n ];
      control_v[ k ] = spline_1d_value( u, control_u );
      j += m[0]+3;
    }
    control_w[ l ] = spline_1d_value( v, control_v );
    ii += (m[0]+3)*(m[1]+3);
  }

  return spline_1d_value( w, control_w );
}

void
test_1d_spline()
{
  vcl_cout << "estimate 1-D spline" << vcl_endl;
  double c_data[] = { 1.0, 2.0, -2.0, -1.0, 2.0 };
  vnl_vector<unsigned> m(1);  m[0] = 2;
  vnl_vector<double> c(c_data,5); // c.size() must equal m[0]+3
  rgrl_spline spline_1d(m, c);

  // test boundary and other points
  for (double u=0; u<=2.0; u+=0.5 ) {
    vnl_vector<double> pt(1,u);
    double return_spline_value = spline_1d.f_x( pt );
    vcl_cout << "\nreturn spline value  f(" << pt << ")=" << return_spline_value ;
    double true_spline_value = spline_1d_value( pt[0], c );
    vcl_cout << "    true spline value  f(" << pt << ")=" << true_spline_value << vcl_endl ;
    TEST_NEAR_REL(" test spline value: ", return_spline_value, true_spline_value, 1e-9);
  }
  // random point
  vnl_vector<double> pt(1);
  pt[0] = random.drand32(0,2);
  double return_spline_value = spline_1d.f_x( pt );
  vcl_cout << "\nreturn spline value  f(" << pt[0] << ")=" << return_spline_value;
  double true_spline_value = spline_1d_value( pt[0], c );
  vcl_cout << "    true spline value  f(" << pt[0] << ")=" << true_spline_value << vcl_endl;
  TEST_NEAR_REL(" test random spline value: ", return_spline_value, true_spline_value, 1e-9);
}

void
test_2d_spline()
{
  vcl_cout << "\nestimate 2-D spline" << vcl_endl;
  vnl_vector<unsigned> m(2);  m[0] = m[1] = 2;
  vnl_vector<double> c(25); // c.size() must equal (m[0]+3)*(m[1]+3)
  c[ 0] = 1; c[ 1] = 2; c[ 3]= -2; c[ 3]= -1; c[ 4] = 2;
  c[ 5] = 2; c[ 6]= -1; c[ 7] = 0; c[ 8] = 2; c[ 9] = 0;
  c[10]= -2; c[11] = 1; c[12] = 1; c[13] = 1; c[14] = 1;
  c[15] = 3; c[16] = 3; c[17] = 1; c[18] = 0; c[19]= -1;
  c[20] = 0; c[21] = 2; c[22]= -1; c[23]= -2; c[24]= -2;
  rgrl_spline spline_2d(m, c);

  // test boundary and other points
  for (double u=0; u<=2.0; u+=0.5 ) {
    for (double v=0; v<=2.0; v+= 0.5 ) {
      vnl_vector<double> pt(2); pt[0] = u; pt[1] = v;
      vcl_cout << pt << vcl_endl;
      double return_spline_value = spline_2d.f_x( pt );
      vcl_cout << "\nreturn spline value  f(" << pt<< ")=" << return_spline_value;
      double true_spline_value = spline_2d_value( pt, c, m );
      vcl_cout << "    true spline value  f(" << pt << ")=" << true_spline_value << vcl_endl;
      TEST_NEAR_REL("test spline value: ", return_spline_value, true_spline_value, 1e-9);
    }
  }
  // test random point
  vnl_vector<double> pt(2);
  pt[0] = random.drand32(0,2);
  pt[1] = random.drand32(0,2);
  vcl_cout << pt << vcl_endl;
  double return_spline_value = spline_2d.f_x( pt );
  vcl_cout << "\nreturn spline value  f(" << pt<< ")=" << return_spline_value;
  double true_spline_value = spline_2d_value( pt, c, m );
  vcl_cout << "    true spline value  f(" << pt << ")=" << true_spline_value << vcl_endl;

  TEST_NEAR_REL("test random spline value: ", return_spline_value, true_spline_value, 1e-9);
}

void
test_3d_spline()
{
  vcl_cout << "\nestimate 3-D spline" << vcl_endl;
  vnl_vector< unsigned > m(3);  m[0] = m[1] = 2;  m[2] = 1;
  vnl_vector<double> c(100); // c.size() must equal (m[0]+3)*(m[1]+3)*(m[2]+3)
  c[ 0] = 1; c[ 1] = 2; c[ 3]= -2; c[ 3]= -1; c[ 4] = 2;
  c[ 5] = 2; c[ 6]= -1; c[ 7] = 0; c[ 8] = 2; c[ 9] = 0;
  c[10]= -2; c[11] = 1; c[12] = 1; c[13] = 1; c[14] = 1;
  c[15] = 3; c[16] = 3; c[17] = 1; c[18] = 0; c[19]= -1;
  c[20] = 0; c[21] = 2; c[22]= -1; c[23]= -2; c[24]= -2;

  c[25] = 2; c[26] = 1; c[28]= -2; c[28]= -1; c[29] = 2;
  c[30]= -2; c[31]= -2; c[32] = 0; c[33] = 2; c[34] = 0;
  c[35]= -3; c[36]= -1; c[37] = 1; c[38] = 1; c[39] = 1;
  c[40] = 3; c[41]= -1; c[42] = 1; c[43] = 0; c[44]= -1;
  c[45]= -1; c[46] = 2; c[47]= -1; c[48]= -2; c[49]= -2;

  c[50] = 1; c[51] = 2; c[53] = 2; c[53] = 1; c[54] = 2;
  c[55] = 2; c[56]= -1; c[57] = 0; c[58] = 2; c[59] = 0;
  c[60]= -2; c[61]= -1; c[62] = 0; c[63] = 1; c[64] = 2;
  c[65] = 3; c[66] = 3; c[67] = 1; c[68] = 0; c[69]= -3;
  c[70]= -1; c[71] = 2; c[72]= -1; c[73]= -2; c[74]= -4;

  c[75] = 1; c[76] = 3; c[78]= -3; c[78]= -1; c[79] = 2;
  c[80] = 2; c[81]= -1; c[82] = 1; c[83] = 2; c[84] = 0;
  c[85]= -2; c[86] = 1; c[87] = 1; c[88] = 1; c[89] = 1;
  c[90] = 2; c[91]= -1; c[92] = 1; c[93] = 0; c[94]= -1;
  c[95]= -1; c[96] = 2; c[97]= -1; c[98]= -2; c[99]= -2;

  rgrl_spline spline_3d(m, c);

  // test boundary and other points
  for (double u=0; u<=2.0; u+=0.5 ) {
    for (double v=0; v<=2.0; v+= 0.5 ) {
      for (double w=0; w<=1.0; w+= 0.25 ) {
        vnl_vector<double> pt(3); pt[0] = u; pt[1] = v; pt[2] = w;
        vcl_cout << pt << vcl_endl;
        double return_spline_value = spline_3d.f_x( pt );
        vcl_cout << "\nreturn spline value  f(" << pt<< ")=" << return_spline_value;
        double true_spline_value = spline_3d_value( pt, c, m );
        vcl_cout << "    true spline value  f(" << pt << ")=" << true_spline_value << vcl_endl;
        TEST_NEAR_REL("test spline value: ", return_spline_value, true_spline_value, 1e-9);
      }
    }
  }
  // test random point
  vnl_vector<double> pt(3);
  pt[0] = random.drand32( 0, 2 );
  pt[1] = random.drand32( 0, 2 );
  pt[2] = random.drand32( 0, 1 );
  vcl_cout << pt << vcl_endl;
  double return_spline_value = spline_3d.f_x( pt );
  vcl_cout << "\nreturn spline value  f(" << pt<< ")=" << return_spline_value << vcl_endl;
  double true_spline_value = spline_3d_value( pt, c, m );
  vcl_cout << "\ntrue spline value  f(" << pt << ")=" << true_spline_value << vcl_endl;

  TEST_NEAR_REL(" test spline value: ", return_spline_value, true_spline_value, 1e-9);
}

void
test_refine_1d_spline()
{
  vcl_cout << "\nTest 1d refinement\n";

  vnl_vector< unsigned > m(1);      m[0] = 5;
  rgrl_spline spline( m );
  TEST("number of control points is 8", spline.num_of_control_points(), m[0]+3);
  vnl_vector< double > c( spline.num_of_control_points() );

  for (unsigned i=0; i<spline.num_of_control_points(); ++i )
    c[ i ] = random.drand32(0,2);

  spline.set_control_points( c );

  vnl_vector< unsigned > new_m(1);  new_m[0] = 9;
  rgrl_spline_sptr refine_spline = spline.refinement( new_m );
  TEST("new number of control points is 12", refine_spline->num_of_control_points(), new_m[0]+3);
#ifdef DEBUG
  vcl_cout << "the original control points: " << c << vcl_endl
           << "the refined control points: " << refine_spline->get_control_points() << vcl_endl;
#endif
  for (unsigned i=0; i < 100; ++i ) {
    vnl_vector< double > p( 1 );
    p[0] = random.drand32( 0, new_m[0]*0.5 );
    vcl_cout << "f(" <<p[0] << ")= " << spline.f_x( p ) << " => " << refine_spline->f_x( p*2.0 ) << '\n';
    TEST_NEAR_REL( " test refined spline value: ", refine_spline->f_x( p*2.0 ), spline.f_x( p ), 1e-9);
  }
}

void
test_refine_2d_spline()
{
  vcl_cout << "\nTest 2d refinement\n";

  vnl_vector< unsigned > m(2);  m[0] = m[1] = 3;
  rgrl_spline spline( m );
  TEST("number of control points is 36", spline.num_of_control_points(), (m[0]+3)*(m[1]+3));
  vnl_vector< double > c( spline.num_of_control_points() );

  for (unsigned i=0; i<c.size(); ++i )
    c[ i ] = random.drand32(0,5);

  spline.set_control_points( c );

  vnl_vector< unsigned > new_m(2);  new_m[0] = new_m[1] = 5;
  rgrl_spline_sptr refine_spline = spline.refinement( new_m );
  TEST("new number of control points is 64", refine_spline->num_of_control_points(), (new_m[0]+3)*(new_m[1]+3));
#ifdef DEBUG
  vcl_cout << "the original control points: " << c << vcl_endl
           << "the refined control points: " << refine_spline->get_control_points() << vcl_endl;
#endif
  for (unsigned i=0; i < 10; ++i ) {
    vnl_vector< double > p( 2 );
    p[ 0 ] = random.drand32( 0, new_m[0]*0.5 );
    for (unsigned j=0; j < 10; ++j ) {
      p[ 1 ] = random.drand32( 0, new_m[1]*0.5 );
      vcl_cout << "f(" <<p << ")= " << spline.f_x( p ) << " => " << refine_spline->f_x( p*2 ) << '\n';
      TEST_NEAR_REL( " test refined spline value: ", refine_spline->f_x( p*2.0 ), spline.f_x( p ), 1e-9);
    }
  }
}

void
test_refine_3d_spline()
{
  vcl_cout << "\nTest 3d refinement\n";

  vnl_vector< unsigned > m(3);  m[0] = m[1] = m[2] = 3;
  rgrl_spline spline( m );
  TEST("number of control points is 216", spline.num_of_control_points(), (m[0]+3)*(m[1]+3)*(m[2]+3));
  vnl_vector< double > c( spline.num_of_control_points() );

  for (unsigned i=0; i<spline.num_of_control_points(); ++i )
    c[ i ] = random.drand32(0,10);

  spline.set_control_points( c );

  vnl_vector< unsigned > new_m(3);  new_m[0] = new_m[1] = new_m[2] = 5;
  rgrl_spline_sptr refine_spline = spline.refinement( new_m );
  TEST("new number of control points is 512", refine_spline->num_of_control_points(), (new_m[0]+3)*(new_m[1]+3)*(new_m[2]+3));
#ifdef DEBUG
  vcl_cout << "the original control points: " << c << vcl_endl
           << "the refined control points: " << refine_spline->get_control_points() << vcl_endl;
#endif
  for (unsigned i=0; i < 10; ++i ) {
    vnl_vector< double > p( 3 );
    p[ 0 ] = random.drand32( 0, new_m[0]*0.5 );
    for (unsigned j=0; j < 10; ++j ) {
      p[ 1 ] = random.drand32( 0, new_m[1]*0.5 );
      for (unsigned k=0; k < 10; ++k ) {
        p[ 2 ] = random.drand32( 0, new_m[2]*0.5 );
        TEST_NEAR_REL( " test refined spline value: ", refine_spline->f_x( p*2.0 ), spline.f_x( p ), 1e-9);
      }
    }
  }
}

} // end anonymous namespace

MAIN( test_spline )
{
  START(" spline objects " );
  test_1d_spline();
  test_2d_spline();
  test_3d_spline();
  test_refine_1d_spline();
  test_refine_2d_spline();
  test_refine_3d_spline();
  SUMMARY();
}
