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
// 1.0     |2000/08/27| Francois BERTEL          |Creation
// 1.1     |2002/01/22| Peter Vanroose           |Made compiling with gcc 3.0
//*****************************************************************************

//:
// \file

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
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
  constexpr double t0 = 0;
  constexpr double t1 = 0.4;
  constexpr double t2 = 0.6;
  constexpr double t3 = 1;

  // Graph and coordinate systems instantiations
  std::cout<<"Creation of graph..."<< std::flush;
  vcsl_graph_sptr graph=new vcsl_graph;
  assert(graph.ptr()!=nullptr);
  std::cout<<" done\n";

  std::cout<<"Creation of acs..."<< std::flush;
  vcsl_spatial_sptr acs=new vcsl_cartesian_3d;
  assert(acs.ptr()!=nullptr);
  std::cout<<acs.ptr()<<" done\n"
          <<"Adding acs to graph..."<< std::flush;
  acs->set_graph(graph);
  std::cout<<" done\n";

  std::cout<<"Creation of cs0..."<< std::flush;
  vcsl_spatial_sptr cs0=new vcsl_cartesian_3d;
  assert(cs0.ptr()!=nullptr);
  std::cout<<cs0.ptr()<<" done\n"
          <<"Adding cs0 to graph..."<< std::flush;
  cs0->set_graph(graph);
  std::cout<<" done\n";

  std::cout<<"Creation of lcs..."<< std::flush;
  vcsl_spatial_sptr lcs=new vcsl_cartesian_3d;
  assert(lcs.ptr()!=nullptr);
  std::cout<<lcs.ptr()<<" done\n"
          <<"Adding lcs to graph..."<< std::flush;
  lcs->set_graph(graph);
  std::cout<<" done\n";

  // Static transformation between acs and cs0
  std::cout<<"Creation of the static translation..."<< std::flush;
  vcsl_translation_sptr static_t=new vcsl_translation;
  assert(static_t.ptr()!=nullptr);
  std::cout<<static_t.ptr()<<" done\n"
          <<"Creation of v0..."<< std::flush;
  vnl_vector<double> v0(3);
  std::cout<<" done\n"
          <<"Initialization of v0..."<< std::flush;
  v0.put(0,5);
  v0.put(1,0);
  v0.put(2,0);
  assert(v0.get(0)==5);
  assert(v0.get(1)==0);
  assert(v0.get(2)==0);
  std::cout<<" done\n"
          <<"Initialization of the static translation with v0..."<< std::flush;
  static_t->set_static(v0);
  assert(static_t->vector().size()==1);
  assert(static_t->vector()[0]==v0);
  std::cout<<" done\n";

  std::cout<<"Linking cs0 and acs with the static translation..."<< std::flush;
  cs0->set_unique(acs,static_t.ptr());
  std::cout<<" done\n";

  // Movement of lcs
  // 1. translation
  std::cout<<"Creation of the dynamic translation..."<< std::flush;
  vcsl_translation_sptr dynamic_t=new vcsl_translation;
  assert(dynamic_t.ptr()!=nullptr);
  std::cout<<dynamic_t.ptr()<<" done\n";

  std::cout<<"Creation of t_beat..."<< std::flush;
  std::vector<double> t_beat;
  std::cout<<" done\n";

  std::cout<<"Initialization of t_beat..."<< std::flush;
  t_beat.push_back(t0);
  t_beat.push_back(t1);
  assert(t_beat.size()==2);
  std::cout<<" done\n";

  std::cout<<"Initialization of the dynamic translation with t_beat..."<< std::flush;
  dynamic_t->set_beat(t_beat);
  assert(dynamic_t->beat()==t_beat);
  std::cout<<" done\n";

  std::cout<<"Creation of v_null..."<< std::flush;
  vnl_vector<double> v_null(3);
  std::cout<<" done\n";

  std::cout<<"Initialization of v_null..."<< std::flush;
  v_null.put(0,0);
  v_null.put(1,0);
  v_null.put(2,0);
  assert(v_null.get(0)==0);
  assert(v_null.get(1)==0);
  assert(v_null.get(2)==0);
  std::cout<<" done\n";

  std::cout<<"Creation of t_vectors..."<< std::flush;
  std::vector<vnl_vector<double> > t_vectors;
  std::cout<<" done\n"
          <<"Initialization of t_vectors with v_null and v0..."<< std::flush;
  t_vectors.push_back(v_null);
  t_vectors.push_back(v0);
  assert(t_vectors.size()==2);
  std::cout<<" done\n";

  std::cout<<"Initialization of the dynamic translation with t_vectors..."<< std::flush;
  dynamic_t->set_vector(t_vectors);
  assert(dynamic_t->vector()==t_vectors);
  std::cout<<" done\n";

  std::cout<<"Creation of t_inter..."<< std::flush;
  std::vector<vcsl_interpolator> t_inter;
  std::cout<<" done\n";

  std::cout<<"Initialization of t_inter..."<< std::flush;
  t_inter.push_back(vcsl_linear);
  assert(t_inter.size()==1);
  std::cout<<" done\n";

  std::cout<<"Initialization of the dynamic translation with t_inter..."<< std::flush;
  dynamic_t->set_interpolators(t_inter);
  assert(dynamic_t->interpolators()==t_inter);
  std::cout<<" done\n";

  // 2. displacement
  std::cout<<"Creation of the dynamic displacement..."<< std::flush;
  vcsl_displacement_sptr dynamic_d=new vcsl_displacement;
  assert(dynamic_d.ptr()!=nullptr);
  std::cout<<dynamic_d.ptr()<<" done\n";

  std::cout<<"Creation of d_beat..."<< std::flush;
  std::vector<double> d_beat;
  std::cout<<" done\n"
          <<"Initialization of d_beat..."<< std::flush;
  d_beat.push_back(t1);
  d_beat.push_back(t2);
  d_beat.push_back(t3);
  assert(d_beat.size()==3);
  std::cout<<" done\n"
          <<"Initialization of the dynamic displacement with d_beat..."<< std::flush;
  dynamic_d->set_beat(d_beat);
  assert(dynamic_d->beat()==d_beat);
  std::cout<<" done\n";

  std::cout<<"Creation of d_points..."<< std::flush;
  std::vector<vnl_vector<double> > d_points;
  std::cout<<" done\n"
          <<"Creation of point..."<< std::flush;
  vnl_vector<double> point(3);
  std::cout<<" done\n"
          <<"Initialization of point..."<< std::flush;
  point.put(0,0);
  point.put(1,5);
  point.put(2,0);
  assert(point.get(0)==0);
  assert(point.get(1)==5);
  assert(point.get(2)==0);
  std::cout<<" done\n"
          <<"Initialization of the d_points with point..."<< std::flush;
  d_points.push_back(point);
  d_points.push_back(point);
  d_points.push_back(point);
  assert(d_points.size()==3);
  std::cout<<" done\n"
          <<"Initialization of the dynamic displacement with d_points..."<< std::flush;
  dynamic_d->set_point(d_points);
  assert(dynamic_d->point()==d_points);
  std::cout<<" done\n";

  std::cout<<"Creation of d_axes..."<< std::flush;
  std::vector<vnl_vector<double> > d_axes;
  std::cout<<" done\n";

  std::cout<<"Creation of axis..."<< std::flush;
  vnl_vector<double> axis(3);
  std::cout<<" done\n"
          <<"Initialization of axis..."<< std::flush;
  axis.put(0,0);
  axis.put(1,0);
  axis.put(2,1);
  assert(axis.get(0)==0);
  assert(axis.get(1)==0);
  assert(axis.get(2)==1);
  std::cout<<" done\n"
          <<"Initialization of the d_axes with axis..."<< std::flush;
  d_axes.push_back(axis);
  d_axes.push_back(axis);
  d_axes.push_back(axis);
  assert(d_axes.size()==3);
  std::cout<<" done\n"
          <<"Initialization of the dynamic displacement with d_axes..."<< std::flush;
  dynamic_d->set_axis(d_axes);
  assert(dynamic_d->axis()==d_axes);
  std::cout<<" done\n";

  std::cout<<"Creation of d_angles..."<< std::flush;
  std::vector<double> d_angles;
  std::cout<<" done\n";

  std::cout<<"Initialization of the d_angles..."<< std::flush;
  d_angles.push_back(0);
  d_angles.push_back(0.1*vnl_math::pi);
  d_angles.push_back(2*vnl_math::pi);
  assert(d_angles.size()==3);
  std::cout<<" done\n"
          <<"Initialization of the dynamic displacement with d_angles..."<< std::flush;
  dynamic_d->set_angle(d_angles);
  assert(dynamic_d->angle()==d_angles);
  std::cout<<" done\n";

  std::cout<<"Creation of d_inter..."<< std::flush;
  std::vector<vcsl_interpolator> d_inter;
  std::cout<<" done\n";

  std::cout<<"Initialization of d_inter..."<< std::flush;
  d_inter.push_back(vcsl_linear);
  d_inter.push_back(vcsl_linear);
  assert(d_inter.size()==2);
  std::cout<<" done\n";

  std::cout<<"Initialization of the dynamic displacement with d_inter..."<< std::flush;
  dynamic_d->set_interpolators(d_inter);
  assert(dynamic_d->interpolators()==d_inter);
  std::cout<<" done\n";

  std::cout<<"Creation of motion..."<< std::flush;
  std::vector<vcsl_spatial_transformation_sptr> motion;
  std::cout<<" done\n"
          <<"Initialization of motion..."<< std::flush;
  motion.emplace_back(dynamic_t.ptr());
  motion.emplace_back(dynamic_d.ptr());
  assert(motion.size()==2);
  std::cout<<" done\n";

  std::cout<<"Linking lcs and the motion..."<< std::flush;
  lcs->set_motion(motion);
  assert(lcs->motion()==motion);
  std::cout<<" done1\n";

  std::cout<<"Creation of l_parents..."<< std::flush;
  std::vector<vcsl_spatial_sptr> l_parents;
  std::cout<<" done\n";

  std::cout<<"Initialization of l_parents..."<< std::flush;
  l_parents.push_back(acs);
  l_parents.push_back(cs0);
  assert(l_parents.size()==2);
  std::cout<<" done\n";

  std::cout<<"Initialization of lcs with l_parents..."<< std::flush;
  lcs->set_parent(l_parents);
  assert(lcs->parent()==l_parents);
  std::cout<<" done\n";

  std::cout<<"Creation of l_beat..."<< std::flush;
  std::vector<double> l_beat;
  std::cout<<" done\n";

  std::cout<<"Initialization of l_beat..."<< std::flush;
  l_beat.push_back(t0);
  l_beat.push_back(t1);
  l_beat.push_back(t3);
  assert(l_beat.size()==3);
  std::cout<<" done\n";

  std::cout<<"Initialization of lcs with l_beat..."<< std::flush;
  lcs->set_beat(l_beat);
  assert(lcs->beat()==l_beat);
  std::cout<<" done\n";

  // Main query

  std::cout<<"Creation of p..."<< std::flush;
  vnl_vector<double> p(3);
  std::cout<<" done\n";

  std::cout<<"Initialization of p..."<< std::flush;
  p.put(0,1);
  p.put(1,2);
  p.put(2,3);
  assert(p.get(0)==1);
  assert(p.get(1)==2);
  assert(p.get(2)==3);
  std::cout<<" done\n";

  vnl_vector<double> q=lcs->from_local_to_cs(p,acs,t0);
  std::cout<<"q(lcs->acs,t0)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=lcs->from_local_to_cs(p,acs,0.2);
  std::cout<<"q(lcs->acs,0.2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=lcs->from_local_to_cs(p,acs,t1);
  std::cout<<"q(lcs->acs,t1)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=lcs->from_local_to_cs(p,acs,t2);
  std::cout<<"q(lcs->acs,t2)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=lcs->from_local_to_cs(p,acs,0.7);
  std::cout<<"q(lcs->acs,0.7)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  q=lcs->from_local_to_cs(p,acs,t3);
  std::cout<<"q(lcs->acs,t3)="<<q.get(0)<<' '<<q.get(1)<<' '<<q.get(2)<<std::endl;

  return 0;
}
