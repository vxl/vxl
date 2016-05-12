#ifndef msm_draw_shape_to_eps_h_
#define msm_draw_shape_to_eps_h_
//:
// \file
// \brief Draws current shape instance to an eps file
// \author Tim Cootes

#include <mbl/mbl_eps_writer.h>
#include <msm/msm_points.h>
#include <msm/msm_curve.h>

//: Draws current shape instance to an eps file
//  Uses writer.draw_polygon() function to write
//  given curves to file in current colour, linewidth etc.
void msm_draw_shape_to_eps(mbl_eps_writer& writer,
                           const msm_points& points,
                           const msm_curves& curves);

//: Draws points to an eps file with given radius
//  Uses writer.draw_disk() or .draw_circle() function to write
//  points in current colour.
void msm_draw_points_to_eps(mbl_eps_writer& writer,
                           const msm_points& points,
                           double radius, bool filled=true);

#endif // msm_draw_shape_to_eps_h_
