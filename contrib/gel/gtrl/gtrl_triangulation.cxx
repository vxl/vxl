// This is gel/gtrl/gtrl_triangulation.cxx
//:
// \file
// \author crossge@crd.ge.com

#include <utility>
#include "gtrl_triangulation.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// UNIX specific timer close
#ifdef _WIN32
#define NO_TIMER
#endif

// triangulation library
extern "C" {
#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */

#include <triangle.h> // from netlib, for triangulate()
}


gtrl_triangulation::gtrl_triangulation( gtrl_polygon poly)
  : poly_(std::move( poly))
{
}

void gtrl_triangulation::run()
{
  REAL *points= new REAL[poly_.size()*2];
  int *markers= new int[poly_.size()];

  // for some reason, the markers don't seem to
  //   like starting at 0
  constexpr int offset = 100;

  std::vector<gtrl_vertex_sptr> pointlist;

  for (int i=0; i< poly_.size(); i++)
    {
      gtrl_vertex_sptr p= poly_[i];
      pointlist.push_back( p);

      points[i*2]  = p->x();
      points[i*2+1]= p->y();
      markers[i]= i+offset;
    }

  // input
  triangulateio in;
  in.numberofpoints= poly_.size();
  in.pointlist= points;
  in.pointmarkerlist= markers;
  in.numberofpointattributes= 0;
  in.numberofholes= 0;
  in.trianglelist= nullptr;

  // output
  triangulateio out;
  out.pointlist= nullptr;
  out.trianglelist= nullptr;
  out.pointmarkerlist= nullptr;
  out.numberofpointattributes= 0;
  out.numberofholes= 0;
  out.numberoftriangleattributes= 0;

  // do triangulation
  triangulate( "-z -i -q", &in, &out, nullptr);

  // create any new points that are necessary
  for (int i=0; i< out.numberofpoints; i++)
    {
      if (out.pointmarkerlist[i]< offset)
        {
          pointlist.push_back( new gtrl_vertex( out.pointlist[i*2],
                                                out.pointlist[i*2+1]));

          out.pointmarkerlist[i]= pointlist.size()-1+ offset;
        }
    }

  // clean up from previous triangulation
  tris_.clear();

  // create the triangles
  for (int i=0; i< out.numberoftriangles; i++)
    {
      gtrl_triangle_sptr triangle= new gtrl_triangle( pointlist[ out.pointmarkerlist[ out.trianglelist[i*3]]- offset],
                                                     pointlist[ out.pointmarkerlist[ out.trianglelist[i*3+1]]- offset],
                                                     pointlist[ out.pointmarkerlist[ out.trianglelist[i*3+2]]- offset]);

      if (poly_.inside( triangle->mid_point()))
        tris_.push_back( triangle);
    }

  pts_= pointlist;

  // clean memory
  delete[] out.pointlist;
  delete[] out.pointmarkerlist;
  delete[] out.trianglelist;
  delete[] points;
  delete[] markers;
}
