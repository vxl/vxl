#include <vector>
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/algo/vil_quad_distance_function.h>

void test_algo_quad_envelope_float()
{
  std::vector<float> data(5);
  for (unsigned i=0;i<5;++i) data[i]=1.0f;
  data[2]=-10;
  std::vector<double> x,y,z;
  double a=0.8;
  vil_quad_envelope(&data[0],1,data.size(),x,y,z,a);
  TEST("Only one parabola in envelope",x.size(),1);
  TEST_NEAR("Centre",x[0],2.0,1e-6);
  TEST_NEAR("Value",y[0],-10.0,1e-6);
  TEST_NEAR("Low limit",z[0],0.0,1e-6);
  TEST_NEAR("High limit",z[1],5.0,1e-6);

  data[0]=-10;
  vil_quad_envelope(&data[0],1,data.size(),x,y,z,a);
  TEST("Two parabolas in envelope",x.size(),2);
  std::cout<<"N.parabolas="<<x.size()<<std::endl;
  TEST_NEAR("Centre",x[0],0.0,1e-6);
  TEST_NEAR("Value",y[0],-10.0,1e-6);
  TEST_NEAR("Low limit",z[0],0.0,1e-6);
  TEST_NEAR("High limit",z[1],1.0,1e-6);
  TEST_NEAR("Centre 2",x[1],2.0,1e-6);
  TEST_NEAR("Value 2",y[1],-10.0,1e-6);
  TEST_NEAR("High limit 2",z[2],5.0,1e-6);


  data[4]=-100;
  vil_quad_envelope(&data[0],1,data.size(),x,y,z,a);
  TEST("Only one parabola in envelope",x.size(),1);
  TEST_NEAR("Centre",x[0],4.0,1e-6);
  TEST_NEAR("Value",y[0],-100.0,1e-6);
  TEST_NEAR("Low limit",z[0],0.0,1e-6);
  TEST_NEAR("High limit",z[1],5.0,1e-6);

  data[4]=-10;
  vil_quad_envelope(&data[0],1,data.size(),x,y,z,a);
  TEST("Three parabolas in envelope",x.size(),3);
  std::cout<<"N.parabolas="<<x.size()<<std::endl;

  std::vector<double> dest(5);
  vil_sample_quad_envelope(x,y,z,a,&dest[0],1,dest.size());
  TEST_NEAR("d[0]",dest[0],-10,1e-6);
  TEST_NEAR("d[1]",dest[1],-9.2,1e-6);
  TEST_NEAR("d[2]",dest[2],-10,1e-6);
  TEST_NEAR("d[3]",dest[3],-9.2,1e-6);
  TEST_NEAR("d[4]",dest[4],-10,1e-6);

  std::vector<int> pos(5);
  data[2]=-10.1f;  // Removes ambiguities
  vil_quad_envelope(&data[0],1,data.size(),x,y,z,a);
  vil_sample_quad_envelope_with_pos(x,y,z,a,&dest[0],1,dest.size(),&pos[0],1);
  TEST("pos[0]",pos[0],0);
  TEST("pos[1]",pos[1],2);
  TEST("pos[2]",pos[2],2);
  TEST("pos[3]",pos[3],2);
  TEST("pos[4]",pos[4],4);


  std::vector<double> dest2(5);
  vil_quad_distance_function_1D(&data[0],1,data.size(),a,&dest2[0],1);
  TEST_NEAR("d[0]",dest2[0],-10,1e-6);
  TEST_NEAR("d[1]",dest2[1],-9.3,1e-6);
  TEST_NEAR("d[2]",dest2[2],-10.1,1e-6);
  TEST_NEAR("d[3]",dest2[3],-9.3,1e-6);
  TEST_NEAR("d[4]",dest2[4],-10,1e-6);
}

