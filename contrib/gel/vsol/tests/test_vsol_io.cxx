//*****************************************************************************
// File name: test_io.cxx
// Description: Test binary I/O
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2004/05/08| Joseph Mundy             |Creation
//*****************************************************************************
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <testlib/testlib_test.h>
//:
// \file

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

void test_vsol_io()
{
  //vsol_point_2d I/O
  vcl_cout << "Testing I/O for vsol_point_2d\n";

  vsol_point_2d_sptr p=new vsol_point_2d(1.1,2.2);
  vsol_point_2d_sptr pa=new vsol_point_2d(3.3,4.4);
  vcl_cout << "testing on points" << *p << *pa <<"\n";

  vsl_b_ofstream bp_out("test_point_2d_io.tmp");
  TEST("Created test_point_2d_io.tmp for writing",(!bp_out), false);

  vsl_b_write(bp_out, p);
  pa->b_write(bp_out);
  bp_out.close();

  // binary test input file stream
  vsl_b_ifstream bp_in("test_point_2d_io.tmp");
  TEST("Opened test_point_2d_io.tmp for reading",(!bp_in), false);

  vsol_point_2d_sptr p_in, p_ina = new vsol_point_2d(0, 0);

  vsl_b_read(bp_in, p_in);
  p_ina->b_read(bp_in);
  bp_in.close();

  vcl_cout << "Recovered point" << *p_in << '\n';
  vcl_cout << "Recovered point" << *p_ina << '\n';

  TEST("Testing point io", 
       p_in && p_ina &&
       p->x()==p_in->x() && p->y()==p_in->y() &&
       pa->x()==p_ina->x() && pa->y()==p_ina->y(),
       true);

  // remove the temporary file
  vpl_unlink ("test_point_2d_io.tmp");


  //vsol_line_2d I/O
  vcl_cout << "\nTesting I/O for vsol_line_2d\n";

  vsol_point_2d_sptr p0 = new vsol_point_2d(1.1,2.2);
  vsol_point_2d_sptr p1 = new vsol_point_2d(3.3,4.4);

  vsol_line_2d_sptr l = new vsol_line_2d(p0, p1);
  vsol_line_2d_sptr la = new vsol_line_2d(p1, p0);
  vcl_cout << "testing on lines" << *l << ' ' << *la << '\n';

  vsl_b_ofstream bl_out("test_line_2d_io.tmp");

  TEST("Created test_line_2d_io.tmp for writing",(!bl_out), false);

  l->b_write(bl_out);
  vsl_b_write(bl_out, la);
  bl_out.close();

  // binary test input file stream
  vsl_b_ifstream bl_in("test_line_2d_io.tmp");
  TEST("Opened test_line_2d_io.tmp for reading",(!bl_in), false);
  vsol_point_2d_sptr p0n, p1n;
  vsol_line_2d_sptr l_in = new vsol_line_2d(p0n, p1n), l_ina;
  l_in->b_read(bl_in);
  vsl_b_read(bl_in, l_ina);
  bl_in.close();

  vcl_cout << "Recovered lines" << *l_in << ' ' << *l_ina << '\n';

  TEST("Testing line io",
       l_in && l_ina &&
       *(l->p0())==*(l_in->p0()) && 
       *(l->p1())==*(l_in->p1()) &&
       *(la->p0())==*(l_ina->p0()) &&
       *(la->p1())==*(l_ina->p1()),
       true);

  // remove the temporary file
  vpl_unlink ("test_line_2d_io.tmp");
}


TESTMAIN(test_vsol_io);
