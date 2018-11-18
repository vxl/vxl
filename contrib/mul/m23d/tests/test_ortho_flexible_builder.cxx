// This is mul/m23d/tests/test_ortho_flexible_builder.cxx
#include <iostream>
#include <cmath>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <m23d/m23d_make_ortho_projection.h>
#include <m23d/m23d_ortho_flexible_builder.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
#include <vnl/vnl_vector.h>
#include <m23d/m23d_rotation_matrix.h>

void test_projection_matrix1(const vnl_matrix<double>& P)
{
  unsigned ns=P.rows()/2;
  unsigned m=P.cols()/3-1;

  bool test_ok=true;
  std::cout<<"Testing projection matrix."<<std::endl;
  for (unsigned k=0;k<=m;++k)
  {
    // Test orthogonality of rows of each projection matrix
    for (unsigned i=0;i<ns;++i)
    {
      vnl_matrix<double> Pik = P.extract(2,3,2*i,3*k);
      vnl_matrix<double> PPt=Pik*Pik.transpose();
      if (std::fabs(PPt(0,1))>1e-5)
      {
        std::cout<<"View "<<i<<" mode "<<k<<" ) rows not orthogonal. "<<PPt(0,1)<<std::endl;
        test_ok=false;
      }
      if (std::fabs(PPt(0,0)-PPt(1,1))>1e-5)
      {
        std::cout<<"View "<<i<<" mode "<<k<<" ) rows don't scale equally. "<<PPt(0,0)-PPt(1,1)<<std::endl;
        test_ok=false;
      }
    }
  }
  if (test_ok)
    std::cout<<"Projection matrix passed the tests."<<std::endl;
}

void test_ortho_flexible_builder_on_rigid()
{
  std::cout<<"==== test m23d_ortho_flexible_builder on rigid ====="<<std::endl;

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
        C(0,c)=i*std::sqrt(1.0/3.0);
        C(1,c)=j*std::sqrt(1.0/3.0);
        C(2,c)=k*std::sqrt(1.0/3.0);
      }

  vnl_matrix<double> D=P*C;

  m23d_ortho_flexible_builder builder;
  builder.partial_reconstruct(D,0);

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

  std::cout<<"First projection:"<<std::endl
          <<P1.extract(2,3)<<std::endl;
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,1e-6);


  std::cout<<"Test scaled cube"<<std::endl;
  C*=2.5;
  D=P*C;
  builder.partial_reconstruct(D,0);
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-D).rms(),0,1e-6);
  TEST_NEAR("RMS error on P1",(P-P1).rms(),0,1e-6);
  TEST_NEAR("RMS error on P3D",(P3D-C).rms(),0,1e-6);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,1e-6);

  std::cout<<"Add some gaussian noise to the 2D observations."<<std::endl;
  for (unsigned i=0;i<D.rows();++i)
    for (unsigned j=0;j<D.cols();++j)
      D(i,j)+=0.1*r.normal64();

  builder.partial_reconstruct(D,0);
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-D).rms(),0,0.1);
  TEST_NEAR("RMS error on P1",(P-P1).rms(),0,0.1);
  TEST_NEAR("RMS error on P3D",(P3D-C).rms(),0,0.1);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,0.05);

  vnl_matrix<double> P_0 = P1.extract(2,3);
  std::cout<<"P0:"<<std::endl<<P_0<<std::endl
          <<"P0.P0'="<<std::endl<<P_0*P_0.transpose()<<std::endl;
  vnl_matrix<double> P_1 = P1.extract(2,3,2,0);
  std::cout<<"P0:"<<std::endl<<P_1<<std::endl
          <<"P0.P0'="<<std::endl<<P_1*P_1.transpose()<<std::endl;

  std::cout<<"=== Test refinement ==="<<std::endl;
  builder.refine();
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-D).rms(),0,0.1);
  TEST_NEAR("RMS error on P1",(P-P1).rms(),0,0.1);
  TEST_NEAR("RMS error on P3D",(P3D-C).rms(),0,0.1);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,0.05);


