/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gst_polygon_2d"
#endif

#include <gst/gst_vertex_2d_ref.h>

#include "gst_polygon_2d.h"


bool gst_polygon_2d::check_validity() const
{
  // no edge list
  if( edges_.size()<1)
    return false;

  // cycle through edges, looking for a completed polygon
  gst_vertex_2d_ref start= edges_[0]->get_start();
  gst_vertex_2d_ref end  = edges_[0]->get_end();

  // length of cycle
  int clen= 1;

  while( end.ptr()!= start.ptr())
    {
      // next edge
      bool found= false;

      for( int i=0; ((i< edges_.size()) && !found); i++)
	{
	  if( edges_[i]->get_start().ptr()== end.ptr())
	    {
	      found= true;
	      end= edges_[i]->get_end();
	    }
	}
      
      // if !found then the cycle isn't closed
      if( !found) return false;

      clen++;
    }

  // check we have looked at all the edges
  if( clen== edges_.size()) return true;

  return false;
}

// simple and efficient point in polygon test.
//   from comp.graphics.algorithms faq
//   should only call if validity passes (no check
//   for efficiency)
bool gst_polygon_2d::inside( const gst_vertex_2d_ref v) const
{
  bool c= false;

  for( int i=0, j= edges_.size()-1; i< edges_.size(); j= i++)
    {
      if ((((edges_[i]->get_start()->get_y()<= v->get_y()) && 
	    (v->get_y()< edges_[j]->get_start()->get_y())) ||
	   ((edges_[j]->get_start()->get_y()<= v->get_y()) && 
	    (v->get_y()< edges_[i]->get_start()->get_y()))) &&
	  (v->get_x()< (edges_[j]->get_start()->get_x() - 
			edges_[i]->get_start()->get_x()) * (v->get_y() - 
							    edges_[i]->get_start()->get_y()) / 
	   (edges_[j]->get_start()->get_y() - edges_[i]->get_start()->get_y()) + 
	   edges_[i]->get_start()->get_x()))
	{
	  c=!c;
	}
      
    }
  
  return c;
}


ostream &operator<<( ostream &os, gst_polygon_2d &p)
{
  for( int i=0; i< p.edges_.size(); i++)
    {
      os << (*p.edges_[i]) << " ";
    }

  return os << endl;
}
