#ifndef vgl_polygon_scan_iterator_h
#define vgl_polygon_scan_iterator_h

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>

// .NAME vgl_polygon_scan_iterator - Fill a polygonal face with interior scan lines
// .INCLUDE vgl/vgl_polygon_scan_iterator.h
// .FILE vgl_polygon_scan_iterator.cxx
//
// .SECTION Description
//  This class provides an interator-style interface to polygon scan
//  conversion.  There are convenient constructors from vgl_polygon, and_
//  lists of floats.  An auxillary clipping window can be specified by the
//  constructor argument, IUBox* win.
//
// Concave Polygon Scan Conversion
// by Paul Heckbert
// from "Graphics Gems", Academic Press, 1990
//
// Scan convert nvert-sided concave non-simple polygon
// with vertices at  (point[i].x, point[i].y)
// for i in [0..nvert-1] within the window win by
// calling spanproc for each visible span of pixels.
// Polygon can be clockwise or counterclockwise.
// Algorithm does uniform point sampling at pixel centers.
// Inside-outside test done by Jordan's rule: a point is considered inside if
// an emanating ray intersects the polygon an odd number of times.
//
// Note: The span limits, startx and endx,  are closed intervals.
// That is, you can use the endpoints of the span as valid interior points.
// Also, the initial and final y scan lines returned by the iterator
// are interior to the polygon.  The constructor argument, win, is a clipping
// window that is intersected with the polygonal region to determine the actual
// scanned area.
//
// Example usage:
//
//     vgl_polygon_scan_iterator psi(mypoints);
//     psi.set_include_boundary(true); // optional flag, default is true
//     for(psi.reset(); psi.next(); ) {
// 	 int y = psi.scany();
// 	 for(int x = psi.startx(); x <= psi.endx(); ++x)
// 	       ....
//     }
// 
//
// .SECTION Author 
//    Adapted from FillPolygon by J.L. Mundy 
//
// .SECTION Modifications:
//     May 1997 - Added a constructor variable, boundaryp, which defines 
//                whether or not the polygon boundary is included in the 
//                scan lines. The original design produced only interior 
//                points of the polygonal region.  It proved desirable to 
//                include an option where the boundary is included as well.
//
//     Jan 1998 - Andrew Fitzgibbon found a bug which occurs when a 
//                nearly horizontal polygon edge crosses an integer scan
//                line coordinate.  Problem was due to (int) cast of
//                floating point y-coordinate values in ::next() - JLM
//
//     May 1998 - Modified data structures to allow for polygons with
//                holes.  Currently only iterators constructed from
//                a Face object can handle holes.  Iterators constructed 
//                from a list of IUPoints or a list of Vertex cannot handle 
//                holes.  - RYF
//
#include <vgl/vgl_region_scan_iterator.h>

/*struct*/class vgl_polygon_scan_iterator : public vgl_region_scan_iterator
{
public:
  // Stores coordinates of a 2d point
  typedef vgl_polygon::point_t Point2;
  
  vgl_polygon_scan_iterator(vgl_polygon const& face, bool boundaryp = true);
  vgl_polygon_scan_iterator(vgl_polygon const& face, bool boundaryp, vgl_box_2d<float> const& window);
  ~vgl_polygon_scan_iterator();

  // Resets iterator to first segment of first scan line
  void reset();

  // Moves iterator to next segment
  bool next();

  // returns current scan line
  int scany() const { return (y-1); }

  // returns start of current span
  int startx() const { return xl; }

  // returns end of current span
  int endx() const { return xr; }

  // are these floating point versions ?
  float fstartx() { return fxl; }
  float fendx() { return fxr; }
  float fscany() { return fy; }

  // Vertex index -- uniquely identifies a vertex in the array chains
  struct vertind {
    int chainnum;    // which chain the vertex is part of
    int vertnum;     // which vertex in the chain
    void display( char const * str );
  };

  // Describes an edge crossing the current scan line
  struct crossedge {
    float x;	// x coord of edge's intersection with current scanline
    float dx;	// change in x with respect to y 
    vertind v;      // edge goes from vertex v.vertnum to v.vertnum + 1
  };

private:

  int boundp;       // boolean indicating if boundary should be included or not
  int xl;           // left bound of current span
  float fxl;        // left bound of current span (float)
  int xr;           // right bound of current span
  float fxr;        // right bound of current span (float)
  int k;            // current index of vertices ordered by increasing y
  int y0;           // bottommost scan line
  int y1;           // topmost scan line
  int y;            // current scan line
  float fy;         // floating point value of current scan line (i.e. float(y))
  int curcrossedge; // crossedge marking start of next scan segment
  vgl_box_2d<float> win;       // clipping window
  bool have_window;

  vgl_polygon poly_;

  vertind * yverts;       // array of all vertices ordered by y coordinate
  crossedge * crossedges; // array of edges crossing current scan line
  int numcrossedges;      // number of edges currently crossing scan line
  int numverts;           // total number of vertices comprising face

  // Returns x coord of vertex v
  float get_x( vertind v ) const {return (poly_[v.chainnum][v.vertnum]).x();}

  // Returns y coord of vertex v
  float get_y( vertind v ) const {return (poly_[v.chainnum][v.vertnum]).y();}

  // Returns vertex v
  Point2 get_pt( vertind v ) {return (poly_[v.chainnum][v.vertnum]);}

  // assumes poly_, win, have_window, boundp are set
  void init();

  // Deletes edge (v,get_next_vert(v)) from crossedges array
  void delete_edge( vertind v );

  // Inserts edge (v,get_next_vert(v)) into crossedges array
  void insert_edge( vertind v );

  // Returns next vertex on chain
  void get_next_vert( vertind v, vertind & next );

  // Returns prev vertex on chain
  void get_prev_vert( vertind v, vertind & prev );

  // For debugging purposes
  void display_chains();
  void display_crossedges();

};

#endif