//  std::cout<<"Recovered 3D points: "<<std::endl<<P3D<<std::endl
//          <<"True 3D points: "<<std::endl<<C<<std::endl;
}

  // Generate cube test data, with one (symmetric) mode
vnl_matrix<double> make_cube_model()
{
  vnl_matrix<double> C(6,14,0.0);
  // First 3 rows are a cube
  unsigned c=0;
  for (int i=-1;i<=1;i+=2)
    for (int j=-1;j<=1;j+=2)
      for (int k=-1;k<=1;k+=2,++c)
      {
        C(0,c)=i*std::sqrt(1.0/3.0);
        C(1,c)=j*std::sqrt(1.0/3.0);
        C(2,c)=k*std::sqrt(1.0/3.0);
      }
  // Next three modes are points moving along x,y,z
  for (unsigned i=0;i<3;++i)
  {
    C(3+i,8+2*i)=-std::sqrt(0.5);
    C(3+i,9+2*i)= std::sqrt(0.5);
  }

  return C;
}

void test_ortho_flexible_builder_on_flexible()
{
  std::cout<<"==== test m23d_ortho_flexible_builder on flexible ====="<<std::endl;

  vnl_random r(35813);
  unsigned ns = 20;
  unsigned n_modes = 1;
  vnl_matrix<double> P=m23d_make_ortho_projection(r,ns,n_modes,true,true);

  vnl_matrix<double> C=make_cube_model();

  vnl_matrix<double> D=P*C;

  m23d_ortho_flexible_builder builder;
  builder.partial_reconstruct(D,1);

  TEST("Size of P1",(builder.projections().rows()==2*ns
                      && builder.projections().cols()==6),true);
  TEST("Size of P3D",(builder.shape_3d().rows()==6
                      && builder.shape_3d().cols()==C.cols()),true);
  vnl_matrix<double> P1  = builder.projections();
  vnl_matrix<double> P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-builder.centred_views()).rms(),0,1e-6);
  TEST_NEAR("RMS error on P1",(P-P1).rms(),0,0.01);
  TEST_NEAR("RMS error on P3D",(P3D-C).rms(),0,0.005);

  TEST_NEAR("RMS error on first mode of P1",(P-P1).extract(2*ns,3).rms(),0,1e-6);


  vnl_matrix<double> pure_P0(2,3);
  pure_P0(0,0)=1; pure_P0(0,1)=0; pure_P0(0,2)=0;
  pure_P0(1,0)=0; pure_P0(1,1)=1; pure_P0(1,2)=0;

  std::cout<<"Projection 0, mode 0:"<<std::endl
          <<P1.extract(2,3)<<std::endl;
  TEST_NEAR("Projection 0, mode 0 is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,0.02);

  std::cout<<"Test scaled cube"<<std::endl;
  C*=2.5;
  D=P*C;
  builder.partial_reconstruct(D,1);
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-builder.centred_views()).rms(),0,1e-6);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,1e-6);
  TEST_NEAR("RMS error on first mode of P1",(P-P1).extract(2*ns,3).rms(),0,1e-6);
  double e2a = (P-P1).extract(2*ns,3,0,3).rms();
  double e2b = (P+P1).extract(2*ns,3,0,3).rms();
  TEST_NEAR("RMS error on second mode of P1",std::min(e2a,e2b),0,0.01);

  TEST_NEAR("RMS error on first 3D basis",(P3D-C).extract(3,C.cols()).rms(),0,1e-6);

  test_projection_matrix1(P1);

  std::cout<<"Add some gaussian noise to the 2D observations."<<std::endl;
  for (unsigned i=0;i<D.rows();++i)
    for (unsigned j=0;j<D.cols();++j)
      D(i,j)+=0.02*r.normal64();

  builder.partial_reconstruct(D,1);
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-builder.centred_views()).rms(),0,0.02);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,0.01);
  TEST_NEAR("RMS error on first mode of P1",(P-P1).extract(2*ns,3).rms(),0,0.01);
  e2a = (P-P1).extract(2*ns,3,0,3).rms();
  e2b = (P+P1).extract(2*ns,3,0,3).rms();
  TEST_NEAR("RMS error on second mode of P1",std::min(e2a,e2b),0,0.02);

  TEST_NEAR("RMS error on first 3D basis",(P3D-C).extract(3,C.cols()).rms(),0,0.05);

  std::cout<<"=== Test refinement ==="<<std::endl;
  builder.refine();
  P1  = builder.projections();
  P3D = builder.shape_3d();
  TEST_NEAR("RMS error on projection",(P1*P3D-builder.centred_views()).rms(),0,0.02);
  TEST_NEAR("First projection is identity",
            (P1.extract(2,3)-pure_P0).rms(),0,0.01);
  TEST_NEAR("RMS error on first mode of P1",(P-P1).extract(2*ns,3).rms(),0,0.01);
  e2a = (P-P1).extract(2*ns,3,0,3).rms();
  e2b = (P+P1).extract(2*ns,3,0,3).rms();
  TEST_NEAR("RMS error on second mode of P1",std::min(e2a,e2b),0,0.02);

  TEST_NEAR("RMS error on first 3D basis",(P3D-C).extract(3,C.cols()).rms(),0,0.05);

