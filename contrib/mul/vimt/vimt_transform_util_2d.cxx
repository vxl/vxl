#include "vimt_transform_util_2d.h"
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute similarity transform which maps the p1,p2 to q1,q2
void vimt_calc_transform_2d( vimt_transform_2d& trans,
                             const vgl_point_2d<double>& p1,
                             const vgl_point_2d<double>& p2,
                             const vgl_point_2d<double>& q1,
                             const vgl_point_2d<double>& q2)
{
  vgl_point_2d<double> pc = centre(p1,p2);
  vgl_point_2d<double> qc = centre(q1,q2);
  vgl_vector_2d<double> dp = p2-p1;
  vgl_vector_2d<double> dq = q2-q1;

  double dp2 = dp.x()*dp.x() + dp.y()*dp.y();
  assert(dp2>0);

  double a = (dp.x()*dq.x() + dp.y()*dq.y())/dp2;
  double b = (dp.x()*dq.y() - dp.y()*dq.x())/dp2;

  double tx = qc.x() - (a*pc.x()-b*pc.y());
  double ty = qc.y() - (b*pc.x()+a*pc.y());

  trans.set_similarity(vgl_point_2d<double>(a,b), vgl_point_2d<double>(tx,ty));
}


//: Transform 2d box
void vimt_transform_util_2d( vgl_box_2d<double>& dest_box,
                             const vgl_box_2d<double>& src_box,
                             const vimt_transform_2d& trans )
{
  vgl_point_2d<double> tl( src_box.min_x(), src_box.min_y() );
  vgl_point_2d<double> tl_pt= trans ( tl );

  vgl_point_2d<double> br( src_box.max_x(), src_box.max_y() );
  vgl_point_2d<double> br_pt= trans ( br );

  dest_box.set_min_point ( tl_pt );
  dest_box.set_max_point ( br_pt );
}

//: Transform vector of 2d pts
void vimt_transform_util_2d( std::vector< vgl_point_2d<double> >& dest_pt_vec,
                             const std::vector< vgl_point_2d<double> >& src_pt_vec,
                             const vimt_transform_2d& trans )
{
  int n= src_pt_vec.size();
  dest_pt_vec.resize(n);
  for (int i=0; i<n; ++i)
  {
    dest_pt_vec[i] = trans ( src_pt_vec[i] );
  }
}
