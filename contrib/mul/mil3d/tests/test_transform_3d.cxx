// This is mul/mil3d/tests/test_transform_3d.cxx
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_distance.h>
#include <mil3d/mil3d_transform_3d.h>
#include <testlib/testlib_test.h>

void test_product(const mil3d_transform_3d& t0, const mil3d_transform_3d& t1)
{
  mil3d_transform_3d t01 = t0*t1;
  vgl_point_3d<double> p0(1,2,0);
  vgl_point_3d<double> q1=t0(t1(p0));
  vgl_point_3d<double> q2=t01(p0);
  TEST("Testing product",vgl_distance(q1,q2)<1e-6,true);
}

void test_products(const mil3d_transform_3d& trans1)
{
  mil3d_transform_3d trans0;
  trans0.set_identity();
  vcl_cout<<"Pre-multiply with Identity"<<vcl_endl;
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with Translation"<<vcl_endl;
  trans0.set_translation(1.2, 3.4,0);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with ZoomOnly"<<vcl_endl;
  trans0.set_zoom_only(2, 3, -4, 0);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with RigidBody"<<vcl_endl;
  trans0.set_rigid_body(1.4,0.9,1.1,1.5,3.4, 5.6);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with Similarity"<<vcl_endl;
  trans0.set_similarity(2,3,1,3,4,5,6);
  test_product(trans0,trans1);

  vcl_cout<<"Pre-multiply with Affine"<<vcl_endl;
  trans0.set_affine(0.5,2,3,2,3,1.5, 3.4, 5.6,0.1);
  test_product(trans0,trans1);
}

void test_the_transform(mil3d_transform_3d& t)
{
   mil3d_transform_3d t_inv = t.inverse();
   vgl_point_3d<double> p0(5,7,0);
   vgl_point_3d<double> p1 = t(p0);
   TEST("Inverse correct",vgl_distance(t_inv(p1),p0)<1e-6,true);

   vnl_matrix<double> I = t.matrix() * t_inv.matrix();
   vnl_matrix<double> I0(4,4);
   I0.fill(0);
   I0.fill_diagonal(1);

   TEST("matrix() correct",(I-I0).absolute_value_max()<1e-6,true);

   test_products(t);
}

void test_transform_3d()
{
  vcl_cout << "***********************" << vcl_endl;
  vcl_cout << " Testing mil3d_transform_3d " << vcl_endl;
  vcl_cout << "***********************" << vcl_endl;

  mil3d_transform_3d trans0;
  vgl_point_3d<double> p0(1,2,0),p1;

  trans0.set_identity();
  vcl_cout<<"== Testing Identity =="<<vcl_endl;
  TEST("Identity transform",vgl_distance(trans0(p0),p0)<1e-8,true);
  test_the_transform(trans0);

  vcl_cout<<"== Testing Translation =="<<vcl_endl;
  trans0.set_translation(1.2, 3.4,0);
  p1 = vgl_point_3d<double>(2.2,5.4,0);
  TEST("Translation transform",vgl_distance(trans0(p0),p1)<1e-8,true);
  test_the_transform(trans0);

  vcl_cout<<"== Testing ZoomOnly =="<<vcl_endl;
  trans0.set_zoom_only(2, 3, 4,0);
  p1 = vgl_point_3d<double>(5,8,0);
  TEST("Zoom transform",vgl_distance(trans0(p0),p1)<1e-8,true);
  test_the_transform(trans0);

  vcl_cout<<"== Testing RigidBody =="<<vcl_endl;
  trans0.set_rigid_body(1.2,1,5,1,2,3);
  test_the_transform(trans0);

  vcl_cout<<"== Testing Similarity =="<<vcl_endl;
  trans0.set_similarity(0.51,2,3,4,0.2,1,-4);
  test_the_transform(trans0);

   vcl_cout<<"== Testing Affine =="<<vcl_endl;
   trans0.set_affine(0.2,-0.3,4,2,1,4,5,0.1,-0.21);
   test_the_transform(trans0);

  // -------- Test the binary I/O --------


  vsl_b_ofstream bfs_out("test_transform_3d.bvl.tmp");
  TEST ("Created test_transform_3d.bvl.tmp for writing",
             (!bfs_out), false);
  vsl_b_write(bfs_out, trans0);
  bfs_out.close();

  mil3d_transform_3d trans0_in;

  vsl_b_ifstream bfs_in("test_transform_3d.bvl.tmp");
  TEST ("Opened test_transform_3d.bvl.tmp for reading",
           (!bfs_in), false);
  vsl_b_read(bfs_in, trans0_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vnl_vector<double> v0,v0_in;
  trans0.params(v0);
  trans0_in.params(v0_in);
  TEST("Binary IO for form", trans0.form()==trans0_in.form(),true);
  TEST("Binary IO for params",
       (v0-v0_in).magnitude()<1e-6,true);
}

TESTLIB_DEFINE_MAIN(test_transform_3d);