#if 0
  std::cout<<"Projection 4, mode 0:"<<std::endl
          <<P1.extract(2,3,8,0)<<std::endl
          <<"Projection 4, mode 1:"<<std::endl
          <<P1.extract(2,3,8,3)<<std::endl
          <<"Projection 4 coeffs: "<<builder.coeffs().get_row(4)<<std::endl
          <<"Pure Projection 4, mode 0:"<<std::endl
          <<P.extract(2,3,8,0)<<std::endl
          <<"Pure Projection 4, mode 1:"<<std::endl
          <<P.extract(2,3,8,3)<<std::endl;
#endif // 0
  std::cout<<"Mean shape: "<<std::endl
          <<builder.mean_shape().transpose()<<std::endl;

  std::cout<<"== Use non-basis version of projection matrices =="<<std::endl;
  P=m23d_make_ortho_projection(r,ns,n_modes,false,false);
  C=make_cube_model();
  D=P*C;
  builder.reconstruct_with_first_as_basis(D,1);
  std::cout<<"Mean shape: "<<std::endl;
  std::cout<<builder.mean_shape().transpose()<<std::endl;

  vnl_matrix<double> MS=builder.mean_shape();
  // Check that each pair of the last points is approximately orthogonal
  for (unsigned i=8;i<14;i+=2)
  {
    vnl_vector<double> dp = MS.get_column(i)+MS.get_column(i+1);
    TEST_NEAR("Moving points symmetric about origin",dp.rms(),0.0,0.001);
  }

  std::cout<<"== Automatic selection of basis =="<<std::endl;
  P=m23d_make_ortho_projection(r,ns,n_modes,false,false);
  C=make_cube_model();
  D=P*C;
  builder.reconstruct(D,1);
  std::cout<<"Mean shape: "<<std::endl
          <<builder.mean_shape().transpose()<<std::endl;

  MS=builder.mean_shape();
  // Check that each pair of the last points is approximately orthogonal
  for (unsigned i=8;i<14;i+=2)
  {
    vnl_vector<double> dp = MS.get_column(i)+MS.get_column(i+1);
    TEST_NEAR("Moving points symmetric about origin",dp.rms(),0.0,0.001);
  }
}

void test_ortho_flexible_builder()
{
  test_ortho_flexible_builder_on_rigid();
  test_ortho_flexible_builder_on_flexible();
}


TESTMAIN(test_ortho_flexible_builder);
