//*****************************************************************************
// File name: test_scale.cxx
// Description: Test the vcsl_scale class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/19| Francois BERTEL          |Creation
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

static void test_scale()
{
  std::cout<<"Creation of graph..."<< std::flush;
  vcsl_graph_sptr graph=new vcsl_graph();
  std::cout<<"done"<<std::endl;

  std::cout<<"Creation of csa..."<< std::flush;
  vcsl_spatial_sptr csa=new vcsl_cartesian_3d();
  std::cout<<"done"<<std::endl;
  TEST("vcsl_cartesian_3d as vcsl_spatial_sptr", bool(csa), true);
  std::cout<<"Adding csa to graph..."<< std::flush;
  csa->set_graph(graph);
  std::cout<<"done"<<std::endl;

  std::cout<<"Creation of cs0..."<< std::flush;
  vcsl_spatial_sptr cs0=new vcsl_cartesian_3d();
  std::cout<<"done"<<std::endl;
  TEST("vcsl_cartesian_3d as vcsl_spatial_sptr", bool(cs0), true);
  std::cout<<"Adding cs0 to graph..."<< std::flush;
  cs0->set_graph(graph);
  std::cout<<"done"<<std::endl;

  std::cout<<"Creation of cs1..."<< std::flush;
  vcsl_spatial_sptr cs1=new vcsl_cartesian_3d();
  std::cout<<"done"<<std::endl;
  TEST("vcsl_cartesian_3d as vcsl_spatial_sptr", bool(cs1), true);
  std::cout<<"Adding cs1 to graph..."<< std::flush;
  cs1->set_graph(graph);
  std::cout<<"done"<<std::endl;

  std::cout<<"Creation of beat..."<< std::flush;
  std::vector<double> beat;
  std::cout<<"done"<<std::endl;
  std::cout<<"Filling of beat..."<< std::flush;
  beat.reserve(2);
  beat.push_back(0);
  beat.push_back(1);
  std::cout<<"done"<<std::endl;

  std::cout<<"Attaching beat to cs0..."<< std::flush;
  cs0->set_beat(beat);
  std::cout<<"done"<<std::endl;

  std::cout<<"Creation of parent..."<< std::flush;
  std::vector<vcsl_spatial_sptr> parent;
  std::cout<<"done"<<std::endl;

  std::cout<<"Filling of parent..."<< std::flush;
  parent.reserve(1);
  parent.push_back(csa);
  std::cout<<"done"<<std::endl;

  std::cout<<"Attaching parent to cs0..."<< std::flush;
  cs0->set_parent(parent);
  std::cout<<"done"<<std::endl;

  std::cout<<"Creation of scale..."<< std::flush;
  vcsl_scale_sptr scale=new vcsl_scale;
  std::cout<<"done"<<std::endl;
  TEST("vcsl_scale_sptr", bool(scale), true);

  std::vector<double>& tr0_beat=beat;

  std::cout<<"Attaching tr0_beat to scale..."<< std::flush;
  scale->set_beat(tr0_beat);
  std::cout<<"done"<<std::endl;

  TEST("set_beat() and beat() of vcsl_transformation", scale->beat(), tr0_beat);

  std::cout<<"Creation of scale_values..."<< std::flush;
  std::vector<double> scale_values;
  std::cout<<"done"<<std::endl;

  std::cout<<"Filling of scale_values..."<< std::flush;
  scale_values.reserve(2);
  scale_values.push_back(1);
  scale_values.push_back(2);
  std::cout<<"done"<<std::endl;

  std::cout<<"Attaching scale_values to scale..."<< std::flush;
  scale->set_scale(scale_values);
  std::cout<<"done"<<std::endl;

  TEST("set_scale() and scale() of vcsl_scale", scale->scale(), scale_values);

  std::cout<<"Creation of scale_interpolators..."<< std::flush;
  std::vector<vcsl_interpolator> scale_interpolators;
  std::cout<<"done"<<std::endl;

  std::cout<<"Filling scale_interpolators..."<< std::flush;
  scale_interpolators.reserve(1);
  scale_interpolators.push_back(vcsl_linear);
  std::cout<<"done"<<std::endl;
  std::cout<<"Attaching scale_interpolators to scale..."<< std::flush;
  scale->set_interpolators(scale_interpolators);
  std::cout<<"done"<<std::endl;

  TEST("set_interpolators() and interpolators() of vcsl_scale", scale->interpolators(), scale_interpolators);

  std::cout<<"Creation of motion..."<< std::flush;
  std::vector<vcsl_spatial_transformation_sptr> motion;
  std::cout<<"done"<<std::endl;

  std::cout<<"Filling motion..."<< std::flush;
  motion.reserve(1);
  motion.emplace_back(scale.ptr());
  std::cout<<"done"<<std::endl;

  std::cout<<"Attaching motion to cs0..."<< std::flush;
  cs0->set_motion(motion);
  std::cout<<"done"<<std::endl;

  std::cout<<"Creation of p..."<< std::flush;
  vnl_vector<double> p(3);
  std::cout<<"done"<<std::endl;

  std::cout<<"Setting of p..."<< std::flush;
  p.put(0,1);
  p.put(1,2);
  p.put(2,3);
  std::cout<<"done"<<std::endl;

  std::cout<<"Creation of q from p at time 0 ..."<< std::flush;
  vnl_vector<double> q=cs0->from_local_to_cs(p,csa,0);
  std::cout<<"done"<<std::endl;
  std::cout<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;
  std::cout<<"Creation of q from p at time 0.5 ..."<< std::flush;
  q=cs0->from_local_to_cs(p,csa,0.5);
  std::cout<<"done"<<std::endl;
  std::cout<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;
  std::cout<<"Creation of q from p at time 1 ..."<< std::flush;
  q=cs0->from_local_to_cs(p,csa,1);
  std::cout<<"done"<<std::endl;
  std::cout<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;
}

TESTMAIN(test_scale);
