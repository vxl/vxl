// This is mul/msm/tests/test_aligners.cxx
//=======================================================================
//
//  Copyright: (C) 2010 The University of Manchester
//
//=======================================================================
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test msm_aligners

#include <msm/msm_translation_aligner.h>
#include <msm/msm_zoom_aligner.h>
#include <msm/msm_rigid_aligner.h>
#include <msm/msm_similarity_aligner.h>
#include <vnl/vnl_vector.h>
#include <vcl_cmath.h>

void test_generic_aligner(const msm_aligner& aligner)
{
  vcl_cout<<"Testing "<<aligner.is_a()<<vcl_endl;

  // Set up a square
  msm_points points0(4);
  points0.set_point(0, 0,0);
  points0.set_point(1, 1,0);
  points0.set_point(2, 0,1);
  points0.set_point(3, 1,1);

  unsigned np = aligner.size();
  vnl_vector<double> pose0(np);
  for (unsigned i=0;i<np;++i) pose0[i]=1.17+i;

  // Apply transformation
  msm_points points1;
  aligner.apply_transform(points0,pose0,points1);
  TEST("Number of points",points0.size(),points1.size());

  // Compute and test the inverse
  vnl_vector<double> inv_pose0 = aligner.inverse(pose0);
  TEST("N.params",pose0.size(),inv_pose0.size());

  msm_points points2;
  aligner.apply_transform(points1,inv_pose0,points2);
  TEST("Inverse returns to original",points0==points2,true);

  // Estimate the optimal transformation
  vnl_vector<double> pose1;
  aligner.calc_transform(points0,points1,pose1);
  TEST_NEAR("calc_transform",(pose1-pose0).rms(),0.0,1e-6);

  pose1.fill(0.0);
  vnl_vector<double> wts(points0.size(),1.0);
  aligner.calc_transform_wt(points0,points1,wts,pose1);
  TEST_NEAR("calc_transform_wt",(pose1-pose0).rms(),0.0,1e-6);

  vcl_cout<<"Testing varying weights"<<vcl_endl;
  for (unsigned i=0;i<points0.size();++i) wts[i]*=(1+i);
  pose1.fill(0.0);
  aligner.calc_transform_wt(points0,points1,wts,pose1);
  TEST_NEAR("calc_transform_wt",(pose1-pose0).rms(),0.0,1e-6);

  pose1.fill(0.0);
  vcl_vector<msm_wt_mat_2d> wt_mat(points0.size());
  aligner.calc_transform_wt_mat(points0,points1,wt_mat,pose1);
  TEST_NEAR("calc_transform_wt_mat",(pose1-pose0).rms(),0.0,1e-6);

  vcl_cout<<"Testing varying weight mats"<<vcl_endl;
  for (unsigned i=0;i<points0.size();++i) wt_mat[i]*=(1+i);
  pose1.fill(0.0);
  aligner.calc_transform_wt_mat(points0,points1,wt_mat,pose1);
  TEST_NEAR("calc_transform_wt_mat",(pose1-pose0).rms(),0.0,1e-6);

  vcl_cout<<"Set one point incorrect, then ignore with zero wt"<<vcl_endl;
  points1.set_point(3, 77,83);
  wts[3]=0;
  wt_mat[3]*=0.0;

  aligner.calc_transform_wt(points0,points1,wts,pose1);
  TEST_NEAR("calc_transform_wt",(pose1-pose0).rms(),0.0,1e-6);

  aligner.calc_transform_wt_mat(points0,points1,wt_mat,pose1);
  TEST_NEAR("calc_transform_wt_mat",(pose1-pose0).rms(),0.0,1e-6);

  // Test composition
  for (unsigned i=0;i<np;++i) pose1[i]=1.32+0.5*i;
  vnl_vector<double> pose1_0 = aligner.compose(pose1,pose0);

  aligner.apply_transform(points0,pose0,points1);
  aligner.apply_transform(points1,pose1,points2);  // pts2 = pose1(pose0(pts0))
  msm_points points3;
  aligner.apply_transform(points0,pose1_0,points3);
  TEST("Composition",points2==points3,true);
}

//: Create set of shapes to align
//  Creates a quadrilateral
void create_shapes(vcl_vector<msm_points>& shapes)
{
  unsigned n=10;
  msm_points points(4);
  points.set_point(0, 0,0);
  points.set_point(1, 1,0);
  points.set_point(2, 0,1);
  points.set_point(3, 1,1);

  shapes.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    shapes[i]=points;
    shapes[i].set_point(3, 1.0/(1+i),1);
  }
}

