//*****************************************************************************
// File name: test_displacement.cxx
// Description: Test the vcsl_displacement class
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
#include <vcsl/vcsl_displacement.h>
#include <vcsl/vcsl_graph.h>

//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result;

  vcsl_graph_sptr graph;
  vcsl_spatial_sptr csa;
  vcsl_spatial_sptr cs0;
  vcl_vector<vcsl_spatial_sptr> *parent;
  vcl_vector<double> *cs0_beat;
  vcl_vector<double> *dis_beat;
  vcl_vector<vcsl_spatial_transformation_sptr> *motion;
  vcsl_displacement_sptr dis;
  vcl_vector<vcsl_interpolator> *interpolators;
  vcl_vector<vnl_vector<double> *> *axis;
  vcl_vector<vnl_vector<double> *> *point;
  vcl_vector<double> *angles;
  vnl_vector<double> *v0;
  vnl_vector<double> *v1;
  vnl_vector<double> *v2;
  vnl_vector<double> *p0;
  vnl_vector<double> *p1;
  vnl_vector<double> *p2;
  vnl_vector<double> *p;
  vnl_vector<double> *q;

  graph=new vcsl_graph;

  csa=new vcsl_cartesian_3d;
  csa->set_graph(graph);

  cs0=new vcsl_cartesian_3d;
  cs0->set_graph(graph);

  parent=new vcl_vector<vcsl_spatial_sptr>;
  parent->push_back(csa);
  cs0->set_parent(*parent);

  cs0_beat=new vcl_vector<double>;
  cs0_beat->reserve(2);
  cs0_beat->push_back(0);
  cs0_beat->push_back(1);
  cs0->set_beat(*cs0_beat);

  motion=new vcl_vector<vcsl_spatial_transformation_sptr>;

  dis=new vcsl_displacement;
  dis_beat=new vcl_vector<double>;
  dis_beat->reserve(3);
  dis_beat->push_back(0);
  dis_beat->push_back(0.5);
  dis_beat->push_back(1);
  dis->set_beat(*dis_beat);
  interpolators=new vcl_vector<vcsl_interpolator>;
  interpolators->reserve(2);
  interpolators->push_back(vcsl_linear);
  interpolators->push_back(vcsl_linear);
  dis->set_interpolators(*interpolators);
  axis=new vcl_vector<vnl_vector<double> *>;
  v0=new vnl_vector<double>(3);
  v0->put(0,0);
  v0->put(1,0);
  v0->put(2,1);
  v1=new vnl_vector<double>(3);
  v1->put(0,0);
  v1->put(1,0);
  v1->put(2,1);
  v2=new vnl_vector<double>(3);
  v2->put(0,0);
  v2->put(1,1);
  v2->put(2,0);
  axis->reserve(3);
  axis->push_back(v0);
  axis->push_back(v1);
  axis->push_back(v2);
  dis->set_axis(*axis);

  point=new vcl_vector<vnl_vector<double> *>;
  p0=new vnl_vector<double>(3);
  p0->put(0,1);
  p0->put(1,2);
  p0->put(2,3);
  p1=new vnl_vector<double>(3);
  p1->put(0,2);
  p1->put(1,3);
  p1->put(2,4);
  p2=new vnl_vector<double>(3);
  p2->put(0,4);
  p2->put(1,5);
  p2->put(2,6);
  point->reserve(3);
  point->push_back(p0);
  point->push_back(p1);
  point->push_back(p2);
  dis->set_point(*point);

  angles=new vcl_vector<double>;
  angles->reserve(3);
  angles->push_back(0);
  angles->push_back(0.1);
  angles->push_back(1.2);
  dis->set_angle(*angles);
  motion->push_back(dis.ptr());

  cs0->set_motion(*motion);

  p=new vnl_vector<double>(3);
  p->put(0,1);
  p->put(1,2);
  p->put(2,3);

  vcl_cout<<"-----> t=0"<<vcl_endl;
  q=cs0->from_local_to_cs(*p,csa,0);
  vcl_cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  vcl_cout<<"-----> t=0.4"<<vcl_endl;
  q=cs0->from_local_to_cs(*p,csa,0.4);
  vcl_cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;
  
  vcl_cout<<"-----> t=0.5"<<vcl_endl;
  q=cs0->from_local_to_cs(*p,csa,0.5);
  vcl_cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  vcl_cout<<"-----> t=0.6"<<vcl_endl;
  q=cs0->from_local_to_cs(*p,csa,0.6);
  vcl_cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  vcl_cout<<"-----> t=1"<<vcl_endl;
  q=cs0->from_local_to_cs(*p,csa,1);
  vcl_cout<<"q(cs0->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  delete p;
  delete motion;
  delete angles;
  delete p2;
  delete p1;
  delete p0;
  delete point;
  delete v2;
  delete v1;
  delete v0;
  delete axis;
  delete interpolators;
  delete dis_beat;
  delete cs0_beat;
  delete parent;
  result=0;
  return result;
}