void test_algo_quad_distance_function_float_float()
{
  std::cout << "*************************************************\n"
           << " Testing vil_quad_distance_function<float,float>\n"
           << "*************************************************\n";
  vil_image_view<float> src(7,7),dest,pos;
  src.fill(1.0f);
  src(2,4)=-10.0f;
  vil_quad_distance_function(src,1,1,dest);
  TEST("Dest ni",dest.ni(),src.ni());
  TEST("Dest nj",dest.nj(),src.nj());
  TEST_NEAR("d(2,4)",dest(2,4),-10,1e-6);
  TEST_NEAR("d(2,5)",dest(2,5),-9,1e-6);
  TEST_NEAR("d(2,6)",dest(2,6),-6,1e-6);
  TEST_NEAR("d(3,6)",dest(3,6),-5,1e-6);
  TEST_NEAR("d(4,6)",dest(4,6),-2,1e-6);
  TEST_NEAR("d(0,1)",dest(0,1),1,1e-6);
  TEST_NEAR("d(0,2)",dest(0,2),-2,1e-6);

  std::cout<<"Extra minima."<<std::endl;
  src(1,2)=-10.0f;
  vil_quad_distance_function(src,1,1,dest);
  TEST_NEAR("d(1,2)",dest(1,2),-10,1e-6);
}

void test_algo_quad_distance_function_with_pos_float_float()
{
  std::cout << "***************************************************************\n"
           << " Testing vil_quad_distance_function<float,float> with position\n"
           << "***************************************************************\n";
  vil_image_view<float> src(7,7),dest;
  vil_image_view<int> pos;
  src.fill(1.0f);
  src(2,4)=-10.0f;
  vil_quad_distance_function(src,1,1,dest,pos);
  TEST("Dest ni",dest.ni(),src.ni());
  TEST("Dest nj",dest.nj(),src.nj());
  TEST_NEAR("d(2,4)",dest(2,4),-10,1e-6);
  TEST("pos(2,4)=(2,4)",pos(2,4,0)==2 && pos(2,4,1)==4,true);
  TEST_NEAR("d(2,5)",dest(2,5),-9,1e-6);
  TEST("pos(2,5)=(2,4)",pos(2,5,0)==2 && pos(2,5,1)==4,true);
  TEST_NEAR("d(2,6)",dest(2,6),-6,1e-6);
  TEST("pos(2,6)=(2,4)",pos(2,6,0)==2 && pos(3,6,1)==4,true);
  TEST_NEAR("d(3,6)",dest(3,6),-5,1e-6);
  TEST("pos(3,6)=(2,4)",pos(3,6,0)==2 && pos(3,6,1)==4,true);
  TEST_NEAR("d(4,6)",dest(4,6),-2,1e-6);
  TEST("pos(4,6)=(2,4)",pos(4,6,0)==2 && pos(4,6,1)==4,true);
  TEST_NEAR("d(0,1)",dest(0,1),1,1e-6);
  TEST("pos(0,1)=(0,1)",pos(0,1,0)==0 && pos(0,1,1)==1,true);
  std::cout<<"pos(0,0)=("<<pos(0,0,0)<<','<<pos(0,0,1)<<")\n"
          <<"pos(0,1)=("<<pos(0,1,0)<<','<<pos(0,1,1)<<")\n"
          <<"pos(0,2)=("<<pos(0,2,0)<<','<<pos(0,2,1)<<')'<<std::endl;
  TEST_NEAR("d(0,2)",dest(0,2),-2,1e-6);
  TEST("pos(0,2)=(2,4)",pos(0,2,0)==2 && pos(0,2,1)==4,true);

  std::cout<<"Extra minima."<<std::endl;
  src(1,2)=-10.0f;
  vil_quad_distance_function(src,1,1,dest,pos);
  TEST_NEAR("d(1,2)=-10",dest(1,2),-10,1e-6);
  TEST("pos(1,2)=(1,2)",pos(1,2,0)==1 && pos(1,2,1)==2,true);
  TEST("pos(0,2)=(1,2)",pos(0,2,0)==1 && pos(0,2,1)==2,true);
}

static void test_algo_quad_distance_function()
{
  test_algo_quad_envelope_float();
  test_algo_quad_distance_function_float_float();
  test_algo_quad_distance_function_with_pos_float_float();
}

TESTMAIN(test_algo_quad_distance_function);
