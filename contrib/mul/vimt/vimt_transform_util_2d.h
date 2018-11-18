#ifndef vimt_transform_util_2d_h_
#define vimt_transform_util_2d_h_

//: \file
//  \brief Functions using vimt_transform_2d
//  \author dac

#include <vimt/vimt_transform_2d.h>

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>

//: Compute similarity transform which maps the p1,p2 to q1,q2
void vimt_calc_transform_2d(  vimt_transform_2d& tran,
                                  const vgl_point_2d<double>& p1,
                                    const vgl_point_2d<double>& p2,
                                    const vgl_point_2d<double>& q1,
                                    const vgl_point_2d<double>& q2);


//: Transform 2d box
void vimt_transform_util_2d( vgl_box_2d<double>& dest_box,
                             const vgl_box_2d<double>& src_box,
                                  const vimt_transform_2d& trans );


//: Transform vector of 2d pts
void vimt_transform_util_2d( std::vector< vgl_point_2d<double> >& dest_pt_vec,
                             const std::vector< vgl_point_2d<double> >& src_pt_vec,
                                  const vimt_transform_2d& trans );




#endif
