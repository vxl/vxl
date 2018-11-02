//*****************************************************************************
// File name: test_transformation_graph.cxx
// Description: Test the search of a path from a coordinate system to another
//              one
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/31| Francois BERTEL          |Creation
// 1.1     |2002/01/22| Peter Vanroose           |Avoid new/delete if possible
// 1.2     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_scale.h>
#include <vcsl/vcsl_graph.h>

static void test1(void);

//-----------------------------------------------------------------------------
//
//       graph: csa<------cs1<-------cs2
//              ^     scale1     scale2
//              |------cs3
//               scale3
//
//       coordinates of p from cs2 to csa ?
//       coordinates of p from cs1 to cs2 ?
//       coordinates of p from cs1 to cs3 ?
//
//-----------------------------------------------------------------------------
static void test1(void)
{
  vcsl_graph_sptr graph=new vcsl_graph;
  vcsl_spatial_sptr csa=new vcsl_cartesian_3d; csa->set_graph(graph);
  vcsl_spatial_sptr cs1=new vcsl_cartesian_3d; cs1->set_graph(graph);
  vcsl_spatial_sptr cs2=new vcsl_cartesian_3d; cs2->set_graph(graph);
  vcsl_spatial_sptr cs3=new vcsl_cartesian_3d; cs3->set_graph(graph);

  std::vector<double> cs1_beat; cs1_beat.reserve(2);
  cs1_beat.push_back(0);
  cs1_beat.push_back(1);
  cs1->set_beat(cs1_beat);

  vcsl_scale_sptr scale1=new vcsl_scale;
  std::vector<double> beat1; beat1.reserve(3);
  beat1.push_back(0);
  beat1.push_back(0.5);
  beat1.push_back(1);
  scale1->set_beat(beat1);

  std::vector<double> scale1_values; scale1_values.reserve(3);
  scale1_values.push_back(1);
  scale1_values.push_back(5);
  scale1_values.push_back(2);
  scale1->set_scale(scale1_values);

  std::vector<vcsl_interpolator> interpolators1; interpolators1.reserve(2);
  interpolators1.push_back(vcsl_linear);
  interpolators1.push_back(vcsl_linear);
  scale1->set_interpolators(interpolators1);

  std::vector<vcsl_spatial_transformation_sptr> motion1;
  motion1.emplace_back(scale1.ptr());
  cs1->set_motion(motion1);

  std::vector<vcsl_spatial_sptr> parent1;
  parent1.push_back(csa);
  cs1->set_parent(parent1);

  std::vector<double> cs2_beat; cs2_beat.reserve(2);
  cs2_beat.push_back(0);
  cs2_beat.push_back(1);
  cs2->set_beat(cs2_beat);

  vcsl_scale_sptr scale2=new vcsl_scale;
  std::vector<double> beat2=cs2_beat;
  scale2->set_beat(beat2);

  std::vector<double> scale2_values; scale2_values.reserve(2);
  scale2_values.push_back(1);
  scale2_values.push_back(3);
  scale2->set_scale(scale2_values);

  //  interpolator2=new vcsl_linear_interpolator;
  std::vector<vcsl_interpolator> interpolators2;
  interpolators2.push_back(vcsl_linear);
  scale2->set_interpolators(interpolators2);

  std::vector<vcsl_spatial_transformation_sptr> motion2;
  motion2.emplace_back(scale2.ptr());
  cs2->set_motion(motion2);

  std::vector<vcsl_spatial_sptr> parent2; parent2.push_back(cs1);
  cs2->set_parent(parent2);

  std::vector<double> cs3_beat; cs3_beat.reserve(2);
  cs3_beat.push_back(0);
  cs3_beat.push_back(1);
  cs3->set_beat(cs3_beat);

  vcsl_scale_sptr scale3=new vcsl_scale;
  std::vector<double> beat3=cs3_beat;
  scale3->set_beat(beat3);

  std::vector<double> scale3_values; scale3_values.reserve(2);
  scale3_values.push_back(1);
  scale3_values.push_back(100);
  scale3->set_scale(scale3_values);

  std::vector<vcsl_interpolator> interpolators3;
  interpolators3.push_back(vcsl_linear);
  scale3->set_interpolators(interpolators3);

  std::vector<vcsl_spatial_transformation_sptr> motion3;
  motion3.emplace_back(scale3.ptr());
  cs3->set_motion(motion3);

  std::vector<vcsl_spatial_sptr> parent3; parent3.push_back(csa);
  cs3->set_parent(parent3);

  vnl_vector<double> p(3);
  p.put(0,1);
  p.put(1,2);
  p.put(2,3);
  std::cout<<"-----> t=0"<<std::endl;
  vnl_vector<double> q=cs2->from_local_to_cs(p,csa,0);
  std::cout<<"q(cs2->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=cs1->from_local_to_cs(p,cs2,0);
  std::cout<<"q(cs1->cs2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=cs1->from_local_to_cs(p,cs3,0);
  std::cout<<"q(cs1->cs3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  std::cout<<"-----> t=0.4"<<std::endl;
  q=cs2->from_local_to_cs(p,csa,0.4);
  std::cout<<"q(cs2->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=cs1->from_local_to_cs(p,cs2,0.4);
  std::cout<<"q(cs1->cs2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=cs1->from_local_to_cs(p,cs3,0.4);
  std::cout<<"q(cs1->cs3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;


  std::cout<<"-----> t=0.5"<<std::endl;
  q=cs2->from_local_to_cs(p,csa,0.5);
  std::cout<<"q(cs2->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=cs1->from_local_to_cs(p,cs2,0.5);
  std::cout<<"q(cs1->cs2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=cs1->from_local_to_cs(p,cs3,0.5);
  std::cout<<"q(cs1->cs3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;


  std::cout<<"-----> t=1"<<std::endl;
  q=cs2->from_local_to_cs(p,csa,1);
  std::cout<<"q(cs2->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=cs1->from_local_to_cs(p,cs2,1);
  std::cout<<"q(cs1->cs2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=cs1->from_local_to_cs(p,cs3,1);
  std::cout<<"q(cs1->cs3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;
}

static void test_transformation_graph()
{
  test1();
}

TESTMAIN(test_transformation_graph);
