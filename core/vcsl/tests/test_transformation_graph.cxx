//*****************************************************************************
// File name: test_tranformation_graph.cxx
// Description: Test the search of a path from a coordinate system to another
//              one
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/07/31| François BERTEL          |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_cassert.h>
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
  vcsl_graph_sptr graph;
  vcsl_spatial_sptr csa;
  vcsl_spatial_sptr cs1;
  vcl_vector<double> *cs1_beat;
  vcl_vector<vcsl_spatial_transformation_sptr> *motion1;
  vcl_vector<vcsl_spatial_sptr> *parent1;

  vcsl_spatial_sptr cs2;
  vcl_vector<double> *cs2_beat;
  vcl_vector<vcsl_spatial_transformation_sptr> *motion2;
  vcl_vector<vcsl_spatial_sptr> *parent2;
  
  vcsl_spatial_sptr cs3;
  vcl_vector<double> *cs3_beat;
  vcl_vector<vcsl_spatial_transformation_sptr> *motion3;
  vcl_vector<vcsl_spatial_sptr> *parent3;

  vcsl_scale_sptr scale1;
  vcl_vector<double> *beat1;
  vcl_vector<double> *scale1_values;
  vcl_vector<vcsl_interpolator> *interpolators1;

  vcsl_scale_sptr scale2;
  vcl_vector<double> *beat2;
  vcl_vector<double> *scale2_values;
  vcl_vector<vcsl_interpolator> *interpolators2;

  vcsl_scale_sptr scale3;
  vcl_vector<double> *beat3;
  vcl_vector<double> *scale3_values;
  vcl_vector<vcsl_interpolator> *interpolators3;

  vnl_vector<double> *p;
  vnl_vector<double> *q;

  graph=new vcsl_graph;
  csa=new vcsl_cartesian_3d;
  csa->set_graph(graph);
  cs1=new vcsl_cartesian_3d;
  cs1->set_graph(graph);
  cs2=new vcsl_cartesian_3d;
  cs2->set_graph(graph);
  cs3=new vcsl_cartesian_3d;
  cs3->set_graph(graph);

  cs1_beat=new vcl_vector<double>;
  cs1_beat->reserve(2);
  cs1_beat->push_back(0);
  cs1_beat->push_back(1);
  cs1->set_beat(*cs1_beat);

  scale1=new vcsl_scale;
  beat1=new vcl_vector<double>;
  beat1->reserve(3);
  beat1->push_back(0);
  beat1->push_back(0.5);
  beat1->push_back(1);
  scale1->set_beat(*beat1);

  scale1_values=new vcl_vector<double>;
  scale1_values->reserve(3);
  scale1_values->push_back(1);
  scale1_values->push_back(5);
  scale1_values->push_back(2);
  scale1->set_scale(*scale1_values);

  interpolators1=new vcl_vector<vcsl_interpolator>;
  interpolators1->reserve(2);
  interpolators1->push_back(vcsl_linear);
  interpolators1->push_back(vcsl_linear);
  scale1->set_interpolators(*interpolators1);

  motion1=new vcl_vector<vcsl_spatial_transformation_sptr>;
  motion1->push_back(scale1.ptr());
  cs1->set_motion(*motion1);

  parent1=new vcl_vector<vcsl_spatial_sptr>;
  parent1->push_back(csa);
  cs1->set_parent(*parent1);

  cs2_beat=new vcl_vector<double>;
  cs2_beat->reserve(2);
  cs2_beat->push_back(0);
  cs2_beat->push_back(1);
  cs2->set_beat(*cs2_beat);

  scale2=new vcsl_scale;
  beat2=cs2_beat;
  scale2->set_beat(*beat2);

  scale2_values=new vcl_vector<double>;
  scale2_values->reserve(2);
  scale2_values->push_back(1);
  scale2_values->push_back(3);
  scale2->set_scale(*scale2_values);
  
  //  interpolator2=new vcsl_linear_interpolator;
  interpolators2=new vcl_vector<vcsl_interpolator>;
  interpolators2->push_back(vcsl_linear);
  scale2->set_interpolators(*interpolators2);

  motion2=new vcl_vector<vcsl_spatial_transformation_sptr>;
  motion2->push_back(scale2.ptr());
  cs2->set_motion(*motion2);

  parent2=new vcl_vector<vcsl_spatial_sptr>;
  parent2->push_back(cs1);
  cs2->set_parent(*parent2);

  cs3_beat=new vcl_vector<double>;
  cs3_beat->reserve(2);
  cs3_beat->push_back(0);
  cs3_beat->push_back(1);
  cs3->set_beat(*cs3_beat);

  scale3=new vcsl_scale;
  beat3=cs3_beat;
  scale3->set_beat(*beat3);

  scale3_values=new vcl_vector<double>;
  scale3_values->reserve(2);
  scale3_values->push_back(1);
  scale3_values->push_back(100);
  scale3->set_scale(*scale3_values);
  
  interpolators3=new vcl_vector<vcsl_interpolator>;
  interpolators3->push_back(vcsl_linear);
  scale3->set_interpolators(*interpolators3);

  motion3=new vcl_vector<vcsl_spatial_transformation_sptr>;
  motion3->push_back(scale3.ptr());
  cs3->set_motion(*motion3);

  parent3=new vcl_vector<vcsl_spatial_sptr>;
  parent3->push_back(csa);
  cs3->set_parent(*parent3);

  p=new vnl_vector<double>(3);
  p->put(0,1);
  p->put(1,2);
  p->put(2,3);
  vcl_cout<<"-----> t=0"<<vcl_endl;
  q=cs2->from_local_to_cs(*p,csa,0);
  vcl_cout<<"q(cs2->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  q=cs1->from_local_to_cs(*p,cs2,0);
  vcl_cout<<"q(cs1->cs2)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  q=cs1->from_local_to_cs(*p,cs3,0);
  vcl_cout<<"q(cs1->cs3)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  vcl_cout<<"-----> t=0.4"<<vcl_endl;
  q=cs2->from_local_to_cs(*p,csa,0.4);
  vcl_cout<<"q(cs2->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  q=cs1->from_local_to_cs(*p,cs2,0.4);
  vcl_cout<<"q(cs1->cs2)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  q=cs1->from_local_to_cs(*p,cs3,0.4);
  vcl_cout<<"q(cs1->cs3)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;


  vcl_cout<<"-----> t=0.5"<<vcl_endl;
  q=cs2->from_local_to_cs(*p,csa,0.5);
  vcl_cout<<"q(cs2->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  q=cs1->from_local_to_cs(*p,cs2,0.5);
  vcl_cout<<"q(cs1->cs2)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  q=cs1->from_local_to_cs(*p,cs3,0.5);
  vcl_cout<<"q(cs1->cs3)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;


  vcl_cout<<"-----> t=1"<<vcl_endl;
  q=cs2->from_local_to_cs(*p,csa,1);
  vcl_cout<<"q(cs2->csa)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  q=cs1->from_local_to_cs(*p,cs2,1);
  vcl_cout<<"q(cs1->cs2)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  q=cs1->from_local_to_cs(*p,cs3,1);
  vcl_cout<<"q(cs1->cs3)="<<q->get(0)<<' '<<q->get(1)<<' '<<q->get(2)<<vcl_endl;
  delete q;

  delete parent3;
  delete parent2;
  delete parent1;
  delete motion3;
  delete motion2;
  delete motion1;
  delete interpolators3;
  delete interpolators2;
  delete interpolators1;
  delete cs3_beat;
  delete cs2_beat;
  delete beat1;
  delete cs1_beat;
}

//-----------------------------------------------------------------------------
// -- Entry point of the test program
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  int result;

  test1();

  result=0;
  return result;
}
