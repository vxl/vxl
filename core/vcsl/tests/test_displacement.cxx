//*****************************************************************************
// File name: test_displacement.cxx
// Description: Test the vcsl_displacement class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/05| François BERTEL          |Creation
// 1.1     |2002/01/22| Peter Vanroose           |Avoid new/delete if possible
// 1.2     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_displacement.h>
#include <vcsl/vcsl_graph.h>

static void test_displacement()
{
  vcsl_graph_sptr graph=new vcsl_graph;
  vcsl_spatial_sptr csa=new vcsl_cartesian_3d; csa->set_graph(graph);
  vcsl_spatial_sptr cs0=new vcsl_cartesian_3d; cs0->set_graph(graph);

  vcl_vector<vcsl_spatial_sptr> parent; parent.push_back(csa);
  cs0->set_parent(parent);

  vcl_vector<double> cs0_beat; cs0_beat.reserve(2);
  cs0_beat.push_back(0);
  cs0_beat.push_back(1);
  cs0->set_beat(cs0_beat);

  vcsl_displacement_sptr dis=new vcsl_displacement;
  vcl_vector<double> dis_beat; dis_beat.reserve(3);
  dis_beat.push_back(0);
  dis_beat.push_back(0.5);
  dis_beat.push_back(1);
  dis->set_beat(dis_beat);
  vcl_vector<vcsl_interpolator> interpolators; interpolators.reserve(2);
  interpolators.push_back(vcsl_linear);
  interpolators.push_back(vcsl_linear);
  dis->set_interpolators(interpolators);
  vnl_vector<double> v0(3), v1(3), v2(3);
  v0.put(0,0); v1.put(0,0); v2.put(0,0);
  v0.put(1,0); v1.put(1,0); v2.put(1,1);
  v0.put(2,1); v1.put(2,1); v2.put(2,0);
  vcl_vector<vnl_vector<double> > axis; axis.reserve(3);
  axis.push_back(v0);
  axis.push_back(v1);
  axis.push_back(v2);
  dis->set_axis(axis);

  vnl_vector<double> p0(3), p1(3), p2(3);
  p0.put(0,1); p1.put(0,2); p2.put(0,4);
  p0.put(1,2); p1.put(1,3); p2.put(1,5);
  p0.put(2,3); p1.put(2,4); p2.put(2,6);
  vcl_vector<vnl_vector<double> > point; point.reserve(3);
  point.push_back(p0);
  point.push_back(p1);
  point.push_back(p2);
  dis->set_point(point);

  vcl_vector<double> angles; angles.reserve(3);
  angles.push_back(0);
  angles.push_back(0.1);
  angles.push_back(1.2);
  dis->set_angle(angles);

  vcl_vector<vcsl_spatial_transformation_sptr> motion;
  motion.push_back(dis.ptr());
  cs0->set_motion(motion);

  vnl_vector<double> p(3);
  p.put(0,1);
  p.put(1,2);
  p.put(2,3);

  vcl_cout<<"-----> t=0"<<vcl_endl;
  vnl_vector<double> q=cs0->from_local_to_cs(p,csa,0);
  vcl_cout<<"q(cs0->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  vcl_cout<<"-----> t=0.4"<<vcl_endl;
  q=cs0->from_local_to_cs(p,csa,0.4);
  vcl_cout<<"q(cs0->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  vcl_cout<<"-----> t=0.5"<<vcl_endl;
  q=cs0->from_local_to_cs(p,csa,0.5);
  vcl_cout<<"q(cs0->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  vcl_cout<<"-----> t=0.6"<<vcl_endl;
  q=cs0->from_local_to_cs(p,csa,0.6);
  vcl_cout<<"q(cs0->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  vcl_cout<<"-----> t=1"<<vcl_endl;
  q=cs0->from_local_to_cs(p,csa,1);
  vcl_cout<<"q(cs0->csa)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;
}

TESTMAIN(test_displacement);
