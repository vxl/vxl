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

void test_vsol_io()
{
  //vsol_point_2d I/O
  vcl_cout << "Testing I/O for vsol_point_2d\n";

  vsol_point_2d_sptr p=new vsol_point_2d(1,2);
  vcl_cout << "testing on point" << *p << "\n";

  vsl_b_ofstream bp_out("test_point_2d_io.tmp");
  TEST("Created test_point_2d_io.tmp for writing",(!bp_out), false);

  vsl_b_write(bp_out, p);
  bp_out.close();

  // binary test input file stream
  vsl_b_ifstream bp_in("test_point_2d_io.tmp");
  TEST("Opened test_point_2d_io.tmp for reading",(!bp_in), false);

  vsol_point_2d_sptr p_in = new vsol_point_2d(0, 0);

  vsl_b_read(bp_in, p_in);
  bp_in.close();

  vcl_cout << "Recovered point" << *p_in << '\n';

  TEST("Testing io", p->x()==p_in->x() && p->y()==p_in->y(), true);

  // remove the temporary file
  vpl_unlink ("test_point_2d_io.tmp");
}


TESTMAIN(test_vsol_io);
