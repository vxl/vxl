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
