//*****************************************************************************
// File name: test_scale.cxx
// Description: Test the vcsl_scale class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/19| François BERTEL          |Creation
// 1.1     |2002/01/22| Peter Vanroose           |Avoid new/delete if possible
// 1.2     |2002/11/13| Peter Vanroose           |converted to use TESTMAIN
//*****************************************************************************

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_scale.h>
#include <vcsl/vcsl_graph.h>

static void test_scale()
{
  vcl_cout<<"Creation of graph..."<< vcl_flush;
  vcsl_graph_sptr graph=new vcsl_graph();
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of csa..."<< vcl_flush;
  vcsl_spatial_sptr csa=new vcsl_cartesian_3d();
  vcl_cout<<"done"<<vcl_endl;
  TEST("vcsl_cartesian_3d as vcsl_spatial_sptr", bool(csa), true);
  vcl_cout<<"Adding csa to graph..."<< vcl_flush;
  csa->set_graph(graph);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of cs0..."<< vcl_flush;
  vcsl_spatial_sptr cs0=new vcsl_cartesian_3d();
  vcl_cout<<"done"<<vcl_endl;
  TEST("vcsl_cartesian_3d as vcsl_spatial_sptr", bool(cs0), true);
  vcl_cout<<"Adding cs0 to graph..."<< vcl_flush;
  cs0->set_graph(graph);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of cs1..."<< vcl_flush;
  vcsl_spatial_sptr cs1=new vcsl_cartesian_3d();
  vcl_cout<<"done"<<vcl_endl;
  TEST("vcsl_cartesian_3d as vcsl_spatial_sptr", bool(cs1), true);
  vcl_cout<<"Adding cs1 to graph..."<< vcl_flush;
  cs1->set_graph(graph);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of beat..."<< vcl_flush;
  vcl_vector<double> beat;
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<"Filling of beat..."<< vcl_flush;
  beat.reserve(2);
  beat.push_back(0);
  beat.push_back(1);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Attaching beat to cs0..."<< vcl_flush;
  cs0->set_beat(beat);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of parent..."<< vcl_flush;
  vcl_vector<vcsl_spatial_sptr> parent;
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Filling of parent..."<< vcl_flush;
  parent.reserve(1);
  parent.push_back(csa);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Attaching parent to cs0..."<< vcl_flush;
  cs0->set_parent(parent);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of scale..."<< vcl_flush;
  vcsl_scale_sptr scale=new vcsl_scale;
  vcl_cout<<"done"<<vcl_endl;
  TEST("vcsl_scale_sptr", bool(scale), true);

  vcl_vector<double>& tr0_beat=beat;

  vcl_cout<<"Attaching tr0_beat to scale..."<< vcl_flush;
  scale->set_beat(tr0_beat);
  vcl_cout<<"done"<<vcl_endl;

  TEST("set_beat() and beat() of vcsl_transformation", scale->beat(), tr0_beat);

  vcl_cout<<"Creation of scale_values..."<< vcl_flush;
  vcl_vector<double> scale_values;
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Filling of scale_values..."<< vcl_flush;
  scale_values.reserve(2);
  scale_values.push_back(1);
  scale_values.push_back(2);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Attaching scale_values to scale..."<< vcl_flush;
  scale->set_scale(scale_values);
  vcl_cout<<"done"<<vcl_endl;

  TEST("set_scale() and scale() of vcsl_scale", scale->scale(), scale_values);

  vcl_cout<<"Creation of scale_interpolators..."<< vcl_flush;
  vcl_vector<vcsl_interpolator> scale_interpolators;
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Filling scale_interpolators..."<< vcl_flush;
  scale_interpolators.reserve(1);
  scale_interpolators.push_back(vcsl_linear);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<"Attaching scale_interpolators to scale..."<< vcl_flush;
  scale->set_interpolators(scale_interpolators);
  vcl_cout<<"done"<<vcl_endl;

  TEST("set_interpolators() and interpolators() of vcsl_scale", scale->interpolators(), scale_interpolators);

  vcl_cout<<"Creation of motion..."<< vcl_flush;
  vcl_vector<vcsl_spatial_transformation_sptr> motion;
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Filling motion..."<< vcl_flush;
  motion.reserve(1);
  motion.push_back(scale.ptr());
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Attaching motion to cs0..."<< vcl_flush;
  cs0->set_motion(motion);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of p..."<< vcl_flush;
  vnl_vector<double> p(3);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Setting of p..."<< vcl_flush;
  p.put(0,1);
  p.put(1,2);
  p.put(2,3);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of q from p at time 0 ..."<< vcl_flush;
  vnl_vector<double> q=cs0->from_local_to_cs(p,csa,0);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;
  vcl_cout<<"Creation of q from p at time 0.5 ..."<< vcl_flush;
  q=cs0->from_local_to_cs(p,csa,0.5);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;
  vcl_cout<<"Creation of q from p at time 1 ..."<< vcl_flush;
  q=cs0->from_local_to_cs(p,csa,1);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;
}

TESTMAIN(test_scale);
