//*****************************************************************************
// File name: test_translation.cxx
// Description: Test the vcsl_translation class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/05| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_translation.h>
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
  vcl_vector<vcsl_spatial_ref> *parent;
  vcl_vector<double> *cs0_beat;
  vcl_vector<double> *tr_beat;
  vcl_vector<vcsl_spatial_transformation_ref> *motion;
  vcsl_translation_ref tr;
  vcl_vector<vcsl_interpolator> *interpolators;
  vcl_vector<vnl_vector<double> *> *vectors;
  vnl_vector<double> *v0;
  vnl_vector<double> *v1;
  vnl_vector<double> *v2;
  vnl_vector<double> *p;
  vnl_vector<double> *q;

  graph=new vcsl_graph;

  csa=new vcsl_cartesian_3d;
  csa->set_graph(graph);

  cs0=new vcsl_cartesian_3d;
  cs0->set_graph(graph);

  parent=new vcl_vector<vcsl_spatial_ref>;
  parent->push_back(csa);
  cs0->set_parent(*parent);

  cs0_beat=new vcl_vector<double>;
  cs0_beat->reserve(2);
  cs0_beat->push_back(0);
  cs0_beat->push_back(1);
  cs0->set_beat(*cs0_beat);

  motion=new vcl_vector<vcsl_spatial_transformation_ref>;

  tr=new vcsl_translation;
  tr_beat=new vcl_vector<double>;
  tr_beat->reserve(3);
  tr_beat->push_back(0);
  tr_beat->push_back(0.5);
  tr_beat->push_back(1);
  tr->set_beat(*tr_beat);
  interpolators=new vcl_vector<vcsl_interpolator>;
  interpolators->reserve(2);
  interpolators->push_back(vcsl_linear);
  interpolators->push_back(vcsl_linear);
  tr->set_interpolators(*interpolators);
  vectors=new vcl_vector<vnl_vector<double> *>;
  v0=new vnl_vector<double>(3);
  v0->put(0,0);
  v0->put(1,0);
  v0->put(2,0);
  v1=new vnl_vector<double>(3);
  v1->put(0,1);
  v1->put(1,2);
  v1->put(2,3);
  v2=new vnl_vector<double>(3);
  v2->put(0,-1);
  v2->put(1,-2);
  v2->put(2,-3);
  vectors->reserve(3);
  vectors->push_back(v0);
  vectors->push_back(v1);
  vectors->push_back(v2);
  tr->set_vector(*vectors);
  motion->push_back(tr.ptr());

  cs0->set_motion(*motion);

  p=new vnl_vector<double>(3);
  p->put(0,1);
  p->put(1,2);
  p->put(2,3);

  cout<<"-----> t=0"<<endl;
  q=cs0->from_local_to_cs(*p,csa,0);
  cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<endl;
  delete q;

  cout<<"-----> t=0.4"<<endl;
  q=cs0->from_local_to_cs(*p,csa,0.4);
  cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<endl;
  delete q;
  
  cout<<"-----> t=0.5"<<endl;
  q=cs0->from_local_to_cs(*p,csa,0.5);
  cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<endl;
  delete q;

  cout<<"-----> t=0.6"<<endl;
  q=cs0->from_local_to_cs(*p,csa,0.6);
  cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<endl;
  delete q;

  cout<<"-----> t=1"<<endl;
  q=cs0->from_local_to_cs(*p,csa,1);
  cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<endl;
  delete q;

  delete p;
  delete motion;
  delete v2;
  delete v1;
  delete v0;
  delete vectors;
  delete interpolators;
  delete tr_beat;
  delete cs0_beat;
  delete parent;
  result=0;
  return result;
}
