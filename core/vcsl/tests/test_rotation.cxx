//*****************************************************************************
// File name: test_rotation.cxx
// Description: Test the vcsl_rotation class
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
#include <vcsl/vcsl_rotation.h>
#include <vcsl/vcsl_graph.h>

//-----------------------------------------------------------------------------
//: Entry point of the test program
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
  vcl_vector<double> *rot_beat;
  vcl_vector<vcsl_spatial_transformation_sptr> *motion;
  vcsl_rotation_sptr rot;
  vcl_vector<vcsl_interpolator> *interpolators;
  vcl_vector<vnl_vector<double> *> *axis;
  vcl_vector<double> *angles;
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

  parent=new vcl_vector<vcsl_spatial_sptr>;
  parent->push_back(csa);
  cs0->set_parent(*parent);

  cs0_beat=new vcl_vector<double>;
  cs0_beat->reserve(2);
  cs0_beat->push_back(0);
  cs0_beat->push_back(1);
  cs0->set_beat(*cs0_beat);

  motion=new vcl_vector<vcsl_spatial_transformation_sptr>;

  rot=new vcsl_rotation;
  rot_beat=new vcl_vector<double>;
  rot_beat->reserve(3);
  rot_beat->push_back(0);
  rot_beat->push_back(0.5);
  rot_beat->push_back(1);
  rot->set_beat(*rot_beat);
  interpolators=new vcl_vector<vcsl_interpolator>;
  interpolators->reserve(2);
  interpolators->push_back(vcsl_linear);
  interpolators->push_back(vcsl_linear);
  rot->set_interpolators(*interpolators);
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
  rot->set_axis(*axis);

  angles=new vcl_vector<double>;
  angles->reserve(3);
  angles->push_back(0);
  angles->push_back(0.1);
  angles->push_back(1.2);
  rot->set_angle(*angles);
  motion->push_back(rot.ptr());

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
  delete v2;
  delete v1;
  delete v0;
  delete axis;
  delete interpolators;
  delete rot_beat;
  delete cs0_beat;
  delete parent;
  result=0;
  return result;
}
