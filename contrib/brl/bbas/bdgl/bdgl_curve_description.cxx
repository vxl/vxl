//:
// \file
#include "bdgl_curve_description.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vdgl/vdgl_edgel_chain.h>


//: Constructor
bdgl_curve_description::bdgl_curve_description()
{
  center_.set(0.0, 0.0);
  curvature_ = 0.0;
  length_ = 0.0;
  gradient_mean_val_ = 0.0;
  gradient_mean_dir_ = 0.0;
  gradient_std_val_ = 0.0;
  gradient_std_dir_ = 0.0;
}


//: Constructor : compute description from a digital curve and a given order of description
void bdgl_curve_description::init(vdgl_edgel_chain_sptr const& ec)
{
  if (!ec)
  {
    vcl_cout<<"In bdgl_curve_description::bdgl_curve_description(..) - warning, null chain\n";
    return;
  }

  center_.set(0.0, 0.0);
  curvature_ = 0.0;
  int N = ec->size();
  double m_val=0.0, m_dir=0.0, m_curv=0.0, curv;
  vdgl_edgel ed,ep,en;

  // means
  for (int i = 0; i<N; i++) {
    ed = ec->edgel(i);

    center_.set(center_.x() + ed.get_x(), center_.y() + ed.get_y());
    m_val += ed.get_grad();
    m_dir += ed.get_theta();
    //vcl_cout<<"grad:("<<ed.get_grad()<<", "<<ed.get_theta()<<")\n";
  }
  center_.set( center_.x()/(double)N , center_.y()/(double)N );
  gradient_mean_val_ = (m_val/(double)N);
  gradient_mean_dir_ = (m_dir/(double)N);

  // standard deviations
  m_val = 0.0;
  m_dir = 0.0;
  if (N>1){
    for (int i = 0; i<N; i++) {
      ed = ec->edgel(i);

      m_val += vnl_math_sqr(ed.get_grad()-gradient_mean_val_);
      m_dir += vnl_math_sqr(ed.get_theta()-gradient_mean_dir_);
    }
    gradient_std_val_ = vcl_sqrt(m_val/(double)(N-1));
    gradient_std_dir_ = vcl_sqrt(m_dir/(double)(N-1));
  } else {
    gradient_std_val_ = 0.0;
    gradient_std_dir_ = 0.0;
  }

  // length
  double dist=0.0;
  if (N>1) {
    for (int i = 1; i<N; i++) {
      ed = ec->edgel(i);
      ep = ec->edgel(i-1);

      dist += vcl_sqrt( (ed.x() - ep.x())*(ed.x() - ep.x()) + (ed.y() - ep.y())*(ed.y() - ep.y()) );
    }
    length_ = (dist);
  } else {
    length_ = 0.0;
  }
  // curvature estimate
  if (N>2) {
    for (int i = 1; i<N-1; i++) {
      ed = ec->edgel(i);
      ep = ec->edgel(i-1);
      en = ec->edgel(i+1);

      // sort of curvature function ... to be properly defined later
      curv = ( 2.0*ed.x() - ep.x() - en.x() )
            *( 2.0*ed.x() - ep.x() - en.x() )
           + ( 2.0*ed.y() - ep.y() - en.y() )
            *( 2.0*ed.y() - ep.y() - en.y() );
      //vcl_cout<<curv<<"\n";
      m_curv += vcl_sqrt( curv );
    }
    curvature_ = (m_curv/(double)(N-2));
  } else {
    curvature_ = 0.0;
  }
  return;
}

// text description
void bdgl_curve_description::info()
{
  vcl_cout<<"curve: c ("<<center_.x()<<", "<<center_.y()<<") | l "<<length_<<" | grad "
          <<gradient_mean_val_<<" th "<<gradient_mean_dir_<<" | dg "
          <<gradient_std_val_<<" dth "<<gradient_std_dir_<<" | curv "<<curvature_<<"\n";

  return;
}

