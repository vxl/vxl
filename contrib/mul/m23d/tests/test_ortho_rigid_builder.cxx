// This is mul/m23d/tests/test_ortho_rigid_builder.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <m23d/m23d_make_ortho_projection.h>
#include <m23d/m23d_ortho_rigid_builder.h>
#include <vcl_cmath.h>
#include <vnl/vnl_random.h>

void test_ortho_rigid_builder()
{
  vcl_cout<<"==== test m23d_ortho_rigid_builder ====="<<vcl_endl;

  vnl_random r(35813);
  unsigned ns = 20;
  vnl_matrix<double> P=m23d_make_ortho_projection(r,ns,0,true,true);

  // Generate cube test data
  vnl_matrix<double> C(3,8);
  unsigned c=0;
  for (int i=-1;i<=1;i+=2)
    for (int j=-1;j<=1;j+=2)
      for (int k=-1;k<=1;k+=2,++c)
      {
        C(0,c)=i*vcl_sqrt(1.0/3.0);
        C(1,c)=j*vcl_sqrt(1.0/3.0);
        C(2,c)=k*vcl_sqrt(1.0/3.0);
      }

  vnl_matrix<double> D=P*C;

  m23d_ortho_rigid_builder builder;
  builder.reconstruct(D);

  TEST("Size of P1",(builder.projections().rows()==2*ns
                      && builder.projections().cols()==3),true);
  TEST("Size of P3D",(builder.shape_3d().rows()==3
                      && builder.shape_3d().cols()==8),true);
  vnl_matrix<double> P1  = builder.projections();
  vnl_matrix<double> P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-D).rms(),0,1e-6);
  TEST_NEAR("RMS error on P1",(P-P1).rms(),0,1e-6);
  TEST_NEAR("RMS error on P3D",(P3D-C).rms(),0,1e-6);

  vnl_matrix<double> pure_P0(2,3);
  pure_P0(0,0)=1; pure_P0(0,1)=0; pure_P0(0,2)=0;
  pure_P0(1,0)=0; pure_P0(1,1)=1; pure_P0(1,2)=0;

  vcl_cout<<"First projection:"<<vcl_endl
          <<P1.extract(2,3)<<vcl_endl;
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,1e-6);


  vcl_cout<<"Test scaled cube"<<vcl_endl;
  C*=2.5;
  D=P*C;
  builder.reconstruct(D);
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-D).rms(),0,1e-6);
  TEST_NEAR("RMS error on P1",(P-P1).rms(),0,1e-6);
  TEST_NEAR("RMS error on P3D",(P3D-C).rms(),0,1e-6);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,1e-6);

  vcl_cout<<"Add some gaussian noise to the 2D observations."<<vcl_endl;
  for (unsigned i=0;i<D.rows();++i)
    for (unsigned j=0;j<D.cols();++j)
      D(i,j)+=0.1*r.normal64();

  builder.reconstruct(D);
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-D).rms(),0,0.1);
  TEST_NEAR("RMS error on P1",(P-P1).rms(),0,0.1);
  TEST_NEAR("RMS error on P3D",(P3D-C).rms(),0,0.1);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,0.05);

  vnl_matrix<double> P_0 = P1.extract(2,3);
  vcl_cout<<"P0:"<<vcl_endl<<P_0<<vcl_endl
          <<"P0.P0'="<<vcl_endl<<P_0*P_0.transpose()<<vcl_endl;
  vnl_matrix<double> P_1 = P1.extract(2,3,2,0);
  vcl_cout<<"P0:"<<vcl_endl<<P_1<<vcl_endl
          <<"P0.P0'="<<vcl_endl<<P_1*P_1.transpose()<<vcl_endl;

  vcl_cout<<"=== Test refinement ==="<<vcl_endl;
  builder.refine();
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-D).rms(),0,0.1);
  TEST_NEAR("RMS error on P1",(P-P1).rms(),0,0.1);
  TEST_NEAR("RMS error on P3D",(P3D-C).rms(),0,0.1);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,0.05);


//  vcl_cout<<"Recovered 3D points: "<<vcl_endl<<P3D<<vcl_endl
//          <<"True 3D points: "<<vcl_endl<<C<<vcl_endl;
}

TESTMAIN(test_ortho_rigid_builder);
