#include <testlib/testlib_test.h>
// not used? #include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vil3d/algo/vil3d_quad_distance_function.h>
#include <vil3d/vil3d_print.h>


void test_algo_quad_distance_function_float_float()
{
  vcl_cout << "***************************************************\n"
           << " Testing vil3d_quad_distance_function<float,float>\n"
           << "***************************************************\n";
  vil3d_image_view<float> src(7,7,7),dest;
  src.fill(1.0f);
  src(2,4,4)=-10.0f;
  vil3d_quad_distance_function(src,1,1,1,dest);
  TEST("Dest ni",dest.ni(),src.ni());
  TEST("Dest nj",dest.nj(),src.nj());
  TEST("Dest nk",dest.nk(),src.nk());
  TEST_NEAR("d(2,4,4)",dest(2,4,4),-10,1e-6);
  TEST_NEAR("d(2,5,4)",dest(2,5,4),-9,1e-6);
  TEST_NEAR("d(2,6,4)",dest(2,6,4),-6,1e-6);
  TEST_NEAR("d(3,6,4)",dest(3,6,4),-5,1e-6);
  TEST_NEAR("d(4,6,4)",dest(4,6,4),-2,1e-6);
  TEST_NEAR("d(0,1,4)",dest(0,1,4),1,1e-6);
  TEST_NEAR("d(0,2,4)",dest(0,2,4),-2,1e-6);
  TEST_NEAR("d(4,6,3)",dest(4,6,3),-1,1e-6);
  TEST_NEAR("d(4,6,5)",dest(4,6,5),-1,1e-6);

  vcl_cout<<"Extra minima."<<vcl_endl;
  src(1,2,4)=-10.0f;
  vil3d_quad_distance_function(src,1,1,1,dest);
  TEST_NEAR("d(1,2,4)",dest(1,2,4),-10,1e-6);
}

void test_algo_quad_distance_function_with_pos_float_float()
{
  vcl_cout << "********************************************\n"
           << " Testing vil_quad_distance_function<float,float> with position\n"
           << "********************************************\n";
  vil3d_image_view<float> src(7,7,7),dest;
  vil3d_image_view<int> pos;
  src.fill(1.0f);
  src(2,4,3)=-10.0f;
  vil3d_quad_distance_function(src,1,1,1,dest,pos);
  TEST("Dest ni",dest.ni(),src.ni());
  TEST("Dest nj",dest.nj(),src.nj());
  TEST("Dest nk",dest.nk(),src.nk());
  TEST_NEAR("d(2,4,3)",dest(2,4,3),-10,1e-6);
  TEST("pos(2,4,3)=(2,4,3)",pos(2,4,3,0)==2 && pos(2,4,3,1)==4 && pos(2,4,3,2)==3,true);
  TEST_NEAR("d(2,5,3)",dest(2,5,3),-9,1e-6);
  TEST("pos(2,5,3)=(2,4,3)",pos(2,5,3,0)==2 && pos(2,5,3,1)==4 && pos(2,5,3,2)==3,true);
  TEST_NEAR("d(2,6,3)",dest(2,6,3),-6,1e-6);
  TEST("pos(2,6,3)=(2,4,3)",pos(2,6,3,0)==2 && pos(2,6,3,1)==4 && pos(2,6,3,2)==3,true);
  TEST_NEAR("d(3,6,3)",dest(3,6,3),-5,1e-6);
  TEST("pos(3,6,3)=(2,4,3)",pos(3,6,3,0)==2 && pos(3,6,3,1)==4 && pos(3,6,3,2)==3,true);
  TEST_NEAR("d(4,6,3)",dest(4,6,3),-2,1e-6);
  TEST("pos(4,6,3)=(2,4,3)",pos(4,6,3,0)==2 && pos(4,6,3,1)==4 && pos(4,6,3,2)==3,true);
  TEST_NEAR("d(4,6,4)",dest(4,6,4),-1,1e-6);
  TEST("pos(4,6,4)=(2,4,3)",pos(4,6,3,0)==2 && pos(4,6,3,1)==4 && pos(4,6,4,2)==3,true);
  TEST_NEAR("d(0,1,3)",dest(0,1,3),1,1e-6);
  TEST("pos(0,1,3)=(0,1,3)",pos(0,1,3,0)==0 && pos(0,1,3,1)==1 && pos(0,1,3,2)==3,true);
  vcl_cout<<"pos(0,0,3)=("<<pos(0,0,3,0)<<','<<pos(0,0,3,1)<<','<<pos(0,0,3,2)<<')'<<vcl_endl
           <"pos(0,1,3)=("<<pos(0,1,3,0)<<','<<pos(0,1,3,1)<<','<<pos(0,1,3,2)<<')'<<vcl_endl
           <"pos(0,2,3)=("<<pos(0,2,3,0)<<','<<pos(0,2,3,1)<<','<<pos(0,2,3,2)<<')'<<vcl_endl;
  TEST_NEAR("d(0,2,3)",dest(0,2,3),-2,1e-6);
  TEST("pos(0,2,3)=(2,4,3)",pos(0,2,3,0)==2 && pos(0,2,3,1)==4 && pos(0,2,3,2)==3,true);

  vcl_cout<<"Extra minima."<<vcl_endl;
  src(1,2,3)=-10.0f;
  vil3d_quad_distance_function(src,1,1,1,dest,pos);
  TEST_NEAR("d(1,2,3)",dest(1,2,3),-10,1e-6);
  TEST("pos(1,2,3)=(1,2,3)",pos(1,2,3,0)==1 && pos(1,2,3,1)==2 && pos(1,2,3,2)==3,true);
  TEST("pos(0,2,3)=(1,2,3)",pos(0,2,3,0)==1 && pos(0,2,3,1)==2 && pos(0,2,3,2)==3,true);
}

MAIN( test_algo_quad_distance_function )
{
  START( "vil3d_quad_distance_function" );
  test_algo_quad_distance_function_float_float();
  test_algo_quad_distance_function_with_pos_float_float();
  SUMMARY();
}
