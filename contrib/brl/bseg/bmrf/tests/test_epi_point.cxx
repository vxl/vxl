//:
// \file
#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vpl/vpl.h>
#include <vgl/vgl_point_2d.h>

#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_point.h>


//: Test the node class
void test_epi_point()
{
  //Testing constructors
  vgl_point_2d<double> p(0,0);
  bmrf_epi_point_sptr epi_point_1 = new bmrf_epi_point(p, 1, 2, 3, 4, 5);
  bmrf_epi_point_sptr epi_point_2 = new bmrf_epi_point(0, 0, 1, 2, 3, 4, 5);
  TEST("Testing constructors",
       epi_point_1->x()==epi_point_2->x() &&
       epi_point_1->y()==epi_point_2->y() &&
       epi_point_1->alpha()==epi_point_2->alpha() &&
       epi_point_1->grad_mag()==epi_point_2->grad_mag() &&
       epi_point_1->grad_ang()==epi_point_2->grad_ang() &&
       epi_point_1->tan_ang()==epi_point_2->tan_ang(),
       true);


//-----------------------------------------------------------------------
// I/O Tests
//----------------------------------------------------------------------

  // binary test output file stream
  vsl_b_ofstream bep_out("test_epi_point_io.tmp");
  TEST("Created test_epi_point_io.tmp for writing",(!bep_out), false);
  //  epi_point_1->b_write(bep_out);
  vsl_b_write(bep_out, epi_point_1);
  bep_out.close();

  bmrf_epi_point_sptr epi_point_in_1 = new bmrf_epi_point();

  // binary test input file stream
  vsl_b_ifstream bep_in("test_epi_point_io.tmp");
  TEST("Opened test_epi_point_io.tmp for reading",(!bep_in), false);

  //  epi_point_in_1->b_read(bep_in);
  vsl_b_read(bep_in, epi_point_in_1);
  bep_in.close();
  vcl_cout << *epi_point_1 << '\n'
           << *epi_point_in_1 << '\n';
  TEST("Testing io",
       epi_point_1->x()==epi_point_in_1->x() &&
       epi_point_1->y()==epi_point_in_1->y() &&
       epi_point_1->alpha()==epi_point_in_1->alpha() &&
       epi_point_1->grad_mag()==epi_point_in_1->grad_mag() &&
       epi_point_1->grad_ang()==epi_point_in_1->grad_ang() &&
       epi_point_1->tan_ang()==epi_point_in_1->tan_ang(),
       true);
  // remove the temporary file
  vpl_unlink ("test_epi_point_io.tmp");
}


MAIN( test_epi_point )
{
  START( "bmrf_epi_point" );
  test_epi_point();
  SUMMARY();
}
