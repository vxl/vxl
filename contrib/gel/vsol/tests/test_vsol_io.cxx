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
#include <vcl_vector.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <testlib/testlib_test.h>
//:
// \file

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>

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

  // Test vector I/O
  vsl_b_ofstream bpv_out("test_point_2d_vec_io.tmp");
  vcl_vector<vsol_point_2d_sptr> points, points_in;
  vcl_cout << "Created the point vector ";
  for(int i = 0; i<3; i++)
    {
      vsol_point_2d_sptr p = new vsol_point_2d(i, i);
      vcl_cout << *p << ' ';
      points.push_back(p);
    }
  vcl_cout << '\n';
 vsl_b_write(bpv_out, points);
 bpv_out.close();  

 //open the points file
 vsl_b_ifstream bpv_in("test_point_2d_vec_io.tmp");
 vsl_b_read(bpv_in, points_in);
 vcl_cout << "Read the point vector ";
 int k = 0;
 bool good = true;
 for(vcl_vector<vsol_point_2d_sptr>::iterator pit = points_in.begin();
     pit != points_in.end(); pit++, k++)
   {
      vcl_cout << *(*pit) << ' ';
      good = good && k == (int)(*pit)->x();
   }
 vcl_cout << '\n';	
 TEST("Testing point vector io", good, true);

  // remove the temporary file
  vpl_unlink ("test_point_2d_vec_io.tmp");


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

  //vsol_polyline_2d I/O
  vcl_cout << "\nTesting I/O for vsol_polyline_2d\n";

  vsl_b_ofstream ply_out("test_polyline_2d_io.tmp");
  TEST("Created test_polyline_2d_io.tmp for writing",(!ply_out), false);

  vsol_polyline_2d_sptr poly = new vsol_polyline_2d(points);
  vcl_cout << "Writing polyline " << *poly << '\n';
  vsl_b_write(ply_out, poly);
  ply_out.close();
  
  vsl_b_ifstream ply_in("test_polyline_2d_io.tmp");
  TEST("Created test_polyline_2d_io.tmp for reading",(!ply_in), false);
  vsol_polyline_2d_sptr poly_in;
  vsl_b_read(ply_in, poly_in);
  vcl_cout << "Read polyline " << *poly_in << '\n';
  TEST("Testing polyline io",
       poly && poly_in &&
       *poly == *poly_in,
       true);

  //vsol_polygon_2d I/O
  vcl_cout << "\nTesting I/O for vsol_polygon_2d\n";
  points.push_back(points[0]);//close the polygon
  vsl_b_ofstream pgy_out("test_polygon_2d_io.tmp");
  TEST("Created test_polygon_2d_io.tmp for writing",(!pgy_out), false);

  vsol_polygon_2d_sptr polyg = new vsol_polygon_2d(points);
  vcl_cout << "Writing polygon " << *polyg << '\n';
  vsl_b_write(pgy_out, polyg);
  pgy_out.close();
  
  vsl_b_ifstream pgy_in("test_polygon_2d_io.tmp");
  TEST("Created test_polygon_2d_io.tmp for reading",(!pgy_in), false);
  vsol_polygon_2d_sptr polyg_in;
  vsl_b_read(pgy_in, polyg_in);
  vcl_cout << "Read polygon " << *polyg_in << '\n';
  TEST("Testing polygon io",
       polyg && polyg_in &&
       *polyg == *polyg_in,
       true);


}


TESTMAIN(test_vsol_io);
