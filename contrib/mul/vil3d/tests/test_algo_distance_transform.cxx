#include <testlib/testlib_test.h>
// not used? #include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vil3d/algo/vil3d_distance_transform.h>
#include <vil3d/vil3d_print.h>


void test_signed_distance_transform()
{
  vcl_cout << "*****************************************\n"
           << " Testing vil3d_signed_distance_transform\n"
           << "*****************************************\n";

  vil3d_image_view<float> image;
  image.set_size(5,5,5);

  vil3d_image_view<bool> mask;
  mask.set_size(5,5,5);

  // put single mask in centre
  {
    mask.fill(0);
    mask(2,2,2)=1;

    float li=5.0f;
    float lj=7.0f;
    float lk=9.0f;

    vil3d_signed_distance_transform(mask,image,1000.0,li,lj,lk);

    double li2=li*li;
    double lj2=lj*lj;
    double lk2=lk*lk;

    double tol=1e-6;
    TEST_NEAR("26 connected distance correct for (0,0,0)",image(0,0,0),2*vcl_sqrt(li2+lj2+lk2),tol);
    TEST_NEAR("26 connected distance correct for (4,4,4)",image(4,4,4),2*vcl_sqrt(li2+lj2+lk2),tol);
    TEST_NEAR("26 connected distance correct for (2,2,0)",image(2,2,0),2*lk,tol);
    TEST_NEAR("26 connected distance correct for (2,0,2)",image(2,0,2),2*lj,tol);
    TEST_NEAR("26 connected distance correct for (0,2,2)",image(0,2,2),2*li,tol);
    TEST_NEAR("26 connected distance correct for (2,2,4)",image(2,2,4),2*lk,tol);
    TEST_NEAR("26 connected distance correct for (2,4,2)",image(2,4,2),2*lj,tol);
    TEST_NEAR("26 connected distance correct for (4,2,2)",image(4,2,2),2*li,tol);
  }

  // put 2*2*2 block of mask points in corner
  {
    mask.fill(0);
    mask(0,0,0)=1;
    mask(1,0,0)=1;
    mask(0,1,0)=1;
    mask(0,0,1)=1;
    mask(1,1,0)=1;
    mask(1,0,1)=1;
    mask(0,1,1)=1;
    mask(1,1,1)=1;

    float li=3.0f;
    float lj=4.0f;
    float lk=5.0f;
    float min_link=vcl_min(li,lj);
    min_link=vcl_min(min_link,lk);

    vil3d_signed_distance_transform(mask,image,1000.0,li,lj,lk);

    // The signed distance transform returns
    // -1.0 *  the 26 connected distance to the non-mask pixels inside the mask
    // 1.0 *  the 26 connected distance to the mask pixels outside the mask
    double tol=1e-6;
    TEST_NEAR("26 connected distance correct for (0,0,0)",image(0,0,0),-2.0*min_link,tol);
    TEST_NEAR("26 connected distance correct for (1,1,1)",image(1,1,1),-1.0*min_link,tol);
    TEST_NEAR("26 connected distance correct for (1,0,0)",image(1,0,0),-1.0*li,tol);
    TEST_NEAR("26 connected distance correct for (0,1,0)",image(0,1,0),-1.0*lj,tol);
    TEST_NEAR("26 connected distance correct for (0,0,1)",image(0,0,1),-1.0*lk,tol);
    TEST_NEAR("26 connected distance correct for (1,1,0)",image(1,1,0),-1.0*vcl_min(li,lj),tol);
    TEST_NEAR("26 connected distance correct for (0,1,1)",image(0,1,1),-1.0*vcl_min(lj,lk),tol);
    TEST_NEAR("26 connected distance correct for (1,0,1)",image(1,0,1),-1.0*vcl_min(li,lk),tol);
  }
}

MAIN( test_algo_distance_transform )
{
  START( "test_distance_transform" );
  test_signed_distance_transform();
  SUMMARY();
}