//=======================================================================
void test_translation_aligner()
{
  msm_translation_aligner aligner;
  test_generic_aligner(aligner);

  // Set up two points
  msm_points points0(2);
  points0.set_point(0, 1,0);
  points0.set_point(1, 0,1);

  msm_points points1(2);
  points1.set_point(0, 3,0);
  points1.set_point(1, 0,2);

  vcl_vector<msm_wt_mat_2d> wt_mat(2);
  wt_mat[0].set_axes(1,0, 10, 0);   // Constrain along x
  wt_mat[1].set_axes(0,1, 10, 0);   // Constrain along y.

  vnl_vector<double> pose1;
  aligner.calc_transform_wt_mat(points0,points1,wt_mat,pose1);
  TEST_NEAR("wt_mat x trans",pose1[0],2,1e-6);
  TEST_NEAR("wt_mat y trans",pose1[1],1,1e-6);
}

//=======================================================================
void test_zoom_aligner()
{
  msm_zoom_aligner aligner;
  test_generic_aligner(aligner);

  // Set up three points
  msm_points points0(3);
  points0.set_point(0, 0,0);
  points0.set_point(1, 1,0);
  points0.set_point(2, 0,1);

  msm_points points1(3);
  points1.set_point(0, 2,0);
  points1.set_point(1, 4,0);
  points1.set_point(2, 0,3);

  vcl_vector<msm_wt_mat_2d> wt_mat(3);
  wt_mat[0].set_axes(1,0, 10, 0);   // Constrain along x
  wt_mat[1].set_axes(1,0, 10, 0);   // Constrain along x.
  wt_mat[2].set_axes(0,1, 10, 0);   // Constrain along y.

  vnl_vector<double> pose1;
  aligner.calc_transform_wt_mat(points0,points1,wt_mat,pose1);
  TEST_NEAR("wt_mat scale",pose1[0],vcl_log(2.0),1e-6);
  TEST_NEAR("wt_mat x trans",pose1[1],2,1e-6);
  TEST_NEAR("wt_mat y trans",pose1[2],1,1e-6);

  vcl_vector<msm_points> shapes;
  create_shapes(shapes);

  msm_points ref_mean_shape;
  vcl_vector<vnl_vector<double> > poses;
  vnl_vector<double> average_pose;
  aligner.align_set(shapes,ref_mean_shape,poses,average_pose);

  vcl_cout<<"Aligned mean: "<<ref_mean_shape<<vcl_endl;

  // Test orthogonality of aligned shape
  ref_mean_shape=shapes[0];
  aligner.normalise_shape(ref_mean_shape);
  vcl_cout<<"mean scale: "<<ref_mean_shape.scale()<<vcl_endl;
  aligner.calc_transform_from_ref(ref_mean_shape,shapes[3],pose1);
  msm_points points2;
  aligner.apply_transform(shapes[3],aligner.inverse(pose1),points2);
  vcl_cout<<"Dot1: "<<dot_product(ref_mean_shape.vector(),
                                  points2.vector())<<vcl_endl
          <<"mean scale: "<<points2.scale()<<vcl_endl;
}

//=======================================================================
void test_rigid_aligner()
{
  vcl_cout<<"==============================="<<vcl_endl;
  msm_rigid_aligner aligner;
  test_generic_aligner(aligner);
/*
  // Set up three points
  msm_points points0(3);
  points0.set_point(0, 0,0);
  points0.set_point(1, 1,0);
  points0.set_point(2, 0,1);

  msm_points points1(3);
  points1.set_point(0, 2,0);
  points1.set_point(1, 4,0);
  points1.set_point(2, 0,3);

  vcl_vector<msm_wt_mat_2d> wt_mat(3);
  wt_mat[0].set_axes(1,0, 10, 0);   // Constrain along x
  wt_mat[1].set_axes(1,0, 10, 0);   // Constrain along x.
  wt_mat[2].set_axes(0,1, 10, 0);   // Constrain along y.

  vnl_vector<double> pose1;
  aligner.calc_transform_wt_mat(points0,points1,wt_mat,pose1);
  TEST_NEAR("wt_mat orientation",pose1[0],vcl_log(2.0),1e-6);
  TEST_NEAR("wt_mat x trans",pose1[1],2,1e-6);
  TEST_NEAR("wt_mat y trans",pose1[2],1,1e-6);
*/
  vcl_vector<msm_points> shapes;
  create_shapes(shapes);

  msm_points ref_mean_shape;
  vcl_vector<vnl_vector<double> > poses;
  vnl_vector<double> average_pose;
  aligner.align_set(shapes,ref_mean_shape,poses,average_pose);

  vcl_cout<<"Aligned mean: "<<ref_mean_shape<<vcl_endl;
}


