//*****************************************************************************
// File name: example1.cxx
// Description: Builds 3 coordinates systems:
// acs: absolute coordinate system
// cs0: coordinate system defined from acs by static translation v0=(5,0,0)
// lcs: local coordinate system of an object
//    * between t0 and t1, lcs is defined from acs with a dynamic translation
//      t0: v=(0,0,0)
//      t1: v=v0
//    * between t1 and t3, lcs is defined from cs0 with a dynamic displacement
//      t1: point=(0,5,0), vector=(0,0,1), angle=0
//      t2: point=(0,5,0), vector=(0,0,1), angle=0.1*pi
//      t3: point=(0,5,0), vector=(0,0,1), angle=2*pi
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/08/27| François BERTEL          |Creation
// 1.1     |2002/01/22| Peter Vanroose           |Made compiling with gcc 3.0
//*****************************************************************************

//:
// \file

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_displacement.h>
#include <vcsl/vcsl_translation.h>
#include <vcsl/vcsl_graph.h>
#include <vnl/vnl_math.h> // for pi

//-----------------------------------------------------------------------------
//: Entry point of the example program
//-----------------------------------------------------------------------------
int main()
{
  const double t0=0;
  const double t1=0.4;
  const double t2=0.6;
  const double t3=1;

  // Graph and coordinate systems instantiations
  vcl_cout<<"Creation of graph..."<< vcl_flush;
  vcsl_graph_sptr graph=new vcsl_graph;
  assert(graph.ptr()!=0);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of acs..."<< vcl_flush;
  vcsl_spatial_sptr acs=new vcsl_cartesian_3d;
  assert(acs.ptr()!=0);
  vcl_cout<<acs.ptr()<<" done\n"
          <<"Adding acs to graph..."<< vcl_flush;
  acs->set_graph(graph);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of cs0..."<< vcl_flush;
  vcsl_spatial_sptr cs0=new vcsl_cartesian_3d;
  assert(cs0.ptr()!=0);
  vcl_cout<<cs0.ptr()<<" done\n"
          <<"Adding cs0 to graph..."<< vcl_flush;
  cs0->set_graph(graph);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of lcs..."<< vcl_flush;
  vcsl_spatial_sptr lcs=new vcsl_cartesian_3d;
  assert(lcs.ptr()!=0);
  vcl_cout<<lcs.ptr()<<" done\n"
          <<"Adding lcs to graph..."<< vcl_flush;
  lcs->set_graph(graph);
  vcl_cout<<" done\n";

  // Static transformation between acs and cs0
  vcl_cout<<"Creation of the static translation..."<< vcl_flush;
  vcsl_translation_sptr static_t=new vcsl_translation;
  assert(static_t.ptr()!=0);
  vcl_cout<<static_t.ptr()<<" done\n"
          <<"Creation of v0..."<< vcl_flush;
  vnl_vector<double> v0(3);
  vcl_cout<<" done\n"
          <<"Initialization of v0..."<< vcl_flush;
  v0.put(0,5);
  v0.put(1,0);
  v0.put(2,0);
  assert(v0.get(0)==5);
  assert(v0.get(1)==0);
  assert(v0.get(2)==0);
  vcl_cout<<" done\n"
          <<"Initialization of the static translation with v0..."<< vcl_flush;
  static_t->set_static(v0);
  assert(static_t->vector().size()==1);
  assert(static_t->vector()[0]==v0);
  vcl_cout<<" done\n";

  vcl_cout<<"Linking cs0 and acs with the static translation..."<< vcl_flush;
  cs0->set_unique(acs,static_t.ptr());
  vcl_cout<<" done\n";

  // Movement of lcs
  // 1. translation
  vcl_cout<<"Creation of the dynamic translation..."<< vcl_flush;
  vcsl_translation_sptr dynamic_t=new vcsl_translation;
  assert(dynamic_t.ptr()!=0);
  vcl_cout<<dynamic_t.ptr()<<" done\n";

  vcl_cout<<"Creation of t_beat..."<< vcl_flush;
  vcl_vector<double> t_beat;
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of t_beat..."<< vcl_flush;
  t_beat.push_back(t0);
  t_beat.push_back(t1);
  assert(t_beat.size()==2);
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of the dynamic translation with t_beat..."<< vcl_flush;
  dynamic_t->set_beat(t_beat);
  assert(dynamic_t->beat()==t_beat);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of v_null..."<< vcl_flush;
  vnl_vector<double> v_null(3);
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of v_null..."<< vcl_flush;
  v_null.put(0,0);
  v_null.put(1,0);
  v_null.put(2,0);
  assert(v_null.get(0)==0);
  assert(v_null.get(1)==0);
  assert(v_null.get(2)==0);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of t_vectors..."<< vcl_flush;
  vcl_vector<vnl_vector<double> > t_vectors;
  vcl_cout<<" done\n"
          <<"Initialization of t_vectors with v_null and v0..."<< vcl_flush;
  t_vectors.push_back(v_null);
  t_vectors.push_back(v0);
  assert(t_vectors.size()==2);
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of the dynamic translation with t_vectors..."<< vcl_flush;
  dynamic_t->set_vector(t_vectors);
  assert(dynamic_t->vector()==t_vectors);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of t_inter..."<< vcl_flush;
  vcl_vector<vcsl_interpolator> t_inter;
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of t_inter..."<< vcl_flush;
  t_inter.push_back(vcsl_linear);
  assert(t_inter.size()==1);
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of the dynamic translation with t_inter..."<< vcl_flush;
  dynamic_t->set_interpolators(t_inter);
  assert(dynamic_t->interpolators()==t_inter);
  vcl_cout<<" done\n";

  // 2. displacement
  vcl_cout<<"Creation of the dynamic displacement..."<< vcl_flush;
  vcsl_displacement_sptr dynamic_d=new vcsl_displacement;
  assert(dynamic_d.ptr()!=0);
  vcl_cout<<dynamic_d.ptr()<<" done\n";

  vcl_cout<<"Creation of d_beat..."<< vcl_flush;
  vcl_vector<double> d_beat;
  vcl_cout<<" done\n"
          <<"Initialization of d_beat..."<< vcl_flush;
  d_beat.push_back(t1);
  d_beat.push_back(t2);
  d_beat.push_back(t3);
  assert(d_beat.size()==3);
  vcl_cout<<" done\n"
          <<"Initialization of the dynamic displacement with d_beat..."<< vcl_flush;
  dynamic_d->set_beat(d_beat);
  assert(dynamic_d->beat()==d_beat);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of d_points..."<< vcl_flush;
  vcl_vector<vnl_vector<double> > d_points;
  vcl_cout<<" done\n"
          <<"Creation of point..."<< vcl_flush;
  vnl_vector<double> point(3);
  vcl_cout<<" done\n"
          <<"Initialization of point..."<< vcl_flush;
  point.put(0,0);
  point.put(1,5);
  point.put(2,0);
  assert(point.get(0)==0);
  assert(point.get(1)==5);
  assert(point.get(2)==0);
  vcl_cout<<" done\n"
          <<"Initialization of the d_points with point..."<< vcl_flush;
  d_points.push_back(point);
  d_points.push_back(point);
  d_points.push_back(point);
  assert(d_points.size()==3);
  vcl_cout<<" done\n"
          <<"Initialization of the dynamic displacement with d_points..."<< vcl_flush;
  dynamic_d->set_point(d_points);
  assert(dynamic_d->point()==d_points);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of d_axes..."<< vcl_flush;
  vcl_vector<vnl_vector<double> > d_axes;
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of axis..."<< vcl_flush;
  vnl_vector<double> axis(3);
  vcl_cout<<" done\n"
          <<"Initialization of axis..."<< vcl_flush;
  axis.put(0,0);
  axis.put(1,0);
  axis.put(2,1);
  assert(axis.get(0)==0);
  assert(axis.get(1)==0);
  assert(axis.get(2)==1);
  vcl_cout<<" done\n"
          <<"Initialization of the d_axes with axis..."<< vcl_flush;
  d_axes.push_back(axis);
  d_axes.push_back(axis);
  d_axes.push_back(axis);
  assert(d_axes.size()==3);
  vcl_cout<<" done\n"
          <<"Initialization of the dynamic displacement with d_axes..."<< vcl_flush;
  dynamic_d->set_axis(d_axes);
  assert(dynamic_d->axis()==d_axes);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of d_angles..."<< vcl_flush;
  vcl_vector<double> d_angles;
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of the d_angles..."<< vcl_flush;
  d_angles.push_back(0);
  d_angles.push_back(0.1*vnl_math::pi);
  d_angles.push_back(2*vnl_math::pi);
  assert(d_angles.size()==3);
  vcl_cout<<" done\n"
          <<"Initialization of the dynamic displacement with d_angles..."<< vcl_flush;
  dynamic_d->set_angle(d_angles);
  assert(dynamic_d->angle()==d_angles);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of d_inter..."<< vcl_flush;
  vcl_vector<vcsl_interpolator> d_inter;
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of d_inter..."<< vcl_flush;
  d_inter.push_back(vcsl_linear);
  d_inter.push_back(vcsl_linear);
  assert(d_inter.size()==2);
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of the dynamic displacement with d_inter..."<< vcl_flush;
  dynamic_d->set_interpolators(d_inter);
  assert(dynamic_d->interpolators()==d_inter);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of motion..."<< vcl_flush;
  vcl_vector<vcsl_spatial_transformation_sptr> motion;
  vcl_cout<<" done\n"
          <<"Initialization of motion..."<< vcl_flush;
  motion.push_back(dynamic_t.ptr());
  motion.push_back(dynamic_d.ptr());
  assert(motion.size()==2);
  vcl_cout<<" done\n";

  vcl_cout<<"Linking lcs and the motion..."<< vcl_flush;
  lcs->set_motion(motion);
  assert(lcs->motion()==motion);
  vcl_cout<<" done1\n";

  vcl_cout<<"Creation of l_parents..."<< vcl_flush;
  vcl_vector<vcsl_spatial_sptr> l_parents;
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of l_parents..."<< vcl_flush;
  l_parents.push_back(acs);
  l_parents.push_back(cs0);
  assert(l_parents.size()==2);
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of lcs with l_parents..."<< vcl_flush;
  lcs->set_parent(l_parents);
  assert(lcs->parent()==l_parents);
  vcl_cout<<" done\n";

  vcl_cout<<"Creation of l_beat..."<< vcl_flush;
  vcl_vector<double> l_beat;
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of l_beat..."<< vcl_flush;
  l_beat.push_back(t0);
  l_beat.push_back(t1);
  l_beat.push_back(t3);
  assert(l_beat.size()==3);
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of lcs with l_beat..."<< vcl_flush;
  lcs->set_beat(l_beat);
  assert(lcs->beat()==l_beat);
  vcl_cout<<" done\n";

  // Main query

  vcl_cout<<"Creation of p..."<< vcl_flush;
  vnl_vector<double> p(3);
  vcl_cout<<" done\n";

  vcl_cout<<"Initialization of p..."<< vcl_flush;
  p.put(0,1);
  p.put(1,2);
  p.put(2,3);
  assert(p.get(0)==1);
  assert(p.get(1)==2);
  assert(p.get(2)==3);
  vcl_cout<<" done\n";

  vnl_vector<double> q=lcs->from_local_to_cs(p,acs,t0);
  vcl_cout<<"q(lcs->acs,t0)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=lcs->from_local_to_cs(p,acs,0.2);
  vcl_cout<<"q(lcs->acs,0.2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=lcs->from_local_to_cs(p,acs,t1);
  vcl_cout<<"q(lcs->acs,t1)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=lcs->from_local_to_cs(p,acs,t2);
  vcl_cout<<"q(lcs->acs,t2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=lcs->from_local_to_cs(p,acs,0.7);
  vcl_cout<<"q(lcs->acs,0.7)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  q=lcs->from_local_to_cs(p,acs,t3);
  vcl_cout<<"q(lcs->acs,t3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<vcl_endl;

  return 0;
}
