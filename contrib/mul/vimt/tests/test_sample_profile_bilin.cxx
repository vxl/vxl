// This is mul/vimt/tests/test_sample_profile_bilin.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vimt/vimt_sample_profile_bilin.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

static void test_sample_profile_bilin_byte_affine()
{
  vcl_cout << "***********************************\n"
           << " Testing vimt_sample_profile_bilin\n"
           << "***********************************\n";

  vcl_cout<<"Testing up to affine mappings\n";

  vcl_cout<< "One plane image\n";
  vimt_image_2d_of<vxl_byte> image1;
  image1.image().set_size(10,10);

  for (unsigned int j=0;j<image1.image().nj();++j)
     for (unsigned int i=0;i<image1.image().ni();++i)
       image1.image()(i,j) = i+j*10;

  vgl_point_2d<double> p0(5.0, 5.0);
  vgl_vector_2d<double> u(1.0,0.0);
  vnl_vector<double> vec;

  vimt_sample_profile_bilin(vec,image1,p0,u,3);
  TEST("Vector size",vec.size(),3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Last value",vec[2],57,1e-6);

  vnl_vector<double> vec2;
  vimt_sample_profile_bilin(vec2,image1,p0,u,8);
  TEST("Vector size",vec2.size(),8);
  TEST_NEAR("First value",vec2[0],55,1e-6);
  TEST_NEAR("Last value (outside image)",vec2[7],0,1e-6);

  vcl_cout<< "Three plane image (float)\n";
  vimt_image_2d_of<float> image3;
  image3.image().set_size(10,10,3);

  for (unsigned int j=0;j<image3.image().nj();++j)
     for (unsigned int i=0;i<image3.image().ni();++i)
       for (unsigned int k=0;k<3;++k)
         image3.image()(i,j,k) = 0.1f*i+j+10*k;

  vnl_vector<double> vec3;

  vimt_sample_profile_bilin(vec3,image3,p0,u,3);
  TEST("Vector size",vec3.size(),9);
  TEST_NEAR("First value",vec3[0],5.5f,1e-6);
  TEST_NEAR("Second value",vec3[1],15.5f,1e-6);
  TEST_NEAR("Last value",vec3[8],25.7f,1e-5);

  vnl_vector<double> vec4;
  vimt_sample_profile_bilin(vec4,image3,p0,u,8);
  TEST("Vector size",vec4.size(),24);
  TEST_NEAR("First value",vec4[0],5.5f,1e-6);
  TEST_NEAR("Last value (outside image)",vec4[23],0.f,1e-6);
}

static void test_sample_profile_bilin_byte_projective()
{
  vcl_cout << "***********************************\n"
           << " Testing vimt_sample_profile_bilin\n"
           << "***********************************\n";

  vcl_cout<<"Testing projective mappings\n";
  vimt_image_2d_of<vxl_byte> image1;
  image1.image().set_size(10,10);

  vimt_transform_2d trans;
  // Create projective identity matrix
  vnl_matrix<double> P(3,3);
  P.fill(0.0);
  P(0,0)=P(1,1)=P(2,2)=1.0;
  trans.set_projective(P);

  // If following line fails then set_projective has been tweaked to
  // select simplest mapping, and the later tests are irrelevant.
  TEST("Projective",trans.form(),vimt_transform_2d::Projective);
  image1.set_world2im(trans);

  vcl_cout<< "One plane image\n";

  for (unsigned int j=0;j<image1.image().nj();++j)
     for (unsigned int i=0;i<image1.image().ni();++i)
       image1.image()(i,j) = i+j*10;

  vgl_point_2d<double> p0(5.0, 5.0);
  vgl_vector_2d<double> u(1.0,0.0);
  vnl_vector<double> vec;

  vimt_sample_profile_bilin(vec,image1,p0,u,3);
  TEST("Vector size",vec.size(),3);
  TEST_NEAR("First value",vec[0],55,1e-6);
  TEST_NEAR("Last value",vec[2],57,1e-6);

  vnl_vector<double> vec2;
  vimt_sample_profile_bilin(vec2,image1,p0,u,8);
  TEST("Vector size",vec2.size(),8);
  TEST_NEAR("First value",vec2[0],55,1e-6);
  TEST_NEAR("Last value (outside image)",vec2[7],0,1e-6);

  vcl_cout<< "Three plane image (float)\n";
  vimt_image_2d_of<float> image3;
  image3.image().set_size(10,10,3);

  for (unsigned int j=0;j<image3.image().nj();++j)
     for (unsigned int i=0;i<image3.image().ni();++i)
       for (unsigned int k=0;k<3;++k)
         image3.image()(i,j,k) = 0.1f*i+j+10*k;

  vnl_vector<double> vec3;

  vimt_sample_profile_bilin(vec3,image3,p0,u,3);
  TEST("Vector size",vec3.size(),9);
  TEST_NEAR("First value",vec3[0],5.5f,1e-6);
  TEST_NEAR("Second value",vec3[1],15.5f,1e-6);
  TEST_NEAR("Last value",vec3[8],25.7f,1e-5);

  vnl_vector<double> vec4;
  vimt_sample_profile_bilin(vec4,image3,p0,u,8);
  TEST("Vector size",vec4.size(),24);
  TEST_NEAR("First value",vec4[0],5.5f,1e-6);
  TEST_NEAR("Last value (outside image)",vec4[23],0.f,1e-6);
}

static void test_sample_profile_bilin()
{
  test_sample_profile_bilin_byte_affine();
  test_sample_profile_bilin_byte_projective();
}

TESTMAIN(test_sample_profile_bilin);