double msm_wt_mat_diff(const msm_points& pts0, const msm_points& pts1,
                       const vcl_vector<msm_wt_mat_2d>& wt)
{
  double error=0.0;
  for (unsigned i=0;i<pts0.size();++i)
  {
    vgl_vector_2d<double> dp=pts0[i]-pts1[i];
    error += wt[i].xWx(dp.x(),dp.y());
  }
  return error;
}

//=======================================================================
void test_similarity_aligner()
{
  msm_similarity_aligner aligner;
  test_generic_aligner(aligner);

  // Set up three points
  msm_points points0(3);
  points0.set_point(0, 0,0);
  points0.set_point(1, 1,0);
  points0.set_point(2, 0,1);

  msm_points points1(3);
  points1.set_point(0, 2,2);
  points1.set_point(1, 2,0);
  points1.set_point(2, 0,2);

  vcl_vector<msm_wt_mat_2d> wt_mat(3);
  wt_mat[0].set_axes(1,0, 1,1);
  wt_mat[1].set_axes(1,1, 0, 10);   // Constrain along (1,1)
  wt_mat[2].set_axes(1,1, 0, 10);   // Constrain along (1,1)

  vnl_vector<double> pose1;
  msm_points points2;
  aligner.calc_transform_wt_mat(points0,points1,wt_mat,pose1);
  TEST_NEAR("wt_mat a",pose1[0]+1,2,1e-6);
  TEST_NEAR("wt_mat b",pose1[1],0,1e-6);
  TEST_NEAR("wt_mat x trans",pose1[2],2,1e-6);
  TEST_NEAR("wt_mat y trans",pose1[3],2,1e-6);
  aligner.apply_transform(points0,pose1,points2);
  vcl_cout<<"Transformed points: "<<points2<<vcl_endl;

  points1.set_point(0, 3,1);
  points1.set_point(1, 2,0);
  points1.set_point(2, 0,2);
  wt_mat[0].set_axes(1,0, 100,  100);
  wt_mat[1].set_axes(1,1, 0, 100);   // Constrain to lie along (1,1)
  wt_mat[2].set_axes(1,1, 0, 100);   // Constrain to lie along (1,1)
  vcl_cout<<points0<<vcl_endl
          <<points1<<vcl_endl;

  aligner.calc_transform_wt_mat(points0,points1,wt_mat,pose1);
  TEST_NEAR("wt_mat a",pose1[0]+1,2,1e-6);
  TEST_NEAR("wt_mat b",pose1[1],2,1e-6);
  TEST_NEAR("wt_mat x trans",pose1[2],3,1e-6);
  TEST_NEAR("wt_mat y trans",pose1[3],1,1e-6);
  aligner.apply_transform(points0,pose1,points2);
  vcl_cout<<"Transformed points: "<<points2<<vcl_endl;

  double error=msm_wt_mat_diff(points1,points2,wt_mat);
  vcl_cout<<"Wtd Error: "<<error<<vcl_endl;

  vcl_vector<msm_points> shapes;
  create_shapes(shapes);

  msm_points ref_mean_shape;
  vcl_vector<vnl_vector<double> > poses;
  vnl_vector<double> average_pose;
  aligner.align_set(shapes,ref_mean_shape,poses,average_pose);

  vcl_cout<<"Aligned mean: "<<ref_mean_shape<<vcl_endl;

  // Test orthogonality of aligned shape
  ref_mean_shape=shapes[0];
  aligner.normalise_shape(ref_mean_shape);
  aligner.calc_transform_from_ref(ref_mean_shape,shapes[3],pose1);
  aligner.apply_transform(shapes[3],aligner.inverse(pose1),points2);
  TEST_NEAR("Aligned is orthogonal",
            dot_product(ref_mean_shape.vector(),
                        points2.vector()),1.0,1e-6);
}

//=======================================================================

void test_aligners()
{
  vcl_cout << "**********************\n"
           << " Testing msm_aligners\n"
           << "**********************\n";

  test_translation_aligner();
  test_zoom_aligner();
  test_similarity_aligner();
  test_rigid_aligner();
}

TESTMAIN(test_aligners);
