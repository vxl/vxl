//:
// \file
#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vpl/vpl.h>
#include <vgl/vgl_point_2d.h>

#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_point.h>
#include <bmrf/bmrf_epi_seg_sptr.h>
#include <bmrf/bmrf_epi_seg.h>


//: Test the node class
void test_epi_seg()
{
  //Testing constructors
  vgl_point_2d<double> p0(0,0), p1(1,1);
  bmrf_epi_point_sptr ep0 = new bmrf_epi_point(p0, 10, 20, 30, 40, 50);
  bmrf_epi_point_sptr ep1 = new bmrf_epi_point(p1, 11, 21, 31, 41, 51);

  bmrf_epi_seg_sptr eps = new bmrf_epi_seg();
  eps->add_point(ep0);
  eps->add_point(ep1);
  eps->add_int_sample(10.25, 0.1, 100, 0.2, 200);
  eps->add_int_sample(10.75, 0.15, 150, 0.25, 250);
  vcl_cout << "epi_seg\n" << *eps << '\n';
  TEST("Testing constructors",
       eps->min_alpha()==10      &&
       eps->max_alpha()==11      &&
       eps->min_s()==20          &&
       eps->max_s()==21          &&
       eps->min_tan_ang()==50    &&
       eps->max_tan_ang()==51    &&
       eps->min_left_int()==100  &&
       eps->max_left_int()==150  &&
       eps->min_right_int()==200 &&
       eps->max_right_int()==250,
       true);


//-----------------------------------------------------------------------
// I/O Tests
//----------------------------------------------------------------------

  // binary test output file stream
  vsl_b_ofstream os("test_epi_seg_io.tmp");
  TEST("Created test_epi_seg_io.tmp for writing",(!os), false);
  eps->b_write(os);
  //vsl_b_write(os, eps);
  os.close();

  bmrf_epi_seg_sptr eps_in = new bmrf_epi_seg();

  // binary test input file stream
  vsl_b_ifstream is("test_epi_seg_io.tmp");
  TEST("Opened test_epi_seg_io.tmp for reading",(!is), false);
  //  vsl_b_read(is, eps_in);
      eps_in->b_read(is);
  is.close();
  vcl_cout << *eps_in << '\n';
  TEST("Testing I/0",
       eps->min_alpha()==eps_in->min_alpha()        &&
       eps->max_alpha()==eps_in->max_alpha()        &&
       eps->min_s()==eps_in->min_s()                &&
       eps->max_s()==eps_in->max_s()                &&
       eps->min_tan_ang()==eps_in->min_tan_ang()    &&
       eps->max_tan_ang()==eps_in->max_tan_ang()    &&
       eps->min_left_int()==eps_in->min_left_int()  &&
       eps->max_left_int()==eps_in->max_left_int()  &&
       eps->min_right_int()==eps_in->min_right_int()&&
       eps->max_right_int()==eps_in->max_right_int(),
       true);

  // remove the temporary file
  vpl_unlink ("test_epi_seg_io.tmp");
}


MAIN( test_epi_seg )
{
  START( "bmrf_epi_seg" );
  test_epi_seg();
  SUMMARY();
}
