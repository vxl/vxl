//*****************************************************************************
// File name: test_transformation_graph.cxx
// Description: Test the search of a path from a coordinate system to another
//              one
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/31| François BERTEL          |Creation
// 1.1     |2002/01/22| Peter Vanroose           |Avoid new/delete if possible
//*****************************************************************************

//:
// \file

#include <vcl_iostream.h>
#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_scale.h>
#include <vcsl/vcsl_graph.h>

static void test1(void);

//-----------------------------------------------------------------------------
//:
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

  vcl_vector<double> cs1_beat; cs1_beat.reserve(2);
  cs1_beat.push_back(0);
  cs1_beat.push_back(1);
  cs1->set_beat(cs1_beat);

  vcsl_scale_sptr scale1=new vcsl_scale;
  vcl_vector<double> beat1; beat1.reserve(3);
  beat1.push_back(0);
  beat1.push_back(0.5);
  beat1.push_back(1);
  scale1->set_beat(beat1);

  vcl_vector<double> scale1_values; scale1_values.reserve(3);
  scale1_values.push_back(1);
  scale1_values.push_back(5);
  scale1_values.push_back(2);
  scale1->set_scale(scale1_values);

  vcl_vector<vcsl_interpolator> interpolators1; interpolators1.reserve(2);
  interpolators1.push_back(vcsl_linear);
  interpolators1.push_back(vcsl_linear);
  scale1->set_interpolators(interpolators1);

  vcl_vector<vcsl_spatial_transformation_sptr> motion1;
  motion1.push_back(scale1.ptr());
  cs1->set_motion(motion1);

  vcl_vector<vcsl_spatial_sptr> parent1;
  parent1.push_back(csa);
  cs1->set_parent(parent1);

  vcl_vector<double> cs2_beat; cs2_beat.reserve(2);
  cs2_beat.push_back(0);
  cs2_beat.push_back(1);
  cs2->set_beat(cs2_beat);

  vcsl_scale_sptr scale2=new vcsl_scale;
  vcl_vector<double> beat2=cs2_beat;
  scale2->set_beat(beat2);

  vcl_vector<double> scale2_values; scale2_values.reserve(2);
  scale2_values.push_back(1);
  scale2_values.push_back(3);
  scale2->set_scale(scale2_values);

  //  interpolator2=new vcsl_linear_interpolator;
  vcl_vector<vcsl_interpolator> interpolators2;
  interpolators2.push_back(vcsl_linear);
  scale2->set_interpolators(interpolators2);

  vcl_vector<vcsl_spatial_transformation_sptr> motion2;
  motion2.push_back(scale2.ptr());
  cs2->set_motion(motion2);

  vcl_vector<vcsl_spatial_sptr> parent2; parent2.push_back(cs1);
  cs2->set_parent(parent2);

  vcl_vector<double> cs3_beat; cs3_beat.reserve(2);
  cs3_beat.push_back(0);
  cs3_beat.push_back(1);
  cs3->set_beat(cs3_beat);

  vcsl_scale_sptr scale3=new vcsl_scale;
  vcl_vector<double> beat3=cs3_beat;
  scale3->set_beat(beat3);

  vcl_vector<double> scale3_values; scale3_values.reserve(2);
  scale3_values.push_back(1);
  scale3_values.push_back(100);
  scale3->set_scale(scale3_values);

  vcl_vector<vcsl_interpolator> interpolators3;
  interpolators3.push_back(vcsl_linear);
  scale3->set_interpolators(interpolators3);

  vcl_vector<vcsl_spatial_transformation_sptr> motion3;
  motion3.push_back(scale3.ptr());
  cs3->set_motion(motion3);

  vcl_vector<vcsl_spatial_sptr> parent3; parent3.push_back(csa);
  cs3->set_parent(parent3);

  vnl_vector<double> p(3);
  p.put(0,1);
  p.put(1,2);
  p.put(2,3);
  vcl_cout<<"-----> t=0"<<vcl_endl;
  vnl_vector<double> q=cs2->from_local_to_cs(p,csa,0);
  vcl_cout<<"q(cs2->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=cs1->from_local_to_cs(p,cs2,0);
  vcl_cout<<"q(cs1->cs2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=cs1->from_local_to_cs(p,cs3,0);
  vcl_cout<<"q(cs1->cs3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  vcl_cout<<"-----> t=0.4"<<vcl_endl;
  q=cs2->from_local_to_cs(p,csa,0.4);
  vcl_cout<<"q(cs2->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=cs1->from_local_to_cs(p,cs2,0.4);
  vcl_cout<<"q(cs1->cs2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=cs1->from_local_to_cs(p,cs3,0.4);
  vcl_cout<<"q(cs1->cs3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;


  vcl_cout<<"-----> t=0.5"<<vcl_endl;
  q=cs2->from_local_to_cs(p,csa,0.5);
  vcl_cout<<"q(cs2->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=cs1->from_local_to_cs(p,cs2,0.5);
  vcl_cout<<"q(cs1->cs2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=cs1->from_local_to_cs(p,cs3,0.5);
  vcl_cout<<"q(cs1->cs3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;


  vcl_cout<<"-----> t=1"<<vcl_endl;
  q=cs2->from_local_to_cs(p,csa,1);
  vcl_cout<<"q(cs2->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=cs1->from_local_to_cs(p,cs2,1);
  vcl_cout<<"q(cs1->cs2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=cs1->from_local_to_cs(p,cs3,1);
  vcl_cout<<"q(cs1->cs3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;
}

//-----------------------------------------------------------------------------
//: Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  test1();
  return 0;
}
