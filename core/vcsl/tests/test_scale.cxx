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

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cassert.h>
#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_scale.h>
#include <vcsl/vcsl_graph.h>

//-----------------------------------------------------------------------------
// Name: main
// Task: Entry point of the test program
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
  cout<<"Creation of graph..."<<flush;
  graph=new vcsl_graph();
  cout<<"done"<<endl;

  cout<<"Creation of csa..."<<flush;
  csa=new vcsl_cartesian_3d();
  assert(csa.ptr()!=0);
  cout<<"done"<<endl;
  cout<<"Adding csa to graph..."<<flush;
  csa->set_graph(graph);
  cout<<"done"<<endl;

  cout<<"Creation of cs0..."<<flush;
  cs0=new vcsl_cartesian_3d();
  assert(cs0.ptr()!=0);
  cout<<"done"<<endl;
  cout<<"Adding cs0 to graph..."<<flush;
  cs0->set_graph(graph);
  cout<<"done"<<endl;

  cout<<"Creation of cs1..."<<flush;
  cs1=new vcsl_cartesian_3d();
  assert(cs1.ptr()!=0);
  cout<<"done"<<endl;
  cout<<"Adding cs1 to graph..."<<flush;
  cs1->set_graph(graph);
  cout<<"done"<<endl;

  cout<<"Creation of beat..."<<flush;
  beat=new vcl_vector<double>;
  assert(beat!=0);
  cout<<"done"<<endl;
  cout<<"Filling of beat..."<<flush;
  beat->reserve(2);
  beat->push_back(0);
  beat->push_back(1);
  cout<<"done"<<endl;
  
  cout<<"Attaching beat to cs0..."<<flush;
  cs0->set_beat(*beat);
  cout<<"done"<<endl;

  cout<<"Creation of parent..."<<flush;
  parent=new vcl_vector<vcsl_spatial_ref>;
  assert(parent!=0);
  cout<<"done"<<endl;

  cout<<"Filling of parent..."<<flush;
  parent->reserve(1);
  parent->push_back(csa);
  cout<<"done"<<endl;

  cout<<"Attaching parent to cs0..."<<flush;
  cs0->set_parent(*parent);
  cout<<"done"<<endl;

  cout<<"Creation of motion..."<<flush;
  motion=new vcl_vector<vcsl_spatial_transformation_ref>;
  assert(motion!=0);
  cout<<"done"<<endl;

  motion->reserve(1);

  cout<<"Creation of scale..."<<flush;
  scale=new vcsl_scale;
  assert(scale.ptr()!=0);
  cout<<"done"<<endl;

  tr0_beat=beat;

  cout<<"Attaching tr0_beat to scale..."<<flush;
  scale->set_beat(*tr0_beat);
  cout<<"done"<<endl;

  cout<<"Test set_beat() and beat() of vcsl_transformation..."<<flush;
  assert(scale->beat()==tr0_beat);
  cout<<"passed"<<endl;

  cout<<"Creation of scale_values..."<<flush;
  scale_values=new vcl_vector<double>;
  assert(scale_values!=0);
  cout<<"done"<<endl;

  cout<<"Filling of scale_values..."<<flush;
  scale_values->reserve(2);
  scale_values->push_back(1);
  scale_values->push_back(2);
  cout<<"done"<<endl;

  cout<<"Attaching scale_values to scale..."<<flush;
  scale->set_scale(*scale_values);
  cout<<"done"<<endl;

  cout<<"Test set_scale() and scale() of vcsl_scale...";
  cout<<flush;
  assert(scale->scale()==scale_values);
  cout<<"passed"<<endl;

  cout<<"Creation of scale_interpolators..."<<flush;
  scale_interpolators=new vcl_vector<vcsl_interpolator>;
  assert(scale_interpolators!=0);
  cout<<"done"<<endl;

  scale_interpolators->reserve(1);
  cout<<"Filling scale_interpolators..."<<flush;
  scale_interpolators->push_back(vcsl_linear);
  cout<<"done"<<endl;
  cout<<"Attaching scale_interpolators to scale..."<<flush;
  scale->set_interpolators(*scale_interpolators);
  cout<<"done"<<endl;

  cout<<"Test set_interpolators() and interpolators() of ";
  cout<<"vcsl_scale..."<<flush;
  assert(scale->interpolators()==scale_interpolators);
  cout<<"passed"<<endl;

  cout<<"Filling motion..."<<flush;
  motion->push_back(scale.ptr());
  cout<<"done"<<endl;

  cout<<"Attaching motion to cs0..."<<flush;
  cs0->set_motion(*motion);
  cout<<"done"<<endl;

  cout<<"Creation of p..."<<flush;
  p=new vnl_vector<double>(3);
  assert(p!=0);
  cout<<"done"<<endl;

  cout<<"Setting of p..."<<flush;
  p->put(0,1);
  p->put(1,2);
  p->put(2,3);
  cout<<"done"<<endl;

  cout<<"Creation of q from p at time 0 ..."<<flush;
  q=cs0->from_local_to_cs(*p,csa,0);
  assert(q!=0);
  cout<<"done"<<endl;
  cout<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<endl;
  delete q;
  cout<<"Creation of q from p at time 0.5 ..."<<flush;
  q=cs0->from_local_to_cs(*p,csa,0.5);
  assert(q!=0);
  cout<<"done"<<endl;
  cout<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<endl;
  delete q;
  cout<<"Creation of q from p at time 1 ..."<<flush;
  q=cs0->from_local_to_cs(*p,csa,1);
  assert(q!=0);
  cout<<"done"<<endl;
  cout<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<endl;
  delete q;

  delete scale_interpolators;
  delete scale_values;
  delete motion;
  delete parent;
  delete beat;

  return result;
}
