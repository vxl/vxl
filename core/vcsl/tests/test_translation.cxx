//*****************************************************************************
// File name: test_translation.cxx
// Description: Test the vcsl_translation class
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
#include <vcsl/vcsl_translation.h>
#include <vcsl/vcsl_graph.h>

static void test_translation()
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

  vcl_vector<vcsl_spatial_transformation_sptr> motion;

  vcsl_translation_sptr tr=new vcsl_translation;
  vcl_vector<double> tr_beat; tr_beat.reserve(3);
  tr_beat.push_back(0);
  tr_beat.push_back(0.5);
  tr_beat.push_back(1);
  tr->set_beat(tr_beat);
  vcl_vector<vcsl_interpolator> interpolators; interpolators.reserve(2);
  interpolators.push_back(vcsl_linear);
  interpolators.push_back(vcsl_linear);
  tr->set_interpolators(interpolators);
  vcl_vector<vnl_vector<double> > vectors; vectors.reserve(3);
  vnl_vector<double> v0(3), v1(3), v2(3);
  v0.put(0,0); v1.put(0,1); v2.put(0,-1);
  v0.put(1,0); v1.put(1,2); v2.put(1,-2);
  v0.put(2,0); v1.put(2,3); v2.put(2,-3);
  vectors.push_back(v0);
  vectors.push_back(v1);
  vectors.push_back(v2);
  tr->set_vector(vectors);
  motion.push_back(tr.ptr());

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

TESTMAIN(test_translation);
