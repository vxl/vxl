#include "msm_draw_shape_to_eps.h"
//:
// \file
// \brief Draws current shape instance to an eps file
// \author Tim Cootes

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

//: Draws current shape instance to an eps file
//  Uses writer.draw_polygon() function to write
//  given curves to file in current colour, linewidth etc.
void msm_draw_shape_to_eps(mbl_eps_writer& writer,
                           const msm_points& points,
                           const msm_curves& curves)
{
  vcl_vector<vgl_point_2d<double> > pts;
  points.get_points(pts);

  for (unsigned c=0;c<curves.size();++c)
  {
    const vcl_vector<unsigned>& index = curves[c].index();
    vcl_vector<vgl_point_2d<double> > p(index.size());
    for (unsigned i=0;i<index.size();++i)
    {
      if (index[i]>=pts.size())
        vcl_cerr<<"Illegal index: "<<index[i]<<vcl_endl;
      assert(index[i]<pts.size());
      p[i]=pts[index[i]];
    }
    writer.draw_polygon(p,!curves[c].open(),false);
  }
}

//: Draws points to an eps file with given radius
//  Uses writer.draw_disk() function to write points
//  in current colour.
void msm_draw_points_to_eps(mbl_eps_writer& writer,
                            const msm_points& points,
                            double radius, bool filled)
{
  for (unsigned i=0;i<points.size();++i)
  {
    if (filled)
      writer.draw_disk(points[i],radius);
    else
      writer.draw_circle(points[i],radius);
  }
}

