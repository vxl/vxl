//*****************************************************************************
// File name: test_scale.cxx
// Description: Test the vcsl_scale class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/19| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_scale.h>
#include <vcsl/vcsl_graph.h>

//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result;

  vcsl_graph_ref graph;
  vcsl_spatial_ref csa;
  vcsl_spatial_ref cs0;
  vcsl_spatial_ref cs1;
  vcl_vector<double> *beat;
  vcl_vector<double> *tr0_beat;
  vcl_vector<vcsl_spatial_ref> *parent;
  vcl_vector<vcsl_spatial_transformation_ref> *motion;
  vcl_vector<double> *scale_values;
  vcl_vector<vcsl_interpolator> *scale_interpolators;
  vnl_vector<double> *p;
  vnl_vector<double> *q;
  vcsl_scale_ref scale;

  result=0;
  vcl_cout<<"Creation of graph..."<< vcl_flush;
  graph=new vcsl_graph();
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of csa..."<< vcl_flush;
  csa=new vcsl_cartesian_3d();
  assert(csa.ptr()!=0);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<"Adding csa to graph..."<< vcl_flush;
  csa->set_graph(graph);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of cs0..."<< vcl_flush;
  cs0=new vcsl_cartesian_3d();
  assert(cs0.ptr()!=0);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<"Adding cs0 to graph..."<< vcl_flush;
  cs0->set_graph(graph);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of cs1..."<< vcl_flush;
  cs1=new vcsl_cartesian_3d();
  assert(cs1.ptr()!=0);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<"Adding cs1 to graph..."<< vcl_flush;
  cs1->set_graph(graph);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of beat..."<< vcl_flush;
  beat=new vcl_vector<double>;
  assert(beat!=0);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<"Filling of beat..."<< vcl_flush;
  beat->reserve(2);
  beat->push_back(0);
  beat->push_back(1);
  vcl_cout<<"done"<<vcl_endl;
  
  vcl_cout<<"Attaching beat to cs0..."<< vcl_flush;
  cs0->set_beat(*beat);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of parent..."<< vcl_flush;
  parent=new vcl_vector<vcsl_spatial_ref>;
  assert(parent!=0);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Filling of parent..."<< vcl_flush;
  parent->reserve(1);
  parent->push_back(csa);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Attaching parent to cs0..."<< vcl_flush;
  cs0->set_parent(*parent);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of motion..."<< vcl_flush;
  motion=new vcl_vector<vcsl_spatial_transformation_ref>;
  assert(motion!=0);
  vcl_cout<<"done"<<vcl_endl;

  motion->reserve(1);

  vcl_cout<<"Creation of scale..."<< vcl_flush;
  scale=new vcsl_scale;
  assert(scale.ptr()!=0);
  vcl_cout<<"done"<<vcl_endl;

  tr0_beat=beat;

  vcl_cout<<"Attaching tr0_beat to scale..."<< vcl_flush;
  scale->set_beat(*tr0_beat);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Test set_beat() and beat() of vcsl_transformation..."<< vcl_flush;
  assert(scale->beat()==tr0_beat);
  vcl_cout<<"passed"<<vcl_endl;

  vcl_cout<<"Creation of scale_values..."<< vcl_flush;
  scale_values=new vcl_vector<double>;
  assert(scale_values!=0);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Filling of scale_values..."<< vcl_flush;
  scale_values->reserve(2);
  scale_values->push_back(1);
  scale_values->push_back(2);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Attaching scale_values to scale..."<< vcl_flush;
  scale->set_scale(*scale_values);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Test set_scale() and scale() of vcsl_scale...";
  vcl_cout<< vcl_flush;
  assert(scale->scale()==scale_values);
  vcl_cout<<"passed"<<vcl_endl;

  vcl_cout<<"Creation of scale_interpolators..."<< vcl_flush;
  scale_interpolators=new vcl_vector<vcsl_interpolator>;
  assert(scale_interpolators!=0);
  vcl_cout<<"done"<<vcl_endl;

  scale_interpolators->reserve(1);
  vcl_cout<<"Filling scale_interpolators..."<< vcl_flush;
  scale_interpolators->push_back(vcsl_linear);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<"Attaching scale_interpolators to scale..."<< vcl_flush;
  scale->set_interpolators(*scale_interpolators);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Test set_interpolators() and interpolators() of ";
  vcl_cout<<"vcsl_scale..."<< vcl_flush;
  assert(scale->interpolators()==scale_interpolators);
  vcl_cout<<"passed"<<vcl_endl;

  vcl_cout<<"Filling motion..."<< vcl_flush;
  motion->push_back(scale.ptr());
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Attaching motion to cs0..."<< vcl_flush;
  cs0->set_motion(*motion);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of p..."<< vcl_flush;
  p=new vnl_vector<double>(3);
  assert(p!=0);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Setting of p..."<< vcl_flush;
  p->put(0,1);
  p->put(1,2);
  p->put(2,3);
  vcl_cout<<"done"<<vcl_endl;

  vcl_cout<<"Creation of q from p at time 0 ..."<< vcl_flush;
  q=cs0->from_local_to_cs(*p,csa,0);
  assert(q!=0);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;
  vcl_cout<<"Creation of q from p at time 0.5 ..."<< vcl_flush;
  q=cs0->from_local_to_cs(*p,csa,0.5);
  assert(q!=0);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;
  vcl_cout<<"Creation of q from p at time 1 ..."<< vcl_flush;
  q=cs0->from_local_to_cs(*p,csa,1);
  assert(q!=0);
  vcl_cout<<"done"<<vcl_endl;
  vcl_cout<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  delete scale_interpolators;
  delete scale_values;
  delete motion;
  delete parent;
  delete beat;

  return result;
}
