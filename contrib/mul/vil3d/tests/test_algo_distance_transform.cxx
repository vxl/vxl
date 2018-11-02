#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/algo/vil3d_distance_transform.h>
#include <vil3d/vil3d_print.h>
#include <vil/vil_rgb.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_distance.h>

void test_signed_distance_transform()
{
  std::cout << "*****************************************\n"
           << " Testing vil3d_signed_distance_transform\n"
           << "*****************************************\n";

  vil3d_image_view<float> image;
  image.set_size(5,5,5);

  vil3d_image_view<bool> mask;
  mask.set_size(5,5,5);

  // put single mask in centre
  {
    mask.fill(false);
    mask(2,2,2)=true;

    float li=5.0f;
    float lj=7.0f;
    float lk=9.0f;

    vil3d_signed_distance_transform(mask,image,1000.0,li,lj,lk);

    double li2=li*li;
    double lj2=lj*lj;
    double lk2=lk*lk;

    double tol=1e-6;
    TEST_NEAR("26 connected distance correct for (0,0,0)",image(0,0,0),2*std::sqrt(li2+lj2+lk2),tol);
    TEST_NEAR("26 connected distance correct for (4,4,4)",image(4,4,4),2*std::sqrt(li2+lj2+lk2),tol);
    TEST_NEAR("26 connected distance correct for (2,2,0)",image(2,2,0),2*lk,tol);
    TEST_NEAR("26 connected distance correct for (2,0,2)",image(2,0,2),2*lj,tol);
    TEST_NEAR("26 connected distance correct for (0,2,2)",image(0,2,2),2*li,tol);
    TEST_NEAR("26 connected distance correct for (2,2,4)",image(2,2,4),2*lk,tol);
    TEST_NEAR("26 connected distance correct for (2,4,2)",image(2,4,2),2*lj,tol);
    TEST_NEAR("26 connected distance correct for (4,2,2)",image(4,2,2),2*li,tol);
  }

  // put 2*2*2 block of mask points in corner
  {
    mask.fill(false);
    mask(0,0,0)=true;
    mask(1,0,0)=true;
    mask(0,1,0)=true;
    mask(0,0,1)=true;
    mask(1,1,0)=true;
    mask(1,0,1)=true;
    mask(0,1,1)=true;
    mask(1,1,1)=true;

    float li=3.0f;
    float lj=4.0f;
    float lk=5.0f;
    float min_link=std::min(li,lj);
    min_link=std::min(min_link,lk);

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
    TEST_NEAR("26 connected distance correct for (1,1,0)",image(1,1,0),-1.0*std::min(li,lj),tol);
    TEST_NEAR("26 connected distance correct for (0,1,1)",image(0,1,1),-1.0*std::min(lj,lk),tol);
    TEST_NEAR("26 connected distance correct for (1,0,1)",image(1,0,1),-1.0*std::min(li,lk),tol);
  }
}

void test_distance_transform()
{
  std::cout << "**********************************\n"
           << " Testing vil3d_distance_transform\n"
           << "**********************************\n";

  vil3d_image_view<float> image, gt_image;
  const unsigned int dim=10, b1=2, b2=7;
  image.set_size(dim,dim,dim);
  gt_image.set_size(dim,dim,dim);

  vil3d_image_view<vil_rgb<float> > orients;
  // ground truth orientations
  vil3d_image_view<vil_rgb<float> > gt_orients;
  orients.set_size(dim,dim,dim);
  gt_orients.set_size(dim,dim,dim);

  image.fill(std::numeric_limits<float>::max());
  gt_image.fill(std::numeric_limits<float>::max());
  std::vector<int> planes;
  planes.push_back(3);
  planes.push_back(7);

  // create two rectangular planes
  for (int plane : planes) {
    for (unsigned i=b1; i<=b2; ++i) {
      for (unsigned j=b1; j<=b2; ++j) {
        image(i,j,plane)=0;
      }
    }
  }

  for (unsigned k=0; k<dim; ++k) {
    for (unsigned j=0; j<dim; ++j) {
      for (unsigned i=0; i<dim; ++i) {
        double min = std::numeric_limits<double>::max();
        vgl_point_3d<float> v1((float)(i),(float)(j),(float)(k));

        // find the distance to the planes
        for (int & plane : planes) {
          for (unsigned i1=b1; i1<=b2; ++i1) {
            for (unsigned j1=b1; j1<=b2; ++j1) {
              vgl_point_3d<float> v2((float)(i1),(float)(j1),(float)plane);
              double diff=vgl_distance(v1,v2);
              if (diff < min) {
                min=diff;
                vgl_vector_3d<float> d=v2-v1;
                gt_image(i,j,k)=(float)diff;
                gt_orients(i,j,k)=vil_rgb<float>(d.x(),d.y(),d.z());
              }
            }
          }
        }
      }
    }
  }

  orients.fill(vil_rgb<float>(0,0,0));
  vil3d_distance_transform_with_dir(image,orients,1,1,1);

  bool result=true;
  for (unsigned k=0; k<dim; ++k){
    for (unsigned j=0; j<dim; ++j) {
      for (unsigned i=0; i<dim; ++i) {
        if (orients(i,j,k).R()!=gt_orients(i,j,k).R() ||
            orients(i,j,k).G()!=gt_orients(i,j,k).G() ||
            orients(i,j,k).B()!=gt_orients(i,j,k).B())
          result=false;
      }
    }
  }
  TEST_EQUAL("The vectors are equal to ground truth vectors", result, true);
}

static void test_algo_distance_transform()
{
  test_signed_distance_transform();
  test_distance_transform();
}

TESTMAIN(test_algo_distance_transform);
