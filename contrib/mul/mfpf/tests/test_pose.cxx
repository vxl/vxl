// This is mul/mfpf/tests/test_pose.cxx
//=======================================================================
//
//  Copyright: (C) 2007 The University of Manchester
//
//=======================================================================
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mfpf_pose

#include <vcl_iostream.h>
#include <mfpf/mfpf_pose.h>


void test_pose()
{
  vcl_cout << "***********************\n"
           << " Testing mfpf_pose\n"
           << "***********************\n";

  vgl_point_2d<double> p0(7,5);
  vgl_vector_2d<double> u0(1.2,-0.3);
  mfpf_pose pose0(p0,u0);

  vgl_point_2d<double> p1(4,5);
  vgl_point_2d<double> q1=pose0(p1);
  vgl_point_2d<double> q1b=pose0(p1.x(),p1.y());

  TEST("pose(p)==pose(x,y)",(q1-q1b).length()<1e-6,true);

  vgl_point_2d<double> r1=pose0.apply_inverse(q1);
  TEST("apply_inverse(p)",(r1-p1).length()<1e-6,true);

  vgl_point_2d<double> r1a=pose0.apply_inverse(q1.x(),q1.y());
  TEST("apply_inverse(x,y)",(r1a-p1).length()<1e-6,true);

  mfpf_pose pose1(8,3.2, 1.1,0.2);

  mfpf_pose rpose = pose0.rel_pose(pose1);
  
  TEST("rel_pose",(pose1(p1) - pose0(rpose(p1))).length()<1e-6,true);

  mfpf_pose inv_pose = pose0.inverse();
  vcl_cout<<"Pose0:   "<<pose0<<vcl_endl;
  vcl_cout<<"Inverse: "<<inv_pose<<vcl_endl;
  vcl_cout<<"Inverse of inverse: "<<inv_pose.inverse()<<vcl_endl;
  mfpf_pose pose2=inv_pose.inverse();
  TEST("Inverse of inverse == Identity",pose0,pose2);

  TEST("inverse(p)==(0,0)",
      (inv_pose(pose0.p())-vgl_point_2d<double>(0,0)).length()<1e-6,true);

  TEST("Inverse consistent",
       (pose0.apply_inverse(q1)-inv_pose(q1)).length()<1e-6,true);

  TEST("Composition p1*p2",pose0*inv_pose,mfpf_pose());

  mfpf_pose pose01 = pose0*pose1;
  vgl_point_2d<double> p4 = pose01(p0);
  TEST("p1*p2(x)==p1(p2(x))",(pose0(pose1(p0))-p4).length()<1e-6,true);

}

TESTMAIN(test_pose);
