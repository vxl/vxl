//*****************************************************************************
// File name: test_vsol_io.cxx
// Description: Test binary I/O
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2004/05/08| Joseph Mundy             |Creation
//*****************************************************************************

//:
// \file
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <testlib/testlib_test.h>

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
  vcl_cout << "testing on points" << *p << *pa << '\n';

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

  vcl_cout << "Recovered point" << *p_in << '\n'
           << "Recovered point" << *p_ina << '\n';

  TEST("Testing point io",
       p_in && p_ina &&
       p->x()==p_in->x() && p->y()==p_in->y() &&
       pa->x()==p_ina->x() && pa->y()==p_ina->y(),
       true);

  // Test vector I/O
  vsl_b_ofstream bpv_out("test_point_2d_vec_io.tmp");
  vcl_vector<vsol_point_2d_sptr> points, points_in;
  vcl_cout << "Created the point vector ";
  for (int i = 0; i<3; i++)
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
  for (vcl_vector<vsol_point_2d_sptr>::iterator pit = points_in.begin();
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

  vcl_vector<vsol_line_2d_sptr> lines, lines_in;
  lines.push_back(l);   lines.push_back(la);
  vsl_b_ofstream blv_out("test_line_2d_io.tmp");
  vsl_b_write(blv_out, lines);
  blv_out.close();  
  
  vsl_b_ifstream blv_in("test_line_2d_io.tmp");
  vsl_b_read(blv_in, lines_in);
  blv_in.close();  
  good = true;
  k = 0;
  for(vcl_vector<vsol_line_2d_sptr>::iterator lit = lines_in.begin();
      lit != lines_in.end(); lit++, k++)
    {
      vcl_cout << "Saved line" << *lines[k] << ' ' 
               << "Read line" << *(*lit) << '\n';
      good = good && *(*lit) == *lines[k];
    }
  TEST("Testing line vector io", good, true);

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

  vsol_point_2d_sptr pb1 = new vsol_point_2d(4.4, 5.5);
  vsol_point_2d_sptr pb2 = new vsol_point_2d(6.6, 7.7);
  vsol_point_2d_sptr pb3 = new vsol_point_2d(8.8, 9.9);
  vcl_vector<vsol_point_2d_sptr> pointsb;
  pointsb.push_back(pb1);   pointsb.push_back(pb2);   pointsb.push_back(pb3);
  vsol_polyline_2d_sptr polyb = new vsol_polyline_2d(pointsb);
  vcl_vector<vsol_polyline_2d_sptr> plines, plines_in;
  plines.push_back(poly);   plines.push_back(polyb);
  vsl_b_ofstream bplv_out("test_polyline_2d_io.tmp");
  vsl_b_write(bplv_out, plines);
  bplv_out.close();  
  
  vsl_b_ifstream bplv_in("test_polyline_2d_io.tmp");
  vsl_b_read(bplv_in, plines_in);
  blv_in.close();  
  good = true;
  k = 0;
  for(vcl_vector<vsol_polyline_2d_sptr>::iterator plit = plines_in.begin();
      plit != plines_in.end(); plit++, k++)
    {
      vcl_cout << "Saved polyline" << *plines[k] << ' ' 
               << "Read polyline" << *(*plit) << '\n';
      good = good && *(*plit) == *plines[k];
    }
  TEST("Testing polyline vector io", good, true);

  // remove the temporary file
  vpl_unlink ("test_polyline_2d_io.tmp");

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


  pointsb.push_back(pointsb[0]);
  vsol_polygon_2d_sptr polygb = new vsol_polygon_2d(pointsb);
  vcl_vector<vsol_polygon_2d_sptr> plygns, plygns_in;
  plygns.push_back(polyg);   plygns.push_back(polygb);
  vsl_b_ofstream bplgv_out("test_polygon_2d_io.tmp");
  vsl_b_write(bplgv_out, plygns);
  bplgv_out.close();  
  
  vsl_b_ifstream bplgv_in("test_polygon_2d_io.tmp");
  vsl_b_read(bplgv_in, plygns_in);
  blv_in.close();  
  good = true;
  k = 0;
  for(vcl_vector<vsol_polygon_2d_sptr>::iterator plit = plygns_in.begin();
      plit != plygns_in.end(); plit++, k++)
    {
      vcl_cout << "Saved polygon" << *plygns[k] << ' ' 
               << "Read polygon" << *(*plit) << '\n';
      good = good && *(*plit) == *plygns[k];
    }
  TEST("Testing polygon vector io", good, true);
  // remove the temporary file
  vpl_unlink ("test_polygon_2d_io.tmp");

  //vsol_box_2d I/O
  vcl_cout << "\nTesting I/O for vsol_box_2d\n";

  vsl_b_ofstream bx_out("test_box_2d_io.tmp");
  TEST("Created test_box_2d_io.tmp for writing",(!bx_out), false);

  vsol_box_2d_sptr box = new vsol_box_2d();
  box->add_point(0, 1);   box->add_point(10, 11); 
  vcl_cout << "Writing box " << *box << '\n';
  vsl_b_write(bx_out, box);
  bx_out.close();

  vsl_b_ifstream bx_in("test_box_2d_io.tmp");
  TEST("Created test_box_2d_io.tmp for reading",(!bx_in), false);
  vsol_box_2d_sptr box_in;
  vsl_b_read(bx_in, box_in);
  vcl_cout << "Read box " << *box_in << '\n';
  TEST("Testing box io",
       box && box_in &&
       box->near_equal(*box_in, 0.01f),
       true);
  bx_in.close();

  vsol_box_2d_sptr boxb = new vsol_box_2d();
  boxb->add_point(100, 110);   boxb->add_point(101, 111); 
  vcl_vector<vsol_box_2d_sptr> boxes, boxes_in;
  boxes.push_back(box);   boxes.push_back(boxb);
  vsl_b_ofstream bboxv_out("test_box_2d_io.tmp");
  vsl_b_write(bboxv_out, boxes);
  bboxv_out.close();  
  
  vsl_b_ifstream bboxv_in("test_box_2d_io.tmp");
  vsl_b_read(bboxv_in, boxes_in);
  bboxv_in.close();  
  good = true;
  k = 0;
  for(vcl_vector<vsol_box_2d_sptr>::iterator plit = boxes_in.begin();
      plit != boxes_in.end(); plit++, k++)
    {
      vcl_cout << "Saved box" << *boxes[k] << ' ' 
               << "Read box" << *(*plit) << '\n';
      good = good && (*plit)->near_equal(*boxes[k], 0.01f);
    }
  TEST("Testing box vector io", good, true);

}

TESTMAIN(test_vsol_io);
