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

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_triangle_2d_sptr.h>
#include <vsol/vsol_triangle_2d.h>
#include <vsol/vsol_rectangle_2d_sptr.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_group_2d_sptr.h>
#include <vsol/vsol_group_2d.h>

void test_vsol_io()
{
  vsl_add_to_binary_loader(vsol_point_2d());
  vsl_add_to_binary_loader(vsol_line_2d());
  vsl_add_to_binary_loader(vsol_polyline_2d());
  vsl_add_to_binary_loader(vsol_polygon_2d());
  vsl_add_to_binary_loader(vsol_conic_2d());
  vsl_add_to_binary_loader(vsol_rectangle_2d());
  vsl_add_to_binary_loader(vsol_triangle_2d());
  vsl_add_to_binary_loader(vsol_group_2d());

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
  bool g = p_in&&p_ina;
  if(g)
    vcl_cout << "Recovered point" << *p_in << '\n'
             << "Recovered point" << *p_ina << '\n';
  TEST("Testing null recovered points", g, true);
  if(g)
    TEST("Testing valid point coordinates",
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
      if(!(*pit))
        {
          good = false;
          continue;
        }
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
  vcl_cout << "testing on lines\n " << *l << ' ' << *la;

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

  g = l_in&&l_ina;
  if(g)
    vcl_cout << "Recovered lines" << *l_in << ' ' << *l_ina << '\n';
  TEST("Testing null lines", g, true);
  if(g)
    TEST("Testing line coordinates",
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
      if(!(*lit))
        {
          good = false;
          continue;
        }
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
  vcl_cout << "Writing polyline " << *poly;
  vsl_b_write(ply_out, poly);
  ply_out.close();

  vsl_b_ifstream ply_in("test_polyline_2d_io.tmp");
  TEST("Created test_polyline_2d_io.tmp for reading",(!ply_in), false);
  vsol_polyline_2d_sptr poly_in;
  vsl_b_read(ply_in, poly_in);
  g = poly_in;
  if(g)
    vcl_cout << "Read polyline " << *poly_in << '\n';
  if(g)  
    TEST("Testing polyline io",
         poly&&
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
      if(!(*plit))
        {
          good = false;
          continue;
        }
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
  vcl_cout << "Writing polygon " << *polyg;
  vsl_b_write(pgy_out, polyg);
  pgy_out.close();

  vsl_b_ifstream pgy_in("test_polygon_2d_io.tmp");
  TEST("Created test_polygon_2d_io.tmp for reading",(!pgy_in), false);
  vsol_polygon_2d_sptr polyg_in;
  vsl_b_read(pgy_in, polyg_in);
  g = polyg_in;
  if(g)
    vcl_cout << "Read polygon " << *polyg_in << '\n';
  if(g)
    TEST("Testing polygon io",
         polyg && 
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
      if(!(*plit))
        {
          good = false;
          continue;
        }
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
  g = box_in;
  if(g)
    vcl_cout << "Read box " << *box_in << '\n';
  if(g)
    TEST("Testing box io",
         box &&
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
  for(vcl_vector<vsol_box_2d_sptr>::iterator bit = boxes_in.begin();
      bit != boxes_in.end(); bit++, k++)
    {
      if(!(*bit))
        {
          good = false;
          continue;
        }
      vcl_cout << "Saved box" << *boxes[k] << ' ' 
               << "Read box" << *(*bit) << '\n';
      good = good && (*bit)->near_equal(*boxes[k], 0.01f);
    }
  TEST("Testing box vector io", good, true);

  // remove the temporary file
  vpl_unlink ("test_box_2d_io.tmp");

  //vsol_conic_2d I/O
  vcl_cout << "\nTesting I/O for vsol_conic_2d\n";

  vsl_b_ofstream cc_out("test_conic_2d_io.tmp");
  TEST("Created test_conic_2d_io.tmp for writing",(!cc_out), false);
  vsol_point_2d_sptr pc0 = new vsol_point_2d(0, 1);
  vsol_point_2d_sptr pc1 = new vsol_point_2d(0.707107,0.707107);
  vsol_conic_2d_sptr conic = new vsol_conic_2d(1, 0, 1, 0, 0, -1);
  conic->set_p0(pc0);   conic->set_p1(pc1); 
  vcl_cout << "Writing conic " << *conic << '\n';
  vsl_b_write(cc_out, conic);
  cc_out.close();

  vsl_b_ifstream cc_in("test_conic_2d_io.tmp");
  TEST("Created test_conic_2d_io.tmp for reading",(!cc_in), false);
  vsol_conic_2d_sptr conic_in;
  vsl_b_read(cc_in, conic_in);
  g = conic_in;
  if(g)
    vcl_cout << "Read conic " << *conic_in << '\n';
  if(g)
    TEST("Testing conic io",
         conic &&
         static_cast<vgl_conic<double> >(*conic) == 
         static_cast<vgl_conic<double> >(*conic_in) &&
         *(conic->p0()) == *(conic_in->p0()) &&
         *(conic->p1()) == *(conic_in->p1()),
         true);
  cc_in.close();

  vsol_point_2d_sptr pc0a = new vsol_point_2d(0, 2);
  vsol_point_2d_sptr pc1a = new vsol_point_2d(2, 0);
  vsol_conic_2d_sptr conicb = new vsol_conic_2d(1, 0, 1, 0, 0, -4);
  conicb->set_p0(pc0a);   conicb->set_p1(pc1a); 
  vcl_vector<vsol_conic_2d_sptr> conics, conics_in;
  conics.push_back(conic);   conics.push_back(conicb);
  vsl_b_ofstream ccv_out("test_conic_2d_io.tmp");
  vsl_b_write(ccv_out, conics);
  ccv_out.close();  
  
  vsl_b_ifstream ccv_in("test_conic_2d_io.tmp");
  vsl_b_read(ccv_in, conics_in);
  ccv_in.close();  
  good = true;
  k = 0;
  for(vcl_vector<vsol_conic_2d_sptr>::iterator cit = conics_in.begin();
      cit != conics_in.end(); cit++, k++)
    {
      if(!(*cit))
        {
          good = false;
          continue;
        }
      vcl_cout << "Saved conic" << *conics[k] << '\n' 
               << "Read conic" << *(*cit) << '\n';
      
      good = good && 
        (
         static_cast<vgl_conic<double> >(*(*cit)) == 
         static_cast<vgl_conic<double> >(*(conics[k]))
         );
       good = good && *((*cit)->p0()) == *(conics[k]->p0());
       good = good && *((*cit)->p1()) == *(conics[k]->p1());
    }
  TEST("Testing conic vector io", good, true);

  // remove the temporary file
  vpl_unlink ("test_conic_2d_io.tmp");

  //vsol_triangle_2d I/O
  vcl_cout << "\nTesting I/O for vsol_triangle_2d\n";

  vsl_b_ofstream tri_out("test_triangle_2d_io.tmp");
  TEST("Created test_triangle_2d_io.tmp for writing",(!tri_out), false);
  vsol_point_2d_sptr tp0 = new vsol_point_2d(0, 0);
  vsol_point_2d_sptr tp1 = new vsol_point_2d(1, 0);
  vsol_point_2d_sptr tp2 = new vsol_point_2d(1, 1);
  vsol_triangle_2d_sptr triangle = new vsol_triangle_2d(tp0, tp1, tp2);
  vcl_cout << "Writing triangle " << *triangle << '\n';
  vsl_b_write(tri_out, triangle);
  tri_out.close();

  vsl_b_ifstream tri_in("test_triangle_2d_io.tmp");
  TEST("Created test_triangle_2d_io.tmp for reading",(!tri_in), false);
  vsol_triangle_2d_sptr triangle_in;
  vsl_b_read(tri_in, triangle_in);
  g = triangle_in;
  if(g)
    vcl_cout << "Read triangle " << *triangle_in << '\n';
  if(g)
    TEST("Testing triangle io",
         triangle &&
         (*triangle) == (*triangle_in),
         true);
  tri_in.close();

  vsol_point_2d_sptr tp0a = new vsol_point_2d(2, 2);
  vsol_point_2d_sptr tp1a = new vsol_point_2d(4, 2);
  vsol_point_2d_sptr tp2a = new vsol_point_2d(4, 4);
  vsol_triangle_2d_sptr trianglea = new vsol_triangle_2d(tp0a, tp1a, tp2a);
  vcl_vector<vsol_triangle_2d_sptr> triangles, triangles_in;
  triangles.push_back(triangle);   triangles.push_back(trianglea);
  vsl_b_ofstream triv_out("test_triangle_2d_io.tmp");
  vsl_b_write(triv_out, triangles);
  triv_out.close();  
  
  vsl_b_ifstream triv_in("test_triangle_2d_io.tmp");
  vsl_b_read(triv_in, triangles_in);
  triv_in.close();  
  good = true;
  k = 0;
  for(vcl_vector<vsol_triangle_2d_sptr>::iterator trit = triangles_in.begin();
      trit != triangles_in.end(); trit++, k++)
    {
      if(!(*trit))
        {
          good = false;
          continue;
        }
      vcl_cout << "Saved triangle" << *triangles[k] << '\n' 
               << "Read triangle" << *(*trit) << '\n';
      good = good && (*(*trit)) == *(triangles[k]);
    }
  TEST("Testing triangle vector io", good, true);

  // remove the temporary file
  vpl_unlink ("test_triangle_2d_io.tmp");

  //vsol_rectangle_2d I/O
  vcl_cout << "\nTesting I/O for vsol_rectangle_2d\n";

  vsl_b_ofstream rect_out("test_rectangle_2d_io.tmp");
  TEST("Created test_rectangle_2d_io.tmp for writing",(!rect_out), false);
  vsol_point_2d_sptr rp0 = new vsol_point_2d(0, 0);
  vsol_point_2d_sptr rp1 = new vsol_point_2d(1, 0);
  vsol_point_2d_sptr rp2 = new vsol_point_2d(1, 1);
  vsol_point_2d_sptr rp3 = new vsol_point_2d(0, 1);
  vsol_rectangle_2d_sptr rectangle = new vsol_rectangle_2d(rp0, rp1, rp2, rp3);
  vcl_cout << "Writing rectangle " << *rectangle << '\n';
  vsl_b_write(rect_out, rectangle);
  rect_out.close();

  vsl_b_ifstream rect_in("test_rectangle_2d_io.tmp");
  TEST("Created test_rectangle_2d_io.tmp for reading",(!rect_in), false);
  vsol_rectangle_2d_sptr rectangle_in;
  vsl_b_read(rect_in, rectangle_in);
  g = rectangle_in;
  if(g)
    vcl_cout << "Read rectangle " << *rectangle_in << '\n';
  if(g)
    TEST("Testing rectangle io",
         rectangle &&
         (*rectangle) == (*rectangle_in),
         true);
  rect_in.close();

  vsol_point_2d_sptr rp0a = new vsol_point_2d(2, 2);
  vsol_point_2d_sptr rp1a = new vsol_point_2d(4, 2);
  vsol_point_2d_sptr rp2a = new vsol_point_2d(4, 4);
  vsol_point_2d_sptr rp3a = new vsol_point_2d(2, 4);
  vsol_rectangle_2d_sptr rectanglea = 
    new vsol_rectangle_2d(rp0a, rp1a, rp2a, rp3a);
  vcl_vector<vsol_rectangle_2d_sptr> rectangles, rectangles_in;
  rectangles.push_back(rectangle);   rectangles.push_back(rectanglea);
  vsl_b_ofstream rectv_out("test_rectangle_2d_io.tmp");
  vsl_b_write(rectv_out, rectangles);
  rectv_out.close();  
  
  vsl_b_ifstream rectv_in("test_rectangle_2d_io.tmp");
  vsl_b_read(rectv_in, rectangles_in);
  rectv_in.close();  
  good = true;
  k = 0;
  for(vcl_vector<vsol_rectangle_2d_sptr>::iterator rectt = rectangles_in.begin();
      rectt != rectangles_in.end(); rectt++, k++)
    {
      if(!(*rectt))
        {
          good = false;
          continue;
        }
      vcl_cout << "Saved rectangle" << *rectangles[k] << '\n' 
               << "Read rectangle" << *(*rectt) << '\n';
      good = good && (*(*rectt)) == *(rectangles[k]);
    }
  TEST("Testing rectangle vector io", good, true);
  // remove the temporary file
  vpl_unlink ("test_rectangle_2d_io.tmp");

  //vsol_group_2d I/O
  vcl_cout << "\nTesting I/O for vsol_group_2d\n";

  vsl_b_ofstream grp_out("test_group_2d_io.tmp");
  TEST("Created test_group_2d_io.tmp for writing",(!grp_out), false);
  vsol_group_2d_sptr grp = new vsol_group_2d();
  grp->add_object(p->cast_to_spatial_object_2d());
  grp->add_object(l->cast_to_spatial_object_2d());
  grp->add_object(conic->cast_to_spatial_object_2d());
  grp->add_object(poly->cast_to_spatial_object_2d());
  grp->add_object(polyg->cast_to_spatial_object_2d());
  grp->add_object(triangle->cast_to_spatial_object_2d());
  grp->add_object(rectangle->cast_to_spatial_object_2d());

  vcl_cout << "Writing group " << *grp << '\n';
  vsl_b_write(grp_out, grp);
  grp_out.close();

  vsl_b_ifstream grp_in("test_group_2d_io.tmp");
  TEST("Created test_group_2d_io.tmp for reading",(!grp_in), false);
  vsol_group_2d_sptr group_in;
  vsl_b_read(grp_in, group_in);
  if(group_in)
    vcl_cout << "Read group " << *group_in << '\n';
  if(group_in)
    TEST("Testing group io",grp && (*grp) == (*group_in), true);
  grp_in.close();

  // remove the temporary file
  vpl_unlink ("test_group_2d_io.tmp");

  //Test Polymorphic I/O
  vcl_cout << "Testing Polymorphic I/O\n";

  vsol_spatial_object_2d_sptr obj1 = p0.ptr();   // A point
  vsol_spatial_object_2d_sptr obj2 = l.ptr();    // A line
  vsol_spatial_object_2d_sptr obj3 = poly.ptr(); // A polyline
  vsol_spatial_object_2d_sptr obj4 = polyg.ptr();// A polygon

  vsl_b_ofstream bpm_out("test_polymorph_io.tmp");
  TEST("Created test_polymorph_io.tmp for writing",(!bpm_out), false);
  vsl_b_write(bpm_out, obj1);
  vsl_b_write(bpm_out, obj2);
  vsl_b_write(bpm_out, obj3);
  vsl_b_write(bpm_out, obj4);
  bpm_out.close();

  if (obj1) vcl_cout << "Saved object 1: "; obj1->print(vcl_cout);
  if (obj2) vcl_cout << "Saved object 2: "; obj2->print(vcl_cout);
  if (obj3) vcl_cout << "Saved object 3: "; obj3->print(vcl_cout);
  if (obj4) vcl_cout << "Saved object 4: "; obj4->print(vcl_cout);

  vsl_b_ifstream bpm_in("test_polymorph_io.tmp");
  TEST("Opened test_polymorph_io.tmp for reading",(!bpm_in), false);

  vsol_spatial_object_2d_sptr obj1_in, obj2_in, obj3_in, obj4_in;
  vsl_b_read(bpm_in, obj1_in);
  vsl_b_read(bpm_in, obj2_in);
  vsl_b_read(bpm_in, obj3_in);
  vsl_b_read(bpm_in, obj4_in);
  bpm_in.close();

  vcl_cout << "Recovered object 1: "; obj1_in->print(vcl_cout);
  vcl_cout << "Recovered object 2: "; obj2_in->print(vcl_cout);
  vcl_cout << "Recovered object 3: "; obj3_in->print(vcl_cout);
  vcl_cout << "Recovered object 4: "; obj4_in->print(vcl_cout);

  TEST("Testing polymorphic io", true, true);
}

TESTMAIN(test_vsol_io);
