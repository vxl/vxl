// This is mul/vimt/tests/test_sample_grid_bilin.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vimt/vimt_sample_grid_bilin.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

static void test_sample_grid_bilin_byte_affine()
{
  vcl_cout<<"Testing up to affine mappings\n";
  vimt_image_2d_of<vxl_byte> image0;
  image0.image().set_size(10,10);

  vcl_cout<<"Testing one plane image\n";

  for (unsigned int j=0;j<image0.image().nj();++j)
     for (unsigned int i=0;i<image0.image().ni();++i)
       image0.image()(i,j) = i+j*10;

  vgl_point_2d<double> p0(5.0,5.0);
  vgl_vector_2d<double> u(1.0,0.0), v(0.0,1.0);
  vnl_vector<double> vec;

  vcl_cout<<"Fully in image\n";
  vimt_sample_grid_bilin(vec,image0,p0,u,v,4,3);
  TEST("Vector length",vec.size(),12);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],75,1e-6);
  TEST_NEAR("Last value",vec[11],78,1e-6);

  vimt_sample_grid_bilin(vec,image0,p0,v,u,4,3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],57,1e-6);
  TEST_NEAR("Last value",vec[11],87,1e-6);

  vcl_cout<<"Beyond edge of image\n";
  vimt_sample_grid_bilin(vec,image0,vgl_point_2d<double>(8.0,5.0),u,v,4,3);
  TEST_NEAR("First value",vec[0],58,1e-6);
  TEST_NEAR("Last value",vec[11],0,1e-6);

  vcl_cout<<"Testing three plane image\n";

  image0.image().set_size(10,10,2);
  for (unsigned int j=0;j<image0.image().nj();++j)
    for (unsigned int i=0;i<image0.image().ni();++i)
      for (unsigned int p=0;p<2;++p)
       image0.image()(i,j,p) = i+j*10+p*100;

  vnl_vector<double> vec2;

  vcl_cout<<"Fully in image\n";
  vimt_sample_grid_bilin(vec2,image0,p0,u,v,4,3);
  TEST("Vector length",vec2.size(),24);
  TEST_NEAR("First value",vec2[0],55,1e-6);
  TEST_NEAR("Second value",vec2[1],155,1e-6);
  TEST_NEAR("Third value",vec2[2],65,1e-6);
  TEST_NEAR("Last value",vec2[23],178,1e-6);

  vcl_cout<<"Beyond edge of image\n";
  vimt_sample_grid_bilin(vec2,image0,vgl_point_2d<double>(8.0,5.0),u,v,4,3);
  TEST_NEAR("First value",vec2[0],58,1e-6);
  TEST_NEAR("Last value",vec2[23],0,1e-6);
}

static void test_sample_grid_bilin_byte_projective()
{
  vcl_cout<<"Testing projective mappings\n";
  vimt_image_2d_of<vxl_byte> image0;
  image0.image().set_size(10,10);

  vimt_transform_2d trans;
  // Create projective identity matrix
  vnl_matrix<double> P(3,3);
  P.fill(0.0);
  P(0,0)=P(1,1)=P(2,2)=1.0;
  trans.set_projective(P);

  // If following line fails then set_projective has been tweaked to
  // select simplest mapping, and the later tests are irrelevant.
  TEST("Projective",trans.form(),vimt_transform_2d::Projective);
  image0.set_world2im(trans);


  vcl_cout<<"Testing one plane image\n";

  for (unsigned int j=0;j<image0.image().nj();++j)
     for (unsigned int i=0;i<image0.image().ni();++i)
       image0.image()(i,j) = i+j*10;

  vgl_point_2d<double> p0(5.0,5.0);
  vgl_vector_2d<double> u(1.0,0.0), v(0.0,1.0);
  vnl_vector<double> vec;

  vcl_cout<<"Fully in image\n";
  vimt_sample_grid_bilin(vec,image0,p0,u,v,4,3);
  TEST("Vector length",vec.size(),12);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],75,1e-6);
  TEST_NEAR("Last value",vec[11],78,1e-6);

  vimt_sample_grid_bilin(vec,image0,p0,v,u,4,3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Third value",vec[2],57,1e-6);
  TEST_NEAR("Last value",vec[11],87,1e-6);

  vcl_cout<<"Beyond edge of image\n";
  vimt_sample_grid_bilin(vec,image0,vgl_point_2d<double>(8.0,5.0),u,v,4,3);
  TEST_NEAR("First value",vec[0],58,1e-6);
  TEST_NEAR("Last value",vec[11],0,1e-6);

  vcl_cout<<"Testing three plane image\n";

  image0.image().set_size(10,10,2);
  for (unsigned int j=0;j<image0.image().nj();++j)
    for (unsigned int i=0;i<image0.image().ni();++i)
      for (unsigned int p=0;p<2;++p)
       image0.image()(i,j,p) = i+j*10+p*100;

  vnl_vector<double> vec2;

  vcl_cout<<"Fully in image\n";
  vimt_sample_grid_bilin(vec2,image0,p0,u,v,4,3);
  TEST("Vector length",vec2.size(),24);
  TEST_NEAR("First value",vec2[0],55,1e-6);
  TEST_NEAR("Second value",vec2[1],155,1e-6);
  TEST_NEAR("Third value",vec2[2],65,1e-6);
  TEST_NEAR("Last value",vec2[23],178,1e-6);

  vcl_cout<<"Beyond edge of image\n";
  vimt_sample_grid_bilin(vec2,image0,vgl_point_2d<double>(8.0,5.0),u,v,4,3);
  TEST_NEAR("First value",vec2[0],58,1e-6);
  TEST_NEAR("Last value",vec2[23],0,1e-6);
}

static void test_sample_grid_bilin_byte()
{
  vcl_cout << "********************************\n"
           << " Testing vimt_sample_grid_bilin\n"
           << "********************************\n";

  test_sample_grid_bilin_byte_affine();
  test_sample_grid_bilin_byte_projective();
}

static void test_sample_grid_bilin()
{
  test_sample_grid_bilin_byte();
}

TESTMAIN(test_sample_grid_bilin);
