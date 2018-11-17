// This is gel/gst/gst_polygon_2d.cxx
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include "gst_polygon_2d.h"
#include "gst_vertex_2d_sptr.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool gst_polygon_2d::check_validity() const
{
  // no edge list
  if (edges_.size()<1)
    return false;

  // cycle through edges, looking for a completed polygon
  gst_vertex_2d_sptr start= edges_[0]->get_start();
  gst_vertex_2d_sptr end  = edges_[0]->get_end();

  // length of cycle
  unsigned int clen= 1;

  while (end.ptr()!= start.ptr())
  {
      // next edge
      bool found= false;

      for (unsigned int i=0; i < edges_.size() && !found; i++)
      {
          if (edges_[i]->get_start().ptr()== end.ptr())
          {
              found= true;
              end= edges_[i]->get_end();
          }
      }

      // if !found then the cycle isn't closed
      if (!found) return false;

      clen++;
  }

  // check we have looked at all the edges
  return clen == edges_.size();
}

// returns the centroid
double gst_polygon_2d::get_centroid_x() const
{
  double xsum= 0;

  for (const auto & edge : edges_)
    xsum+= edge->get_start()->get_x();

  return xsum/edges_.size();
}

double gst_polygon_2d::get_centroid_y() const
{
  double ysum= 0;

  for (const auto & edge : edges_)
    ysum+= edge->get_start()->get_y();

  return ysum/edges_.size();
}


// returns the area (signed)
//        2 A( P ) = sum_{i=0}^{n-1} (x_i y_{i+1} - y_i x_{i+1}).
double gst_polygon_2d::area() const
{
  double a= 0;

  for (unsigned int i=0; i< edges_.size(); ++i)
  {
    int ip1=((i+1)==edges_.size())?0:(i+1);

    double dp = edges_[i]->get_start()->get_x()* edges_[ip1]->get_start()->get_y()
              - edges_[i]->get_start()->get_x()* edges_[ip1]->get_start()->get_x();

    a+= dp;
  }

  return a/2;
}


// simple and efficient point in polygon test.
//   from comp.graphics.algorithms faq
//   should only call if validity passes (no check
//   for efficiency)
bool gst_polygon_2d::inside( const double x, const double y) const
{
  bool c= false;

  for (unsigned int i=0, j= edges_.size()-1; i< edges_.size(); j= i++)
  {
    if ((((edges_[i]->get_start()->get_y()<= y) &&
          (y< edges_[j]->get_start()->get_y())) ||
         ((edges_[j]->get_start()->get_y()<= y) &&
          (y< edges_[i]->get_start()->get_y()))) &&
        (x< (edges_[j]->get_start()->get_x() -
             edges_[i]->get_start()->get_x()) * (y -
                                                 edges_[i]->get_start()->get_y()) /
         (edges_[j]->get_start()->get_y() - edges_[i]->get_start()->get_y()) +
         edges_[i]->get_start()->get_x()))
    {
      c=!c;
    }
  }

  return c;
}

bool gst_polygon_2d::inside( const gst_vertex_2d_sptr& v) const
{
  return inside( v->get_x(), v->get_y());
}


std::ostream &operator<<( std::ostream &os, gst_polygon_2d &p)
{
  for (auto & edge : p.edges_)
    os << (*edge) << ' ';

  return os << std::endl;
}
