// This is mul/msm/tests/test_shape_model.cxx
//=======================================================================
//
//  Copyright: (C) 2010 The University of Manchester
//
//=======================================================================
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test msm_shape_models

#include <msm/msm_shape_model_builder.h>
#include <msm/msm_shape_instance.h>
#include <msm/msm_similarity_aligner.h>
#include <msm/msm_no_limiter.h>

//: Create set of rectangles
void create_rectangles(std::vector<msm_points>& points)
{
  unsigned n=10;
  points.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    points[i].set_size(4);
    points[i].set_point(0, 0,0);
    points[i].set_point(1, 5+i,0);
    points[i].set_point(2, 0,5);
    points[i].set_point(3, 5+i,5);
  }
}


//=======================================================================

void test_shape_model()
{
  std::cout << "***********************\n"
           << " Testing msm_shape_model\n"
           << "***********************\n";

  std::vector<msm_points> shapes;
  create_rectangles(shapes);

  msm_shape_model_builder builder;
  builder.set_aligner(msm_similarity_aligner());
  builder.set_param_limiter(msm_no_limiter());
  builder.set_mode_choice(0,10,0.98);

  msm_shape_model shape_model;
  builder.build_model(shapes,shape_model);
  std::cout<<shape_model<<std::endl;

  TEST("N.Points",shape_model.size(),4);
  TEST("N.Modes",shape_model.n_modes(),1);
  TEST_NEAR("Mean length",shape_model.mean().magnitude(),1,1e-6);

  msm_shape_instance instance(shape_model);
  instance.fit_to_points(shapes[2]);
  std::cout<<"Points: "<<instance.points()<<std::endl;
  TEST("Fit is accurate",instance.points(),shapes[2]);
  TEST("N.Parameters",instance.params().size(),1);
  TEST("N.Pose",instance.pose().size(),4);

  vnl_vector<double> wt(4);
  for (unsigned i=0;i<4;++i) wt[i]=1+i;
  instance.fit_to_points_wt(shapes[3],wt);
  TEST("Wt.Fit is accurate",instance.points(),shapes[3]);

  std::vector<msm_wt_mat_2d> wt_mat(4);
  for (unsigned i=0;i<4;++i) wt_mat[i]*=(1+i);
  instance.fit_to_points_wt_mat(shapes[4],wt_mat);
  TEST("Wt.Mat.Fit is accurate",instance.points(),shapes[4]);

}

TESTMAIN(test_shape_model);
